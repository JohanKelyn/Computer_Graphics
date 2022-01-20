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
		out.position = uniform.M_cam*va.position;

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
		out.position = uniform.M_proj*out.position;
		//out.position = out.position / out.position[3];
		out.position = uniform.view*out.position;
		
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


	vector<VertexAttributes> vertices;
	uniform.color = Vector4f(0.8, 0.1, 0.8, 1);
	uniform.ambient_color = Vector4f(0.4, 0.1, 0.4, 1);
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
	uniform.field_of_view = 0.7854;
	uniform.focal_length = 3;
	uniform.field_of_view = true;

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

	// Creating the camera transformation matrix
	Vector4f g(0,0,-1,0);
	Vector4f t(0,1,0,0);
	Vector4f w = -g.normalized();
	Vector4f u((t[1]*w[2])-(w[1]*t[2]), -(t[0]*w[2])-(t[2]*w[0]), (t[0]*w[1])-(t[2]*w[0]),0);
	u = u.normalized();
	Vector4f v((w[1]*u[2])-(u[1]*w[2]), -(w[0]*u[2])-(w[2]*u[0]), (w[0]*u[1])-(w[2]*u[0]),0);

	Matrix4f M;
	M << u[0], v[0], w[0], uniform.camera_position[0],
		u[1], v[1], w[1], uniform.camera_position[1],
		u[2], v[2], w[2], uniform.camera_position[2],
		0, 0, 0, 1;

	Matrix4f M_cam = M.inverse();
	uniform.M_cam = M_cam;
	
	// Creating the Orthographic transformation matrix
	float n = -1.5;
	float f = -6;
	float top = tan(uniform.field_of_view/2) * n;
	float r = aspect_ratio * abs(top);
	float l = -r;
	float b = -top;

	Matrix4f M_ort;
	M_ort << 2/(r-l), 0, 0, -(r + l)/(r - l),
			0, 2/(top - b), 0, -(top + b)/(top - b),
			0, 0, 2/(n-f), -(n+f)/(n-f),
			0, 0, 0, 1;

	Matrix4f M_per;
	M_per << n, 0, 0, 0,
			0, n, 0, 0,
			0, 0, n+f, -f*n,
			0, 0, 1, 0;

	if(uniform.is_perspective){
		uniform.M_proj = M_ort * M_per;
	}
	else{
		uniform.M_proj = M_ort;
	}

	MatrixXf normals = MatrixXf::Zero(V.rows(),3);
	for(unsigned i=0; i < F.rows(); ++i){
		Vector3f vert1((V.row(F.row(i)[0])[0]), (V.row(F.row(i)[0])[1]), (V.row(F.row(i)[0])[2]));
		Vector3f vert2((V.row(F.row(i)[1])[0]), (V.row(F.row(i)[1])[1]), (V.row(F.row(i)[1])[2]));
		Vector3f vert3((V.row(F.row(i)[2])[0]), (V.row(F.row(i)[2])[1]), (V.row(F.row(i)[2])[2]));
		Vector3f v1 = vert2 - vert1;
		Vector3f v2 = vert3 - vert2;
		Vector3f n1((v1[1]*v2[2])-(v2[1]*v1[2]), -(v1[0]*v2[2])-(v1[2]*v2[0]), (v1[0]*v2[1])-(v1[2]*v2[0]));
		
		normals.row(F.row(i)[0])[0] += n1[0];
		normals.row(F.row(i)[0])[1] += n1[1];
		normals.row(F.row(i)[0])[2] += n1[2];
		normals.row(F.row(i)[1])[0] += n1[0];
		normals.row(F.row(i)[1])[1] += n1[1];
		normals.row(F.row(i)[1])[2] += n1[2];
		normals.row(F.row(i)[2])[0] += n1[0];
		normals.row(F.row(i)[2])[1] += n1[1];
		normals.row(F.row(i)[2])[2] += n1[2];
	}

	for(unsigned i=0; i<normals.rows(); ++i){
		normals.row(i) = normals.row(i).normalized();
	}

	for(unsigned i=0; i < F.rows(); ++i){
		Vector3f vert1((V.row(F.row(i)[0])[0]), (V.row(F.row(i)[0])[1]), (V.row(F.row(i)[0])[2]));
		Vector3f vert2((V.row(F.row(i)[1])[0]), (V.row(F.row(i)[1])[1]), (V.row(F.row(i)[1])[2]));
		Vector3f vert3((V.row(F.row(i)[2])[0]), (V.row(F.row(i)[2])[1]), (V.row(F.row(i)[2])[2]));

		Vector3f normal1 = normals.row(F.row(i)[0]);
		Vector3f normal2 = normals.row(F.row(i)[1]);
		Vector3f normal3 = normals.row(F.row(i)[2]);
		
		vertices.push_back(VertexAttributes(vert1[0], vert1[1], vert1[2], normal1[0], normal1[1], normal1[2]));
		vertices.push_back(VertexAttributes(vert2[0], vert2[1], vert2[2], normal2[0], normal2[1], normal2[2]));
		vertices.push_back(VertexAttributes(vert3[0], vert3[1], vert3[2], normal3[0], normal3[1], normal3[2]));
	}

	rasterize_triangles(program,uniform,vertices,frameBuffer);
	
	vector<uint8_t> image;
	framebuffer_to_uint8(frameBuffer,image);
	stbi_write_png("triangle.png", frameBuffer.rows(), frameBuffer.cols(), 4, image.data(), frameBuffer.rows()*4);
	
	return 0;
}
