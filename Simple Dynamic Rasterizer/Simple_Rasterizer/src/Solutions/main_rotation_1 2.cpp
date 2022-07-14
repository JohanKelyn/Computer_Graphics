// C++ include
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>


// Utilities for the Assignment
#include "raster.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"

#include <gif.h>

using namespace std;

// Shortcut to avoid Eigen:: everywhere, DO NOT USE IN .h
using namespace Eigen;

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



int main() 
{
	int width = 640;
	int height = 480;

	//Load the triangles meshes into a V matrix and F matrix
	MatrixXd V; 
	MatrixXi F;
	string filename = "../data/bunny.off";
	load_off(filename, V, F);

	// The Framebuffer storing the image rendered by the rasterizer
	Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(width,height);

	// Global Constants (empty in this example)
	UniformAttributes uniform;

	// Basic rasterization program2
	Program program;

	// The vertex shader is the identity
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		VertexAttributes out;
		
		out.position = va.position;
		
		Vector4f ambient_color = uniform.ambient_color.cwiseProduct(uniform.color);
		Vector4f lights_color(0.0, 0.0, 0.0, 1);
		for(unsigned i=0; i<uniform.light_position.size(); ++i){
			Vector4f Li = (uniform.light_position[i] - out.position).normalized();
			Vector4f diffuse = uniform.mat_diffuse * max(Li.dot(va.normal), float(0.0));

			Vector4f h_unit = (Li - (out.position - uniform.camera_position)).normalized();
			Vector4f specular = uniform.mat_specular * pow(max(h_unit.dot(out.normal), float(0.0)), 256.0);
			
			Vector4f D = uniform.light_position[i] - out.position;
			lights_color += (diffuse + specular).cwiseProduct(uniform.light_intensity) /  D.squaredNorm();
		}

		Vector4f C = ambient_color + lights_color;
		out.color = Vector4f(C[0],C[1],C[2],1);
		out.position = uniform.view*(uniform.M_rot * uniform.M_s) * out.position;
		out.normal = va.normal;
		return out;
	};

	// The fragment shader uses a fixed color
	program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		FragmentAttributes out(va.color[0],va.color[1],va.color[2]);
		out.position = va.position;
		out.distance = (out.position - uniform.camera_position).norm();
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


	// Add a transformation to compensate for the aspect ratio of the framebuffer
	float aspect_ratio = float(frameBuffer.cols())/float(frameBuffer.rows());

	uniform.view <<
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1;

	if (aspect_ratio < 1)
		uniform.view(0,0) = aspect_ratio;
	else
		uniform.view(1,1) = 1/aspect_ratio;

	vector<VertexAttributes> vertices;
	uniform.color = Vector4f(0.2, 0.2, 0.2, 1);
	uniform.ambient_color = Vector4f(0.5, 0.5, 0.5, 1);
	uniform.camera_position = Vector4f(0,0,4,1);
	uniform.light_position.push_back(Vector4f(8,8,0,1));
	uniform.light_position.push_back(Vector4f(6,-8,0,1));
	uniform.light_position.push_back(Vector4f(4,8,0,1));
	uniform.light_position.push_back(Vector4f(2,-8,0,1));
	uniform.light_position.push_back(Vector4f(0,8,0,1));
	uniform.light_position.push_back(Vector4f(-2,-8,0,1));
	uniform.light_position.push_back(Vector4f(-4,8,0,1));
	uniform.light_intensity = Vector4f(10, 10, 10, 1);;
	uniform.mat_diffuse = Vector4f(0.5, 0.5, 0.5, 1);
	uniform.mat_specular = Vector4f(0.2, 0.2, 0.2, 1);

	vector<VertexAttributes> lines;
	for(unsigned i=0; i < F.rows(); ++i){
		Vector3f vert1((V.row(F.row(i)[0])[0]), (V.row(F.row(i)[0])[1]), (V.row(F.row(i)[0])[2]));
		Vector3f vert2((V.row(F.row(i)[1])[0]), (V.row(F.row(i)[1])[1]), (V.row(F.row(i)[1])[2]));
		Vector3f vert3((V.row(F.row(i)[2])[0]), (V.row(F.row(i)[2])[1]), (V.row(F.row(i)[2])[2]));
		lines.push_back(VertexAttributes(vert1[0], vert1[1], vert1[2]));
		lines.push_back(VertexAttributes(vert2[0], vert2[1], vert2[2]));
		lines.push_back(VertexAttributes(vert2[0], vert2[1], vert2[2]));
		lines.push_back(VertexAttributes(vert3[0], vert3[1], vert3[2]));
		lines.push_back(VertexAttributes(vert3[0], vert3[1], vert3[2]));
		lines.push_back(VertexAttributes(vert1[0], vert1[1], vert1[2]));
	}

	const char * fileName = "triangle.gif";
	vector<uint8_t> image;
	int delay = 25;
	GifWriter g;
	GifBegin(&g, fileName, frameBuffer.rows(), frameBuffer.cols(), delay);

	for (float i=0;i<=1.1;i+=0.05)
	{
		Matrix<float,4,4> T;
		T << (1+i), 0, 0, 0,
			0, (1+i), 0, 0,
			0, 0, (1+i), 0,
			0, 0, 0, (1+1);

		Matrix<float,4,4> M_rot;
		M_rot << cos(i*2*M_PI), 0, sin(i*2*M_PI), 0,
			0, 1, 0, 0,
			-sin(i*2*M_PI), 0, cos(i*2*M_PI), 0,
			0, 0, 0, 1;
		
		uniform.M_s = T;
		uniform.M_rot = M_rot;
		
		frameBuffer.setConstant(FrameBufferAttributes());
		rasterize_lines(program,uniform,lines,0.8,frameBuffer);
		framebuffer_to_uint8(frameBuffer,image);
		GifWriteFrame(&g, image.data(), frameBuffer.rows(), frameBuffer.cols(), delay);
	}
	GifEnd(&g);
	
	return 0;
}
