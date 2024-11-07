// C++ include
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <algorithm>

// Utilities for the Assignment
#include "raster.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"


// Definitions of Functions
void load_off(const std::string &filename, Eigen::MatrixXd &V, Eigen::MatrixXi &F);
void process_normals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixX3d &N);
void flat_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer);
void wireframe_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer);
void per_vertex_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer);


int main() 
{
	// The Framebuffer storing the image rendered by the rasterizer
	Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(500,500);

	// Global Constants (empty in this example)
	UniformAttributes uniform;

	// Basic rasterization program
	Program program;

	// The vertex shader is the identity
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		VertexAttributes out;
		out.position = uniform.view * (uniform.translate * uniform.rotate * uniform.scale) * va.position;
		out.normal = va.normal;
		return out;
	};

	// The fragment shader uses a fixed color
	program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		Eigen::Vector4f lights_color(0.0, 0.0, 0.0, 1);

		Eigen::Vector4f ambient = uniform.ambient.cwiseProduct(uniform.nature_color);

		for(unsigned i=0; i<uniform.light_positions.size(); ++i){
			Eigen::Vector4f Li = (uniform.light_positions[i] - va.position).normalized();
			Eigen::Vector4f N = va.normal.normalized();

			// Diffuse contribution
			Eigen::Vector4f diffuse = uniform.diffuse * std::max(Li.dot(N), 0.0f);

			// Specular contribution
			Eigen::Vector4f h_unit = (Li - (va.position - uniform.camera_position)).normalized();
			Eigen::Vector4f specular = uniform.specular * pow(std::max(h_unit.dot(va.normal), float(0.0)), 256.0);

			// Attenuate lights according to the squared distance to the lights
			Eigen::Vector4f D = uniform.light_positions[i] - va.position;
			lights_color += (diffuse + specular).cwiseProduct(uniform.light_intensity) /  D.squaredNorm();
		}

		Eigen::Vector4f C = ambient + lights_color;
		C[3] = std::min(C[3],float(1));
		FragmentAttributes out(C[0],C[1],C[2]);
		out.distance = (va.position - uniform.camera_position).norm();
		return out;
	};

	// The blending shader converts colors between 0 and 1 to uint8
	program.BlendingShader = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous)
	{
		if(fa.distance < previous.depth){
			FrameBufferAttributes out(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
			out.depth = fa.distance;
			return out;
		}
		else{
			return previous;
		}	
	};

	// Filename 
	std::string path = "./../data/";
	std::string name = "bunny";
	std::string filepath = path + name + ".off";

	// Loading .off file and setting up triangles
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	load_off(filepath, V, F);

	float aspect_ratio = float(frameBuffer.cols())/float(frameBuffer.rows());
	
	// Setting Uniforms
	uniform.view = Eigen::Matrix4f().setIdentity();
	if (aspect_ratio < 1)
		uniform.view(0,0) = aspect_ratio;
	else
		uniform.view(1,1) = 1/aspect_ratio;

	uniform.camera_position = Eigen::Vector4f(0.0, 0.0, 4.0, 1.0);
	uniform.light_intensity = Eigen::Vector4f(10, 10, 10, 1);
	uniform.light_positions.push_back(Eigen::Vector4f(8,8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(6,-8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(4,8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(2,-8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(0,8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(-2,-8,0,1));
	uniform.light_positions.push_back(Eigen::Vector4f(-4,8,0,1));
	uniform.nature_color = Eigen::Vector4f(0.8, 0.1, 0.8, 1);
	uniform.ambient = Eigen::Vector4f(0.4, 0.1, 0.4, 1);
	uniform.diffuse = Eigen::Vector4f(0.5, 0.5, 0.5, 1);
	uniform.specular = Eigen::Vector4f(0.2, 0.2, 0.2, 1);

	const float PI = 3.14159265359;
	float angle = 0;
	
	// Transformations
	uniform.rotate << cos(angle), 0.0f, sin(angle), 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f, 
	-sin(angle), 0.0f, cos(angle), 0.0f, 
	0.0f, 0.0f, 0.0f, 1.0f;

	uniform.scale << 10.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 10.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 10.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 1.0f;

	uniform.translate << 1.0f, 0.0f, 0.0f, 0.25f, 
	0.0f, 1.0f, 0.0f, -1.0f, 
	0.0f, 0.0f, 1.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 1.0f;

	// Flat Shading
	//flat_shading(V, F, program, uniform, frameBuffer);

	// Wireframe
	//wireframe_shading(V, F, program, uniform, frameBuffer);

	// Per Vertex Shading
	per_vertex_shading(V, F, program, uniform, frameBuffer);

	std::vector<uint8_t> image;
	framebuffer_to_uint8(frameBuffer,image);
	std::string file_output_name = name + ".png";
	stbi_write_png(file_output_name.c_str(), frameBuffer.rows(), frameBuffer.cols(), 4, image.data(), frameBuffer.rows()*4);
	
	return 0;
}

// Read a triangle mesh from an off file
void load_off(const std::string &filename, Eigen::MatrixXd &V, Eigen::MatrixXi &F) {
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

void process_normals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixX3d &N) {
	for(int i = 0; i < F.rows(); i++) {
		Eigen::Vector3d vert1 = V.row(F.row(i)[0]);
		Eigen::Vector3d vert2 = V.row(F.row(i)[1]);
		Eigen::Vector3d vert3 = V.row(F.row(i)[2]);

		//vertices
		VertexAttributes v1((float)vert1[0], (float)vert1[1], (float)vert1[2]);
		VertexAttributes v2((float)vert2[0], (float)vert2[1], (float)vert2[2]);
		VertexAttributes v3((float)vert3[0], (float)vert3[1], (float)vert3[2]);

		// Normal
		Eigen::Vector3d n = (vert2 - vert1).cross(vert3 - vert1);
		N.row(F.row(i)[0]) += n;
		N.row(F.row(i)[1]) += n;
		N.row(F.row(i)[2]) += n;
	}

	for(int i = 0; i < N.rows(); i++) {
		N.row(i) = N.row(i).normalized();
	}
}

// Flat shading
void flat_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer) {
	std::vector<VertexAttributes> vertices;
	for(int i = 0; i < F.rows(); i++) {
		Eigen::Vector3d vert1 = V.row(F.row(i)[0]);
		Eigen::Vector3d vert2 = V.row(F.row(i)[1]);
		Eigen::Vector3d vert3 = V.row(F.row(i)[2]);

		//vertices
		VertexAttributes v1((float)vert1[0], (float)vert1[1], (float)vert1[2]);
		VertexAttributes v2((float)vert2[0], (float)vert2[1], (float)vert2[2]);
		VertexAttributes v3((float)vert3[0], (float)vert3[1], (float)vert3[2]);

		// Normal
		Eigen::Vector3d n = (vert2 - vert1).cross(vert3 - vert1);
		v1.normal = Eigen::Vector4f(n[0], n[1], n[2], 0);
		v2.normal = Eigen::Vector4f(n[0], n[1], n[2], 0);
		v3.normal = Eigen::Vector4f(n[0], n[1], n[2], 0);

		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
	}
	// Rasterize Triangles - Flat Shading
	rasterize_triangles(program,uniform,vertices, frameBuffer);
}

void wireframe_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer) {
	std::vector<VertexAttributes> vertices;
	float offset = 0.0003; // prevent "z-fighting"
	for(int i = 0; i < F.rows(); i++) {
		vertices.clear();
		Eigen::Vector3d vert1 = V.row(F.row(i)[0]);
		Eigen::Vector3d vert2 = V.row(F.row(i)[1]);
		Eigen::Vector3d vert3 = V.row(F.row(i)[2]);

		//vertices
		VertexAttributes v1((float)vert1[0], (float)vert1[1], (float)vert1[2]+offset);
		VertexAttributes v2((float)vert2[0], (float)vert2[1], (float)vert2[2]+offset);
		VertexAttributes v3((float)vert3[0], (float)vert3[1], (float)vert3[2]+offset);

		vertices.push_back(v1);
		vertices.push_back(v2);

		vertices.push_back(v2);
		vertices.push_back(v3);

		vertices.push_back(v3);
		vertices.push_back(v1);

		rasterize_lines(program,uniform,vertices,0.5,frameBuffer);
	}
}

void per_vertex_shading(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, const Program &program, const UniformAttributes& uniform, Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> &frameBuffer) {
	std::vector<VertexAttributes> vertices;
	Eigen::Matrix<double, 502, 3> N;
	N.setZero();

	for(int i = 0; i < F.rows(); i++) {
		Eigen::Vector3d vert1 = V.row(F.row(i)[0]);
		Eigen::Vector3d vert2 = V.row(F.row(i)[1]);
		Eigen::Vector3d vert3 = V.row(F.row(i)[2]);

		//vertices
		VertexAttributes v1((float)vert1[0], (float)vert1[1], (float)vert1[2]);
		VertexAttributes v2((float)vert2[0], (float)vert2[1], (float)vert2[2]);
		VertexAttributes v3((float)vert3[0], (float)vert3[1], (float)vert3[2]);

		// Normal
		Eigen::Vector3d n = (vert2 - vert1).cross(vert3 - vert1);
		N.row(F(i,0)) += n;
		N.row(F(i,1)) += n;
		N.row(F(i,2)) += n;
	}

	for(int i = 0; i < N.rows(); i++) {
		N.row(i) = N.row(i).normalized();
	}

	for(int i = 0; i < F.rows(); i++) {
		Eigen::Vector3d vert1 = V.row(F.row(i)[0]);
		Eigen::Vector3d vert2 = V.row(F.row(i)[1]);
		Eigen::Vector3d vert3 = V.row(F.row(i)[2]);

		//vertices
		VertexAttributes v1((float)vert1[0], (float)vert1[1], (float)vert1[2]);
		VertexAttributes v2((float)vert2[0], (float)vert2[1], (float)vert2[2]);
		VertexAttributes v3((float)vert3[0], (float)vert3[1], (float)vert3[2]);

		// Normal
		v1.normal = Eigen::Vector4f(N(F(i,0),0), N(F(i,0),1), N(F(i,0),2), 0);
		v2.normal = Eigen::Vector4f(N(F(i,1),0), N(F(i,1),1), N(F(i,1),2), 0);
		v3.normal = Eigen::Vector4f(N(F(i,2),0), N(F(i,2),1), N(F(i,2),2), 0);

		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
	}
	// Rasterize Triangles - Flat Shading
	rasterize_triangles(program,uniform,vertices, frameBuffer);
}