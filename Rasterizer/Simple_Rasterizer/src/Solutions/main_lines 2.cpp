// C++ include
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


// Utilities for the Assignment
#include "raster.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"

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

	// The Framebuffer storing the image rendered by the rasterizer
	Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(width,height);

	// Global Constants (empty in this example)
	UniformAttributes uniform;

	// Basic rasterization program
	Program program;

	// The vertex shader is the identity
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		return va;
	};

	// The fragment shader uses a fixed color
	program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		FragmentAttributes out(uniform.color[0],uniform.color[1],uniform.color[2]);
		out.position = va.position;
		out.distance = (out.position - uniform.camera_position).norm();
		return out;
	};

	// The blending shader converts colors between 0 and 1 to uint8
	program.BlendingShader = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous)
	{
		if(fa.position[2] < previous.depth){
			FrameBufferAttributes out(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
			out.depth = fa.position[2];
			return out;
		}
		else{
			//return previous;
			return previous;
		}
	};

	//Load the triangles meshes into a V matrix and F matrix
	MatrixXd V; 
	MatrixXi F;
	string filename = "../data/bunny.off";
	load_off(filename, V, F);

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

	uniform.color = Vector4f(0.8, 0.1, 0.8, 1);
	uniform.camera_position = Vector4f(0,0,4,1);
	uniform.light_position.push_back(Vector4f(8,8,0,1));
	uniform.light_position.push_back(Vector4f(6,-8,0,1));
	uniform.light_position.push_back(Vector4f(4,8,0,1));
	uniform.light_position.push_back(Vector4f(2,-8,0,1));
	uniform.light_position.push_back(Vector4f(0,8,0,1));
	uniform.light_position.push_back(Vector4f(-2,-8,0,1));
	uniform.light_position.push_back(Vector4f(-4,8,0,1));
	uniform.light_intensity = Vector4f(10, 10, 10, 1);;

	vector<VertexAttributes> vertices;
	for(unsigned i=0; i < F.rows(); ++i){
		double x1 = (V.row(F.row(i)[0])[0]);
		double y1 = (V.row(F.row(i)[0])[1]);
		double z1 = (V.row(F.row(i)[0])[2]);
		double x2 = (V.row(F.row(i)[1])[0]);
		double y2 = (V.row(F.row(i)[1])[1]);
		double z2 = (V.row(F.row(i)[1])[2]);
		double x3 = (V.row(F.row(i)[2])[0]);
		double y3 = (V.row(F.row(i)[2])[1]);
		double z3 = (V.row(F.row(i)[2])[2]);
		vertices.push_back(VertexAttributes(x1, y1, z1));
		vertices.push_back(VertexAttributes(x2, y2, z2));
		vertices.push_back(VertexAttributes(x2, y2, z2));
		vertices.push_back(VertexAttributes(x3, y3, z3));
		vertices.push_back(VertexAttributes(x3, y3, z3));
		vertices.push_back(VertexAttributes(x1, y1, z1));
	}

	rasterize_lines(program,uniform,vertices,0.5,frameBuffer);
	
	vector<uint8_t> image;
	framebuffer_to_uint8(frameBuffer,image);
	stbi_write_png("triangle.png", frameBuffer.rows(), frameBuffer.cols(), 4, image.data(), frameBuffer.rows()*4);
	
	return 0;
}
