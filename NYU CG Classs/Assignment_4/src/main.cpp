////////////////////////////////////////////////////////////////////////////////
// C++ include
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <math.h>

// Eigen for matrix operations
#include <Eigen/Dense>
#include <Eigen/Geometry>

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"
#include "utils.h"

// JSON parser library (https://github.com/nlohmann/json)
#include "json.hpp"
using json = nlohmann::json;

// Shortcut to avoid Eigen:: everywhere, DO NOT USE IN .h
using namespace Eigen;

////////////////////////////////////////////////////////////////////////////////
// Define types & classes
////////////////////////////////////////////////////////////////////////////////

struct Ray {
	Vector3d origin;
	Vector3d direction;
	Ray() { }
	Ray(Vector3d o, Vector3d d) : origin(o), direction(d) { }
};

struct Light {
	Vector3d position;
	Vector3d intensity;
};

struct Intersection {
	Vector3d position;
	Vector3d normal;
	double ray_param;
};

struct Camera {
	bool is_perspective;
	Vector3d position;
	double field_of_view; // between 0 and PI
	double focal_length;
	double lens_radius; // for depth of field
};

struct Material {
	Vector3d ambient_color;
	Vector3d diffuse_color;
	Vector3d specular_color;
	double specular_exponent; // Also called "shininess"

	Vector3d reflection_color;
	Vector3d refraction_color;
	double refraction_index;
};

struct Object {
	Material material;
	virtual ~Object() = default; // Classes with virtual methods should have a virtual destructor!
	virtual bool intersect(const Ray &ray, Intersection &hit) = 0;
};

// We use smart pointers to hold objects as this is a virtual class
typedef std::shared_ptr<Object> ObjectPtr;

struct Sphere : public Object {
	Vector3d position;
	double radius;

	virtual ~Sphere() = default;
	virtual bool intersect(const Ray &ray, Intersection &hit) override;
};

struct Parallelogram : public Object {
	Vector3d origin;
	Vector3d u;
	Vector3d v;

	virtual ~Parallelogram() = default;
	virtual bool intersect(const Ray &ray, Intersection &hit) override;
};

struct AABBTree {
	struct Node {
		AlignedBox3d bbox;
		int parent; // Index of the parent node (-1 for root)
		int left; // Index of the left child (-1 for a leaf)
		int right; // Index of the right child (-1 for a leaf)
		int triangle; // Index of the node triangle (-1 for internal nodes)
	};

	std::vector<Node> nodes;
	int root;

	AABBTree() = default; // Default empty constructor
	AABBTree(const MatrixXd &V, const MatrixXi &F); // Build a BVH from an existing mesh
};

struct Mesh : public Object {
	MatrixXd vertices; // n x 3 matrix (n points)
	MatrixXi facets; // m x 3 matrix (m triangles)

	AABBTree bvh;

	Mesh() = default; // Default empty constructor
	Mesh(const std::string &filename);
	virtual ~Mesh() = default;
	virtual bool intersect(const Ray &ray, Intersection &hit) override;
};

struct Scene {
	Vector3d background_color;
	Vector3d ambient_light;

	Camera camera;
	std::vector<Material> materials;
	std::vector<Light> lights;
	std::vector<ObjectPtr> objects;
};

////////////////////////////////////////////////////////////////////////////////

// Read a triangle mesh from an off file
void load_off(const std::string &filename, MatrixXd &V, MatrixXi &F) {
	std::ifstream in(filename);
	std::string token;
	in >> token;
	int nv, nf, ne;
	in >> nv >> nf >> ne;
	V.resize(nv, 3);
	F.resize(nf, 3);
	for (int i = 0; i < nv; ++i) {
		in >> V(i, 0) >> V(i, 1) >> V(i, 2);
	}
	for (int i = 0; i < nf; ++i) {
		int s;
		in >> s >> F(i, 0) >> F(i, 1) >> F(i, 2);
		assert(s == 3);
	}
}

Mesh::Mesh(const std::string &filename) {
	// Load a mesh from a file (assuming this is a .off file), and create a bvh
	load_off(filename, vertices, facets);
	bvh = AABBTree(vertices, facets);
}

////////////////////////////////////////////////////////////////////////////////
// BVH Implementation
////////////////////////////////////////////////////////////////////////////////

// Bounding box of a triangle
AlignedBox3d bbox_triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c) {
	AlignedBox3d box;
	box.extend(a);
	box.extend(b);
	box.extend(c);
	return box;
}

int createNode(const MatrixXd &V, const MatrixXi &F, const MatrixXd &Centroids, const std::vector<int> &triangles, AABBTree *tree){
	// base condition for Top-Down method
	int size = triangles.size();
	// std::cout << "size: " << size << " ";
	if (size == 1) {
		AlignedBox3d root_box;
		for (int i = 0; i < F.cols(); i++) {
			root_box.extend((Vector3d)V.row(F(triangles[0], i)));
		}
		AABBTree::Node root_node;
		root_node.bbox = root_box;
		root_node.triangle = triangles[0];
		tree->nodes.push_back(root_node);
		return tree->nodes.size() - 1;
	}

	// get the box contains all triangles passed in
	AlignedBox3d root_box;
	// Get the biggest box contains all vertices
	for (int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < F.cols(); j++) {
			root_box.extend((Vector3d)V.row(F(triangles[i], j)));
		}
	}
	// get the longest axis of the root box
	Vector3d sizes = root_box.sizes();
	int axis = 0;
	for (int i = 1; i < V.cols(); i++) {
		if (sizes(i) > sizes(axis)) axis = i;
	}
	// Sort the centroids
	std::vector<std::pair<double, int>> centros;
	for (int i = 0; i < triangles.size(); i++) {
		centros.push_back(std::make_pair(Centroids.row(triangles[i])(axis), triangles[i]));
	}
	std::sort(centros.begin(), centros.end(), [](const std::pair<double, int>& c1, const std::pair<double, int>& c2) {
		return c1.first < c2.first;
	});

	// new sorted triangles
	std::vector<int> new_triangles; // vector stores the index of sorted triangles
	for (std::pair<double, int> p : centros) {
		new_triangles.push_back(p.second);
	}

	std::size_t const half_size = size / 2;
	std::vector<int> left(new_triangles.begin(), new_triangles.begin() + half_size);
	std::vector<int> right(new_triangles.begin() + half_size, new_triangles.end());
	int left_node = createNode(V, F, Centroids, left, tree);
	int right_node = createNode(V, F, Centroids, right, tree);
	AABBTree::Node root_node;
	root_node.bbox = root_box;
	root_node.left = left_node;
	root_node.right = right_node;
	tree->nodes[left_node].parent = tree->nodes.size();
	tree->nodes[right_node].parent = tree->nodes[left_node].parent;
	root_node.triangle = -1; // for internal node
	tree->nodes.push_back(root_node);
	return tree->nodes.size() - 1;
}

AABBTree::AABBTree(const MatrixXd &V, const MatrixXi &F) {
	// Compute the centroids of all the triangles in the input mesh
	MatrixXd centroids(F.rows(), V.cols());
	centroids.setZero();
	for (int i = 0; i < F.rows(); ++i) {
		for (int k = 0; k < F.cols(); ++k) {
			centroids.row(i) += V.row(F(i, k));
		}
		centroids.row(i) /= F.cols();
	}

	// Method (1): Top-down approach.
	// Split each set of primitives into 2 sets of roughly equal size,
	// based on sorting the centroids along one direction or another.
	
	// get the index of all triangles
	std::vector<int> triangles; // vector stores the index of sorted triangles
	for (int i = 0; i < F.rows(); i++) {
		triangles.push_back(i);
	}

	this->root = createNode(V, F, centroids, triangles, this);
	
	// Method (2): Bottom-up approach.
	// Merge nodes 2 by 2, starting from the leaves of the forest, until only 1 tree is left.
}

////////////////////////////////////////////////////////////////////////////////

bool Sphere::intersect(const Ray &ray, Intersection &hit) {
	// Compute the intersection between the ray and the sphere
	// If the ray hits the sphere, set the result of the intersection in the
	// struct 'hit'
	float a = ray.direction.dot(ray.direction);
	float b = 2*(ray.direction).dot(ray.origin - position);
	float c = (ray.origin - position).dot(ray.origin - position) - radius * radius;

	double discriminant = (b*b - 4*a*c);

	if( discriminant < 0) return false;
	
	float t1 = (-b - sqrt(discriminant)) / (2*a);
	float t2 = (-b + sqrt(discriminant)) / (2*a);

	if (t2 < 0) return false;
	if (t1 < 0) {
		hit.ray_param = t2;
		hit.position = ray.origin + t2 * ray.direction;
	}
	else {
		hit.ray_param = t1;
		hit.position = ray.origin + hit.ray_param * ray.direction;
	}

	if(ray.direction.dot(hit.position - position) < 0) hit.normal = (hit.position - position).normalized();
	else hit.normal = -(hit.position - position).normalized();
	
	
	return true;
}

bool Parallelogram::intersect(const Ray &ray, Intersection &hit) {
	// Check if the ray intersects with the parallelogram
		Matrix3d M;
		M.col(0) = origin - u;
		M.col(1) = origin - v;
		M.col(2) = ray.direction;

		Vector3d y = origin - ray.origin;

		Vector3d x = M.colPivHouseholderQr().solve(y);

		if (x(2) > 0 && x(0) >= 0 && x(0) <= 1 && x(1) >= 0 && x(1) <= 1) {
			// The ray hit the parallelogram, compute the exact intersection point
			hit.position = ray.origin + x(2) * ray.direction;

			Vector3d normal = ((u - origin).cross(v - origin)).normalized();

			hit.normal = (ray.direction.dot(normal) < 0) ? normal : -normal; 
			// Hit Parameter
			hit.ray_param = x(2);

			return true;
		}

	return false;
}

// -----------------------------------------------------------------------------

bool intersect_triangle(const Ray &ray, const Vector3d &a, const Vector3d &b, const Vector3d &c, Intersection &hit) {
	// Check if the ray intersects with the parallelogram
		Matrix3d M;
		M.col(0) = a - b;
		M.col(1) = a - c;
		M.col(2) = ray.direction;

		Vector3d y = a - ray.origin;

		Vector3d x = M.colPivHouseholderQr().solve(y);

		if (x(2) > 0 && x(0) >= 0 && x(1) >= 0 && x(1) + x(0) <= 1) {
			// The ray hit the parallelogram, compute the exact intersection point
			hit.position = ray.origin + x(2) * ray.direction;

			Vector3d normal = ((b - a).cross(c - a)).normalized();

			hit.normal = (ray.direction.dot(normal) < 0) ? normal : -normal; 
			// Hit Parameter
			hit.ray_param = x(2);

			return true;
		}

	return false;
}

bool intersect_box(const Ray &ray, const AlignedBox3d &box) {
	// Compute whether the ray intersects the given box.
	// There is no need to set the resulting normal and ray parameter, since
	// we are not testing with the real surface here anyway.
	Vector3d min = box.min();
	Vector3d max = box.max();
	double t_min = std::numeric_limits<double>::min();
	double t_max = std::numeric_limits<double>::max();
	for (int a = 0; a < 3; a++) {
		auto t0 = fmin((min(a) - ray.origin(a)) / ray.direction(a),
					   (max(a) - ray.origin(a)) / ray.direction(a));
		auto t1 = fmax((min(a) - ray.origin(a)) / ray.direction(a),
					   (max(a) - ray.origin(a)) / ray.direction(a));
		t_min = fmax(t0, t_min);
		t_max = fmin(t1, t_max);
		if (t_max <= t_min)
			return false;
	}
	return true;
}

bool Mesh::intersect(const Ray &ray, Intersection &closest_hit) {
	// TODO (Assignment 3)
	bool intersection_happened = false;
	/*
	// Method (1): Traverse every triangle and return the closest hit.
	Intersection hit;
	double closest_distance = 1000000;
	for(unsigned i = 0; i < facets.rows(); i++) {
		Vector3d a = vertices.row(facets.row(i)(0));
		Vector3d b = vertices.row(facets.row(i)(1));
		Vector3d c = vertices.row(facets.row(i)(2));
		if(intersect_triangle(ray, a, b, c, hit)) {
			double distance_to_hit = (hit.position - ray.origin).norm();
			closest_distance = distance_to_hit < closest_distance ? distance_to_hit : closest_distance;
			intersection_happened = true;
		}
	}
	closest_hit = hit;
	*/

	
	// Method (2): Traverse the BVH tree and test the intersection with a
	// triangles at the leaf nodes that intersects the input ray.
	AABBTree::Node node = bvh.nodes[bvh.root];
	
	// use two vectors to store all intersected nodes
	std::vector<AABBTree::Node> internal_nodes;
	internal_nodes.push_back(node);
	std::vector<AABBTree::Node> leaf_nodes;
	while (!internal_nodes.empty()) { // still have internal node to verify
		node = internal_nodes.back();
		internal_nodes.pop_back();
		if (node.triangle != -1) { // this is a leaf node
			leaf_nodes.push_back(node);
			continue;
		}
		if (intersect_box(ray, bvh.nodes[node.left].bbox)) {
			// intersect with the left subTree
			internal_nodes.push_back(bvh.nodes[node.left]);
		}
		if (intersect_box(ray, bvh.nodes[node.right].bbox)) {
			// intersect with the right subTree
			internal_nodes.push_back(bvh.nodes[node.right]);
		}
	}

	// search the leaf node
	closest_hit.ray_param = std::numeric_limits<double>::max();
	Intersection hit;
	int triangle;
	for (int i = 0; i < leaf_nodes.size(); i++) {
		triangle = leaf_nodes[i].triangle;
		if (intersect_triangle(ray, 
							   vertices.row(facets(triangle, 0)),
							   vertices.row(facets(triangle, 1)), 
							   vertices.row(facets(triangle, 2)), 
							   hit)) {
			if (hit.ray_param < closest_hit.ray_param) {
				closest_hit.ray_param = hit.ray_param;
				closest_hit.position = hit.position;
				closest_hit.normal = hit.normal;
				intersection_happened = true;
			}
		}
	}
	

	return intersection_happened;
}

////////////////////////////////////////////////////////////////////////////////
// Define ray-tracing functions
////////////////////////////////////////////////////////////////////////////////

// Function declaration here (could be put in a header file)
Vector3d ray_color(const Scene &scene, const Ray &ray, const Object &object, const Intersection &hit, int max_bounce);
Object * find_nearest_object(const Scene &scene, const Ray &ray, Intersection &closest_hit);
bool is_light_visible(const Scene &scene, const Ray &ray, const Light &light);
Vector3d shoot_ray(const Scene &scene, const Ray &ray, int max_bounce);
bool refract(const Vector3d &d, const Vector3d &N, double n0, double n1, Vector3d &t);
// -----------------------------------------------------------------------------

bool refract(const Vector3d &d, const Vector3d &N, double n0, double n1, Vector3d &t) {
	double nr = n0 / n1;
	double c1 = (-d).dot(N);
	double cond = 1.0 - nr * nr * (1 - c1 * c1);
	if (cond < 0) return false;
	double c2 = sqrt(cond);
	t = nr * d + (nr * c1 - c2) * N;
	return true;
}

Vector3d ray_color(const Scene &scene, const Ray &ray, const Object &obj, const Intersection &hit, int max_bounce) {
	// Material for hit object
	const Material &mat = obj.material;
	float epsilon = 0.001;

	// Ambient light contribution
	Vector3d ambient_color = obj.material.ambient_color.array() * scene.ambient_light.array();

	// Punctual lights contribution (direct lighting)
	Vector3d lights_color(0, 0, 0);

	for (const Light &light : scene.lights) {
		Vector3d Li = (light.position - hit.position).normalized();
		Vector3d N = hit.normal;

		// Shoot a shadow ray to determine if the light should affect the intersection point
		Ray shadow_ray;	
		shadow_ray.origin = hit.position + epsilon * N;
		shadow_ray.direction = Li;
		bool in_shadow = is_light_visible(scene, shadow_ray, light);

		if(in_shadow == true) continue;
		// Diffuse contribution
		Vector3d diffuse = mat.diffuse_color * std::max(Li.dot(N), 0.0);

		// Specular contribution
		Vector3d v = (scene.camera.position - hit.position).normalized();
		Vector3d h = (v + Li).normalized();
		Vector3d specular = mat.specular_color * pow(std::max(h.dot(N), 0.0), mat.specular_exponent);

		// Attenuate lights according to the squared distance to the lights
		Vector3d D = light.position - hit.position;
		lights_color += (diffuse + specular).cwiseProduct(light.intensity) /  D.squaredNorm();
	}
	// Compute the color of the reflected ray and add its contribution to the current point color.
	Vector3d reflection_color(0, 0, 0);
	
	if (mat.reflection_color.norm() > 0 && max_bounce > 0) {
		Vector3d r = ray.direction - 2 * ray.direction.dot(hit.normal) * hit.normal;
		Ray reflection_ray(hit.position + epsilon * r, r);
		reflection_color += mat.reflection_color.cwiseProduct(shoot_ray(scene, reflection_ray, max_bounce - 1));
	}
	
	// Compute the color of the refracted ray and add its contribution to the current point color.
	// Make sure to check for total internal reflection before shooting a new ray.
	Vector3d refraction_color(0, 0, 0);
	Vector3d t;
	if (mat.refraction_color.norm() > 0 && max_bounce > 0) {
		if (ray.direction.normalized().dot(hit.normal) > 0) { // air to object
			if (refract(ray.direction, hit.normal, 1.0, mat.refraction_index, t)) {
				Ray refraction_ray(hit.position + epsilon * ray.direction, t);
				refraction_color += mat.refraction_color.cwiseProduct(shoot_ray(scene, refraction_ray, max_bounce - 1));
			}
		}
		else { // object to air
			if (refract(ray.direction, hit.normal, mat.refraction_index, 1.0, t)) {
				Ray refraction_ray(hit.position + epsilon * ray.direction, t);
				refraction_color += mat.refraction_color.cwiseProduct(shoot_ray(scene, refraction_ray, max_bounce - 1));
			}
		}
	}	
	// Rendering equation
	Vector3d C = ambient_color + lights_color + reflection_color + refraction_color;

	return C;
}

// -----------------------------------------------------------------------------

Object * find_nearest_object(const Scene &scene, const Ray &ray, Intersection &closest_hit) {
	int closest_index = -1;
	closest_hit.ray_param = std::numeric_limits<double>::max();
	Intersection hit;
	for (int i = 0; i < scene.objects.size(); i++) {
		if (scene.objects[i]->intersect(ray, hit)) {
			if (hit.ray_param < closest_hit.ray_param) {
				closest_hit.position = hit.position;
				closest_hit.normal = hit.normal;
				closest_hit.ray_param = hit.ray_param;
				closest_index = i;
			}
		}
	}
	// Find the object in the scene that intersects the ray first
	// The function must return 'nullptr' if no object is hit, otherwise it must
	// return a pointer to the hit object, and set the parameters of the argument
	// 'hit' to their expected values.
	if (closest_index < 0) {
		// Return a NULL pointer
		return nullptr;
	} else {
		// Return a pointer to the hit object. Don't forget to set 'closest_hit' accordingly!
		return scene.objects[closest_index].get();
	}
}

bool is_light_visible(const Scene &scene, const Ray &ray, const Light &light) {
	// Determine if the light is visible here
	bool in_shadow = false;
	Intersection hit;
	for(int i = 0; i < scene.objects.size(); i++) {
		if(scene.objects[i]->intersect(ray, hit)) {
			if (hit.ray_param < (light.position - ray.origin).norm()) return true;
		}
	}
	return false;
}

Vector3d shoot_ray(const Scene &scene, const Ray &ray, int max_bounce) {
	Intersection hit;
	if (Object * obj = find_nearest_object(scene, ray, hit)) {
		// 'obj' is not null and points to the object of the scene hit by the ray
		return ray_color(scene, ray, *obj, hit, max_bounce);
	} else {
		// 'obj' is null, we must return the background color
		return scene.background_color;
	}
}

////////////////////////////////////////////////////////////////////////////////

void render_scene(const Scene &scene) {
	std::cout << "Simple ray tracer." << std::endl;

	int w = 640;
	int h = 480;
	MatrixXd R = MatrixXd::Zero(w, h);
	MatrixXd G = MatrixXd::Zero(w, h);
	MatrixXd B = MatrixXd::Zero(w, h);
	MatrixXd A = MatrixXd::Zero(w, h); // Store the alpha mask

	// The camera always points in the direction -z
	// The sensor grid is at a distance 'focal_length' from the camera center,
	// and covers an viewing angle given by 'field_of_view'.
	double aspect_ratio = double(w) / double(h);
	double tan_half_view = std::tan(scene.camera.field_of_view / 2);
	double L = tan_half_view * scene.camera.focal_length;
	double scale_y = L; // TODO: Stretch the pixel grid by the proper amount here
	double scale_x = L * aspect_ratio; //
	

	// The pixel grid through which we shoot rays is at a distance 'focal_length'
	// from the sensor, and is scaled from the canonical [-1,1] in order
	// to produce the target field of view.
	Vector3d grid_origin(-scale_x, scale_y, -scene.camera.focal_length);
	Vector3d x_displacement(2.0/w*scale_x, 0, 0);
	Vector3d y_displacement(0, -2.0/h*scale_y, 0);
	unsigned rep = 5;

	for (unsigned i = 0; i < w; ++i) {
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Ray tracing: " << (100.0 * i) / w << "%\r" << std::flush;
		for (unsigned j = 0; j < h; ++j) {
			// Implement depth of field
			Vector3d C(0, 0, 0);
			int max_bounce = 5;

			Vector3d shift = grid_origin + (i+0.5)*x_displacement + (j+0.5)*y_displacement;

			// Prepare the ray
			Ray ray;

			if (scene.camera.is_perspective) {
				// Perspective camera
				for (unsigned k = 0; k < rep; k++) {
					double dx = (std::rand() / RAND_MAX * 2 - 1) * scene.camera.lens_radius;
					double dy = (std::rand() / RAND_MAX * 2 - 1) * scene.camera.lens_radius;
					ray.origin = scene.camera.position + Vector3d(dx, dy, 0);
					ray.direction = shift - ray.origin;
					C += shoot_ray(scene, ray, max_bounce)/rep;
				}	
			} else {
				// Orthographic camera
				ray.origin = scene.camera.position + Vector3d(shift[0], shift[1], 0);
				ray.direction = Vector3d(0, 0, -1);
				C = shoot_ray(scene, ray, max_bounce);
			}
				R(i, j) = C(0);
				G(i, j) = C(1);
				B(i, j) = C(2);
				A(i, j) = 1;	
		}
	}
	std::cout << "Ray tracing: 100%  " << std::endl;
	// Save to png
	const std::string filename("raytrace.png");
	write_matrix_to_png(R, G, B, A, filename);
}

////////////////////////////////////////////////////////////////////////////////

Scene load_scene(const std::string &filename) {
	Scene scene;

	// Load json data from scene file
	json data;
	std::ifstream in(filename);
	in >> data;

	// Helper function to read a Vector3d from a json array
	auto read_vec3 = [] (const json &x) {
		return Vector3d(x[0], x[1], x[2]);
	};

	// Read scene info
	scene.background_color = read_vec3(data["Scene"]["Background"]);
	scene.ambient_light = read_vec3(data["Scene"]["Ambient"]);

	// Read camera info
	scene.camera.is_perspective = data["Camera"]["IsPerspective"];
	scene.camera.position = read_vec3(data["Camera"]["Position"]);
	scene.camera.field_of_view = data["Camera"]["FieldOfView"];
	scene.camera.focal_length = data["Camera"]["FocalLength"];
	scene.camera.lens_radius = data["Camera"]["LensRadius"];

	// Read materials
	for (const auto &entry : data["Materials"]) {
		Material mat;
		mat.ambient_color = read_vec3(entry["Ambient"]);
		mat.diffuse_color = read_vec3(entry["Diffuse"]);
		mat.specular_color = read_vec3(entry["Specular"]);
		mat.reflection_color = read_vec3(entry["Mirror"]);
		mat.refraction_color = read_vec3(entry["Refraction"]);
		mat.refraction_index = entry["RefractionIndex"];
		mat.specular_exponent = entry["Shininess"];
		scene.materials.push_back(mat);
	}

	// Read lights
	for (const auto &entry : data["Lights"]) {
		Light light;
		light.position = read_vec3(entry["Position"]);
		light.intensity = read_vec3(entry["Color"]);
		scene.lights.push_back(light);
	}

	// Read objects
	for (const auto &entry : data["Objects"]) {
		ObjectPtr object;
		if (entry["Type"] == "Sphere") {
			auto sphere = std::make_shared<Sphere>();
			sphere->position = read_vec3(entry["Position"]);
			sphere->radius = entry["Radius"];
			object = sphere;
		} else if (entry["Type"] == "Parallelogram") {
			auto parallelogram = std::make_shared<Parallelogram>();
			parallelogram->origin = read_vec3(entry["Position"]);
			parallelogram->u = read_vec3(entry["u"]);
			parallelogram->v = read_vec3(entry["v"]);
			object = parallelogram;
		} else if (entry["Type"] == "Mesh") {
			// Load mesh from a file
			std::string filename = std::string("../data/") + entry["Path"].get<std::string>();
			object = std::make_shared<Mesh>(filename);
		}
		object->material = scene.materials[entry["Material"]];
		scene.objects.push_back(object);
	}

	return scene;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

	Scene scene = load_scene("./../data/scene.json");
	render_scene(scene);
	return 0;
}
