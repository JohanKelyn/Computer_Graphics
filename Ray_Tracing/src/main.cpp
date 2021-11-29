////////////////////////////////////////////////////////////////////////////////
// C++ include
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <queue>

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

struct Node {
        AlignedBox3d bbox;
        int triangle;
        Node* left;
        Node* right;
        Node* parent;
};

struct AABBTree {
	Node* root;
    std::vector<Node*> nodes;

    AABBTree() = default; // Default empty constructor
    AABBTree(const MatrixXd vertices, const MatrixXi facets);
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

AlignedBox3d merge(AlignedBox3d box1, AlignedBox3d box2){
	double box1_min_x = box1.min()[0];
	double box2_min_x = box2.min()[0];
	double box1_max_x = box1.max()[0];
	double box2_max_x = box2.max()[0];
	double box1_min_y = box1.min()[1];
	double box2_min_y = box2.min()[1];
	double box1_max_y = box1.max()[1];
	double box2_max_y = box2.max()[1];
	double box1_min_z = box1.min()[2];
	double box2_min_z = box2.min()[2];
	double box1_max_z = box1.max()[2];
	double box2_max_z = box2.max()[2];
	double max_x = std::max(box1_max_x, box2_max_x);
	double min_x = std::min(box1_min_x, box2_min_x);
	double max_y = std::max(box1_max_y, box2_max_y);
	double min_y = std::min(box1_min_y, box2_min_y);
	double max_z = std::max(box1_max_z, box2_max_z);
	double min_z = std::min(box1_min_z, box2_min_z);

	AlignedBox3d box(Vector3d(min_x, min_y, min_z), Vector3d(max_x, max_y, max_z));
	return box;
}

AABBTree::AABBTree(const MatrixXd V, const MatrixXi F){
    std::vector<Node*> S;
	for(int i=0; i<F.rows(); ++i){
		Node* n = new Node;
		n->bbox = bbox_triangle(V.row(F(i,0)), V.row(F(i,1)), V.row(F(i,2)));
		n->triangle = i;
		n->left = NULL;
		n->right = NULL;
		nodes.push_back(n);
		S.push_back(n);
	}
	int k = 0;
	int length = S.size();
	int idx;
	while(length > 1){
		double mindist = INFINITY;
		for(int i=1; i<length; ++i){
			if((S[0]->bbox.center() - S[i]->bbox.center()).norm() < mindist){
				mindist = std::min((S[0]->bbox.center() - S[i]->bbox.center()).norm(),mindist);
				idx = i;
				}
		}
        
		Node* n2 = new Node;
		n2->bbox = merge(S[0]->bbox, S[idx]->bbox);
		n2->triangle = -1;
		nodes.push_back(n2);
		n2->left = S[0];
		n2->right = S[idx];
		S.at(0)->parent = n2;
		S.at(idx)->parent = n2;
		S.erase(S.begin()+idx);
		S.erase(S.begin());
		S.push_back(n2);
		k++;
		length = S.size();
	}

	root = nodes.back();
	root->parent = NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool Sphere::intersect(const Ray &ray, Intersection &hit) {
	Vector3d origin_to_center = ray.origin - position;
    auto a = ray.direction.dot(ray.direction);
    auto b = 2.0 * ray.direction.dot(origin_to_center);
    auto c = origin_to_center.dot(origin_to_center) - radius * radius;
    auto discriminant = b*b - 4*a*c;
	
	if (discriminant > 0){
		auto root = sqrt(discriminant);
		double temp0 = (- b - root)/(2*a);
		double temp1 = (- b + root)/(2*a);
		
		if(((ray.origin + temp0 * ray.direction).norm() > (ray.origin + temp1 * ray.direction).norm()) && (temp1 > 0)){
			hit.ray_param = temp1;
			hit.position = ray.origin + hit.ray_param * ray.direction;
			hit.normal = (hit.position - position).normalized();
			return true;
		}

		if(((ray.origin + temp0 * ray.direction).norm() < (ray.origin + temp1 * ray.direction).norm()) && (temp0 > 0)){
			hit.ray_param = temp0;
			hit.position = ray.origin + hit.ray_param * ray.direction;
			hit.normal = (hit.position - position).normalized();
			return true;
		}
		return false;
		
	}
	
	return false;
}

bool Parallelogram::intersect(const Ray &ray, Intersection &hit) {
	Vector3d y = ray.origin - origin;
	Matrix3d A = Matrix3d::Zero(3,3);
	A.col(0) = u;
	A.col(1) = v;
	A.col(2) = -ray.direction;

	Vector3d x = A.colPivHouseholderQr().solve(y);
	if ((x(0) >= 0 && x(0) <= 1 && x(1) >= 0 &&  x(1) <= 1 && x(2) >=0) && (x(0) + x(1)) <= 2) {
		hit.ray_param = x(2);
		hit.position = ray.origin + hit.ray_param * ray.direction;
		if (origin(2) <= ray.origin(2)){
			hit.normal = (u.cross(v)).normalized();
		}
		else{
			hit.normal = -1*(u.cross(v)).normalized();
		}

		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------

bool intersect_triangle(const Ray &ray, const Vector3d &a, const Vector3d &b, const Vector3d &c, Intersection &hit) {
	Vector3d y = ray.origin - a;
	Vector3d u = b - a;
	Vector3d v = c - a;
	Matrix3d A = Matrix3d::Zero(3,3);
	A.col(0) = u;
	A.col(1) = v;
	A.col(2) = -ray.direction;

	Vector3d x = A.colPivHouseholderQr().solve(y);
	if ((x(0) >= 0 && x(1) >= 0 && x(2) >=0) && (x(0) + x(1)) <= 1) {
		hit.ray_param = x(2);
		hit.position = ray.origin + hit.ray_param * ray.direction;
		if (a(2) <= ray.origin(2)){
			hit.normal = ((u).cross(v)).normalized();
		}
		else{
			hit.normal = -1*((u).cross(v)).normalized();
		}

		return true;
	}
	else return false;
}

bool intersect_box(const Ray &ray, const AlignedBox3d &box) {
	// Compute whether the ray intersects the given box.
	// There is no need to set the resulting normal and ray parameter, since
	// we are not testing with the real surface here anyway
	// Compute whether the ray intersects the given box.
	// There is no need to set the resulting normal and ray parameter, since
	// we are not testing with the real surface here anyway.
	double t_min_x, t_min_y, t_min_z, t_max_x, t_max_y, t_max_z;
	double a, b, c;
	a = 1/ray.direction[0];
	b = 1/ray.direction[1];
	c = 1/ray.direction[2];

	if(a >= 0){
		t_min_x = (box.min()[0] - ray.origin[0])*a;
		t_max_x = (box.max()[0] - ray.origin[0])*a;
	}
	else{
		t_min_x = (box.max()[0] - ray.origin[0])*a;
		t_max_x = (box.min()[0] - ray.origin[0])*a;
	}
	
	if(b >= 0){
		t_min_y = (box.min()[1] - ray.origin[1])*b;
		t_max_y = (box.max()[1] - ray.origin[1])*b;
	}
	else{
		t_min_y = (box.max()[1] - ray.origin[1])*b;
		t_max_y = (box.min()[1] - ray.origin[1])*b;
	}

	if(c >= 0){
		t_min_z = (box.min()[2] - ray.origin[2])*c;
		t_max_z = (box.max()[2] - ray.origin[2])*c;
	}
	else{
		t_min_z = (box.max()[2] - ray.origin[2])*c;
		t_max_z = (box.min()[2] - ray.origin[2])*c;
	}

	double t_min = std::max(std::min(t_min_x, t_max_x), std::max(std::min(t_min_y, t_max_y), std::min(t_min_z, t_max_z)));
	double t_max = std::min(std::max(t_min_x, t_max_x), std::min(std::max(t_min_y, t_max_y), std::max(t_min_z, t_max_z)));

	if(t_min <= t_max){
		return true;
	}
	return false;
}

bool Mesh::intersect(const Ray &ray, Intersection &closest_hit) {
	int method = 2;
	
	if(method == 1){
		Intersection hit;
		double closest = INFINITY;
		bool hit_happened = false;
		// Method (1): Traverse every triangle and return the closest hit.
		for(int i = 0; i < facets.rows(); i++){ 
			Vector3d v1 = vertices.row(facets.row(i)[0]);
			Vector3d v2 = vertices.row(facets.row(i)[1]);
			Vector3d v3 = vertices.row(facets.row(i)[2]);
		
			if(intersect_triangle(ray, v1, v2, v3, hit)){
				if(hit.ray_param < closest){
					closest = hit.ray_param;
					closest_hit = hit;
					hit_happened = true;
				}
			}
		}
		return hit_happened;
	}
	// Method (2): Traverse the BVH tree and test the intersection with a
	// triangles at the leaf nodes that intersects the input ray.
	//return false;
	bool hittrinagleleft = false;
	bool hittriangleright = false;
	double closest1 = INFINITY;
	double closest2 = INFINITY;
	Intersection hit1;
	Intersection hit2;
	Intersection closesthit1;
	Intersection closesthit2;
	std::queue<Node*> q;
	
	if(method == 2){
		if(intersect_box(ray, bvh.root->bbox)){
			//std::cout << "intersect root";
			q.push(bvh.root);
			while(q.size() != 0){
				Node* u = q.front();
				q.pop();
				Node* v = u->left;
				Node* w = u->right;
				if(v != NULL && v->triangle == -1){
					if(intersect_box(ray, v->bbox)){
						q.push(v);
					}
				}
				else if (v != NULL && v->triangle != -1){
					//std::cout << "Intersection triangle " << u->right->triangle << "\n";
					if(intersect_triangle(ray, vertices.row(facets(v->triangle, 0)), vertices.row(facets(v->triangle, 1)), vertices.row(facets(v->triangle, 2)), hit1)){
						if(hit1.ray_param < closest1){
							closest1 = hit1.ray_param;
							closesthit1 = hit1;
						}
						hittrinagleleft = true;
						//closesthit
						//std::cout << "Intersection triangle " << v->triangle << "\n";
					}
				}
				if(w != NULL && w->triangle == -1){
					if(intersect_box(ray, w->bbox)){
						q.push(w);
					}
				}
				else if (w != NULL && w->triangle != -1){
					//std::cout << "Intersection triangle " << u->right->triangle << "\n";
					if(intersect_triangle(ray, vertices.row(facets(w->triangle, 0)), vertices.row(facets(w->triangle, 1)), vertices.row(facets(w->triangle, 2)), hit2)){
						if(hit2.ray_param < closest2){
							closest2 = hit2.ray_param;
							closesthit2 = hit2;
						}
						hittriangleright = true;
						
						//std::cout << "Intersection triangle " << u->right->triangle << "\n";
					}
				}
			}
			// hit1.pay_param < closest_hit.ray_param  ----> closest_hit =  hit1
			// hit2.pay_param < closest_hit.ray_param  ----> closest_hit =  hit2
			if(hittriangleright && hittrinagleleft){
				if(closesthit1.ray_param > closesthit2.ray_param){
					closest_hit = closesthit2;
				}
				else{
					closest_hit = closesthit1;
				}
				return true;
			}
			else if(hittriangleright == true && hittrinagleleft == false){
				closest_hit = closesthit2;
				return true;
			}
			else if(hittrinagleleft == true && hittriangleright == false){
				closest_hit = closesthit1;
				return true;
			}
			else{
				return false;
			}
	}
		else return false;
	} return false;
}

////////////////////////////////////////////////////////////////////////////////
// Define ray-tracing functions
////////////////////////////////////////////////////////////////////////////////

// Function declaration here (could be put in a header file)
Vector3d ray_color(const Scene &scene, const Ray &ray, const Object &object, const Intersection &hit, int max_bounce);
Object * find_nearest_object(const Scene &scene, const Ray &ray, Intersection &closest_hit);
bool is_light_visible(const Scene &scene, const Ray &ray, const Light &light);
Vector3d shoot_ray(const Scene &scene, const Ray &ray, int max_bounce);

// -----------------------------------------------------------------------------

Vector3d ray_color(const Scene &scene, const Ray &ray, const Object &obj, const Intersection &hit, int max_bounce) {
	// Material for hit object
	const Material &mat = obj.material;

	// Ambient light contribution
	Vector3d ambient_color = obj.material.ambient_color.array() * scene.ambient_light.array();

	// Punctual lights contribution (direct lighting)
	Vector3d lights_color(0, 0, 0);
	Vector3d reflection_color(0, 0, 0);
	Vector3d refraction_color(0, 0, 0);

	for (const Light &light : scene.lights) {
		Vector3d Li = (light.position - hit.position).normalized();
		Vector3d N = hit.normal;

		// Shadow rays
		float eps = 0.1;
		Ray shadowray;
		shadowray.direction = (light.position - hit.position).normalized();
		shadowray.origin = hit.position + eps * shadowray.direction;

		if(is_light_visible(scene, shadowray, light)){
			// Diffuse contribution
			Vector3d diffuse = mat.diffuse_color * std::max(Li.dot(N), 0.0);

			// Specular contribution
			Vector3d Vi = (scene.camera.position - hit.position).normalized();
			Vector3d h_unit = (Li + Vi).normalized();
			Vector3d specular = mat.specular_color * pow(std::max(h_unit.dot(N), 0.0), mat.specular_exponent);

			// Attenuate lights according to the squared distance to the lights
			Vector3d D = light.position - hit.position;
			lights_color += (diffuse + specular).cwiseProduct(light.intensity) /  D.squaredNorm();
		}
	}
	
	Vector3d v = (scene.camera.position - hit.position).normalized();
	// Reflected Ray
	if((obj.material.reflection_color.norm() <= sqrt(3)) && (obj.material.reflection_color.norm() > 0) && max_bounce > 0){
		Ray reflectionRay; 
		reflectionRay.direction = 2*hit.normal*(hit.normal.dot(v)) - v;
		reflectionRay.origin = hit.position + 0.0001 * reflectionRay.direction;
		reflection_color = shoot_ray(scene, reflectionRay, max_bounce-1);
		reflection_color = reflection_color.cwiseProduct(mat.reflection_color);
	}

	// Refracted Ray
	if((obj.material.refraction_color.norm() <= 3) && (obj.material.refraction_color.norm() > 0)){
		Ray refractionRay; 
		auto nv = hit.normal.dot(v);
		auto ri = obj.material.refraction_index;
		refractionRay.direction = (ri * nv - sqrt(1 - (ri * ri) * (1 - nv * nv))) * hit.normal - ri * v;
		refractionRay.origin = hit.position + 0.0001 * refractionRay.direction;
		refraction_color = shoot_ray(scene, refractionRay, max_bounce-1);
		refraction_color = refraction_color.cwiseProduct(mat.refraction_color);
	}
	
			

	// Rendering equation
	Vector3d C = ambient_color + lights_color + reflection_color + refraction_color;

	return C;
}

// -----------------------------------------------------------------------------

Object * find_nearest_object(const Scene &scene, const Ray &ray, Intersection &closest_hit) {
	int closest_index = -1;
	Intersection hit;
	// Find the object in the scene that intersects the ray first
	// The function must return 'nullptr' if no object is hit, otherwise it must
	// return a pointer to the hit object, and set the parameters of the argument
	// 'hit' to their expected values.
	double min_dist = INFINITY;
	for(int i = 0; i < scene.objects.size(); ++i){
		if (scene.objects[i]->intersect(ray, hit)){
			double dist = (hit.position - ray.origin).norm();
			if(min_dist > dist){ 
				min_dist = dist;
				closest_hit = hit;
				closest_index = i;
			}
		}
	}

	if (closest_index < 0) {
		// Return a NULL pointer
		return nullptr;
	} else {
		// Return a pointer to the hit object. Don't forget to set 'closest_hit' accordingly!
		return scene.objects[closest_index].get();
	}
}

bool is_light_visible(const Scene &scene, const Ray &ray, const Light &light) {
	Intersection hitObject;
	bool visible = true;
	int counter = 0;
	for(unsigned i=0; i<scene.objects.size(); ++i){
		if(scene.objects[i]->intersect(ray,hitObject)){
			visible = false;
		}
	}
	return visible;
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
	int iter = 3;
	int max_bounce = 2;
	MatrixXd R = MatrixXd::Zero(w, h);
	MatrixXd G = MatrixXd::Zero(w, h);
	MatrixXd B = MatrixXd::Zero(w, h);
	MatrixXd A = MatrixXd::Zero(w, h); // Store the alpha mask

	// The camera always points in the direction -z
	// The sensor grid is at a distance 'focal_length' from the camera center,
	// and covers a viewing angle given by 'field_of_view'.
	double aspect_ratio = double(w) / double(h);
	double scale_y = scene.camera.focal_length * tan(scene.camera.field_of_view/2); // TODO: Stretch the pixel grid by the proper amount here
	double scale_x = scale_y * aspect_ratio; //

	// The pixel grid through which we shoot rays is at a distance 'focal_length'
	// from the sensor, and is scaled from the canonical [-1,1] in order
	// to produce the target field of view.
	Vector3d grid_origin(-scale_x, scale_y, scene.camera.position[2]-scene.camera.focal_length);
	Vector3d x_displacement(2.0/w*scale_x, 0, 0);
	Vector3d y_displacement(0, -2.0/h*scale_y, 0);

	for (unsigned i = 0; i < w; ++i) {
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Ray tracing: " << (100.0 * i) / w << "%\r" << std::flush;
		for (unsigned j = 0; j < h; ++j) {
			Vector3d C = Vector3d::Zero();
			for (unsigned k = 0; k < iter; ++k){
				Vector3d x = Vector3d::Random()*scene.camera.lens_radius;
				x(2) = 0;
				Vector3d shift = grid_origin + (i+0.5)*x_displacement + (j+0.5)*y_displacement;

				// Prepare the ray
				Ray ray;

				if (scene.camera.is_perspective) {
					// Perspective camera
					ray.origin = scene.camera.position + x;
					ray.direction = shift - ray.origin;
				} else {
					// Orthographic camera
					ray.origin = scene.camera.position + Vector3d(shift[0], shift[1], 0);
					ray.direction = Vector3d(0, 0, -1);
				}
				C += shoot_ray(scene, ray, max_bounce);
			}
			C = C/iter;
			
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
			parallelogram->origin = read_vec3(entry["Origin"]);
			parallelogram->u = read_vec3(entry["u"]) - parallelogram->origin;
			parallelogram->v = read_vec3(entry["v"]) - parallelogram->origin;
			object = parallelogram;
		} else if (entry["Type"] == "Mesh") {
			// Load mesh from a file
			std::string filename = std::string(DATA_DIR) + entry["Path"].get<std::string>();
			object = std::make_shared<Mesh>(filename);
		}
		object->material = scene.materials[entry["Material"]];
		scene.objects.push_back(object);
	}

	return scene;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	std::string filename = "";
	if (argc < 2) {
		filename = "../data/scene.json";
	}
	else{
		filename = argv[1];
	}
	Scene scene = load_scene(filename);
	render_scene(scene);

	return 0;
}
