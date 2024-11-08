#pragma once

#include <Eigen/Core>

class VertexAttributes
{
	public:
	VertexAttributes(float x = 0, float y = 0, float z = 0, float w = 1, float nx = 0, float ny = 0, float nz = 0, float nw = 1)
	{
		position << x,y,z,w;
		normal << nx,ny,nz,nw;
	}

    // Interpolates the vertex attributes
    static VertexAttributes interpolate(
        const VertexAttributes& a,
        const VertexAttributes& b,
        const VertexAttributes& c,
        const float alpha, 
        const float beta, 
        const float gamma
    ) 
    {
        VertexAttributes r;
        r.position = alpha*a.position + beta*b.position + gamma*c.position;
		r.normal = alpha*a.normal + beta*b.normal + gamma*c.normal;
        return r;
    }
	Eigen::Vector4f position;
	Eigen::Vector4f normal;
};

class FragmentAttributes
{
	public:
	FragmentAttributes(float r = 0, float g = 0, float b = 0, float a = 1)
	{
		color << r,g,b,a;
	}

	Eigen::Vector4f color;
	float distance;
};

class FrameBufferAttributes
{
	public:
	FrameBufferAttributes(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
	{
		color << r,g,b,a;
		depth = INFINITY;
	}

	Eigen::Matrix<uint8_t,4,1> color;
	float depth;
};

class UniformAttributes
{
	public:
	Eigen::Matrix4f view;
	Eigen::Vector4f camera_position;
	std::vector<Eigen::Vector4f> light_positions;
	Eigen::Vector4f nature_color;
	Eigen::Vector4f light_intensity;
	Eigen::Vector4f ambient;
	Eigen::Vector4f diffuse;
	Eigen::Vector4f specular;

	Eigen::Matrix4f translate;
	Eigen::Matrix4f rotate;
	Eigen::Matrix4f scale;
};