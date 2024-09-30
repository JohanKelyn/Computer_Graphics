// C++ include
#include <iostream>
#include <string>
#include <vector>

// Utilities for the Assignment
#include "utils.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"


// Shortcut to avoid Eigen:: everywhere, DO NOT USE IN .h
using namespace Eigen;

float red = 0.7;
float green = 0.1;
float blue = 0.8;

float ka = 0.2;
float kd = 0.7;
float ke = 0.8;


void raytrace_sphere() {
	std::cout << "Simple ray tracer, one sphere with orthographic projection" << std::endl;

	const std::string filename("sphere_orthographic.png");
	MatrixXd C = MatrixXd::Zero(800,800); // Store the color
	MatrixXd A = MatrixXd::Zero(800,800); // Store the alpha mask

	// The camera is orthographic, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	Vector3d x_displacement(2.0/C.cols(),0,0);
	Vector3d y_displacement(0,-2.0/C.rows(),0);

	// Single light source
	const Vector3d light_position(-1,1,1);

	// Camera position
	const Vector3d camera_position(0,0,1);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray
			Vector3d ray_origin = origin + double(i)*x_displacement + double(j)*y_displacement;
			Vector3d ray_direction(0,0,-1);

			// Intersect with the sphere
			Vector3d sphere_center(0.2, 0.3, 0.0);
			const double sphere_radius = 0.5;

			float a = ray_direction.dot(ray_direction);
			float b = 2*(ray_direction).dot(ray_origin - sphere_center);
			float c = (ray_origin - sphere_center).dot(ray_origin - sphere_center) - sphere_radius * sphere_radius;

			if((b*b - 4*a*c) > 0) {
				float t1 = (-b + sqrt(b*b - 4*a*c)) / (2*a);
				float t2 = (-b - sqrt(b*b - 4*a*c)) / (2*a);

				float t = abs(t1) > abs(t2) ? t2 : t1;

				Vector3d ray_intersection = ray_origin + t * ray_direction;

				Vector3d ray_normal = (ray_intersection - sphere_center).normalized();

				auto ambient = ka;

				auto diffuse = kd * (light_position-ray_intersection).normalized().dot(ray_normal);
				diffuse = std::max(diffuse,0.);

				auto h = (camera_position + light_position).normalized();
				auto specular = ke * pow(ray_normal.dot(h), 1000);
				specular = std::max(specular, 0.);

				C(i,j) = ambient + diffuse + specular;

				// Clamp to zero
				C(i,j) = std::max(C(i,j), 0.0);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		} 
	}
	// Save to png
	write_matrix_to_png(red*C,green*C,blue*C,A,filename);

}

void raytrace_parallelogram() {
	std::cout << "Simple ray tracer, one parallelogram with orthographic projection" << std::endl;

	const std::string filename("plane_orthographic.png");
	MatrixXd C = MatrixXd::Zero(800,800); // Store the color
	MatrixXd A = MatrixXd::Zero(800,800); // Store the alpha mask

	// The camera is orthographic, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	Vector3d x_displacement(2.0/C.cols(),0,0);
	Vector3d y_displacement(0,-2.0/C.rows(),0);

	// Parameters of the parallelogram (position of the lower-left corner + two sides)
	Vector3d a(-0.9, -0.9, 0.5); // Parallelogram origin
	Vector3d b(0.9,-0.9,0.5); // Side 1
	Vector3d c(-0.9,0.9,0.5); // Side 2

	// Single light source
	const Vector3d light_position(0,1,1);

	// Camera position
	const Vector3d camera_position(0,0,1);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray
			Vector3d ray_origin = origin + double(i)*x_displacement + double(j)*y_displacement;
			Vector3d ray_direction(0,0,-1);
			
			// Check if the ray intersects with the parallelogram
			Matrix3d M;
			M.col(0) = a - b;
			M.col(1) = a - c;
			M.col(2) = ray_direction;

			Vector3d y = a - ray_origin;

			Vector3d x = M.colPivHouseholderQr().solve(y);

			if (x(2) > 0 && x(0) > 0 && x(0) <= 1 && x(1) > 0 && x(1) <= 1 && (x(0) + x(1) <= 2) ) {
				// The ray hit the parallelogram, compute the exact intersection point
				Vector3d ray_intersection = ray_origin + x(2) * ray_direction;

				// Compute normal at the intersection point
				Vector3d ray_normal = ((b - a).cross(c - a)).normalized();

				auto ambient = ka;

				auto diffuse = kd * (light_position-ray_intersection).normalized().dot(ray_normal);
				diffuse = std::max(diffuse,0.);

				auto h = (camera_position + light_position).normalized();
				auto specular = ke * pow(ray_normal.dot(h), 1000);
				specular = std::max(specular, 0.);
				
				// Compute color
				C(i,j) = ambient + diffuse + specular;

				// Clamp to zero
				C(i,j) = std::max(C(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(red*C,green*C,blue*C,A,filename);
}

void raytrace_perspective() {
	std::cout << "Simple ray tracer, one parallelogram with perspective projection" << std::endl;

	const std::string filename("plane_perspective.png");
	MatrixXd C = MatrixXd::Zero(800,800); // Store the color
	MatrixXd A = MatrixXd::Zero(800,800); // Store the alpha mask

	// The camera is perspective, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	Vector3d x_displacement(2.0/C.cols(),0,0);
	Vector3d y_displacement(0,-2.0/C.rows(),0);

	// Parameters of the parallelogram (position of the lower-left corner + two sides)
	Vector3d a(-0.9, -0.9,0.0); // Parallelogram origin
	Vector3d b(0.9,-0.9,0.0); // Side 1
	Vector3d c(-0.9,0.9,0.0); // Side 2

	// Single light source
	const Vector3d light_position(-1,1,1);

	// Camera position
	const Vector3d camera_position(0,0,3);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray (origin point and direction)
			Vector3d ray_origin = camera_position;
			Vector3d ray_direction = (origin + double(i)*x_displacement + double(j)*y_displacement) - ray_origin;

			// Check if the ray intersects with the parallelogram
			Matrix3d M;
			M.col(0) = a - b;
			M.col(1) = a - c;
			M.col(2) = ray_direction;

			Vector3d y = a - ray_origin;

			Vector3d x = M.colPivHouseholderQr().solve(y);


			if (x(2) > 0 && x(0) > 0 && x(0) <= 1 && x(1) > 0 && x(1) <= 1 && (x(0) + x(1) <= 2) ) {
				// The ray hit the parallelogram, compute the exact intersection point
				Vector3d ray_intersection = ray_origin + x(2) * ray_direction;

				// Compute normal at the intersection point
				Vector3d ray_normal = ((b - a).cross(c - a)).normalized();

				auto ambient = ka;

				auto diffuse = kd * (light_position-ray_intersection).normalized().dot(ray_normal);
				diffuse = std::max(diffuse,0.);

				auto h = (camera_position + light_position).normalized();
				auto specular = ke * pow(ray_normal.dot(h), 1000);
				specular = std::max(specular, 0.);
				
				// Compute color
				C(i,j) = ambient + diffuse + specular;

				// Clamp to zero
				C(i,j) = std::max(C(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(red*C,green*C,blue*C,A,filename);
}

void raytrace_shading(){
	std::cout << "Simple ray tracer, one sphere with different shading" << std::endl;

	const std::string filename("shading.png");
	MatrixXd C = MatrixXd::Zero(800,800); // Store the color
	MatrixXd A = MatrixXd::Zero(800,800); // Store the alpha mask

	// The camera is perspective, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	Vector3d x_displacement(2.0/C.cols(),0,0);
	Vector3d y_displacement(0,-2.0/C.rows(),0);

	// Single light source
	const Vector3d light_position(-1,1,1);
	double ambient = 0.1;
	MatrixXd diffuse = MatrixXd::Zero(800, 800);
	MatrixXd specular = MatrixXd::Zero(800, 800);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray
			Vector3d ray_origin = origin + double(i)*x_displacement + double(j)*y_displacement;
			Vector3d ray_direction = RowVector3d(0,0,-1);

			// Intersect with the sphere
			// NOTE: this is a special case of a sphere centered in the origin and for orthographic rays aligned with the z axis
			Vector2d ray_on_xy(ray_origin(0),ray_origin(1));
			const double sphere_radius = 0.9;

			if (ray_on_xy.norm() < sphere_radius) {
				// The ray hit the sphere, compute the exact intersection point
				Vector3d ray_intersection(ray_on_xy(0),ray_on_xy(1),sqrt(sphere_radius*sphere_radius - ray_on_xy.squaredNorm()));

				// Compute normal at the intersection point
				Vector3d ray_normal = ray_intersection.normalized();

				// TODO: Add shading parameter here
				auto ambient = ka;

				auto diffuse = kd * (light_position-ray_intersection).normalized().dot(ray_normal);
				diffuse = std::max(diffuse,0.);

				auto h = (ray_origin + light_position).normalized();
				auto specular = ke * pow(ray_normal.dot(h), 1000);
				specular = std::max(specular, 0.);

				C(i,j) = ambient + diffuse + specular;

				// Clamp to zero
				C(i,j) = std::max(C(i,j), 0.0);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(red*C,green*C,blue*C,A,filename);
}

int main() {
	//raytrace_sphere();
	//raytrace_parallelogram();
	raytrace_perspective();
	//raytrace_shading();

	return 0;
}
