#include "Object.h"
#include <iostream>


Object::Object(const char* filepath)
{

    // Reading Model
    load_off(filepath);

    // Get Normals for Data
    N = Eigen::MatrixXf::Zero(V.rows(),3);
    get_per_face_normals();

    // Number of vertices and indices
    number_vertices = V.rows() * V.cols();
    number_indices = F.rows() * F.cols();
}

using namespace Eigen;

void Object::load_off(const std::string &filename)
{
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


void Object::get_per_face_normals()
{
	VectorXf verts = VectorXf::Zero(V.rows());
	for(unsigned i=0; i < F.rows(); ++i){
		Vector3f vert1((V.row(F.row(i)[0])[0]), (V.row(F.row(i)[0])[1]), (V.row(F.row(i)[0])[2]));
		Vector3f vert2((V.row(F.row(i)[1])[0]), (V.row(F.row(i)[1])[1]), (V.row(F.row(i)[1])[2]));
		Vector3f vert3((V.row(F.row(i)[2])[0]), (V.row(F.row(i)[2])[1]), (V.row(F.row(i)[2])[2]));
		Vector3f v1 = (vert2 - vert1).normalized();
		Vector3f v2 = (vert3 - vert2).normalized();
		Vector3f n1((v1[1]*v2[2]-v2[1]*v1[2]), -(v1[0]*v2[2]-v1[2]*v2[0]), (v1[0]*v2[1]-v1[1]*v2[0]));
		
		N.row(F.row(i)[0]) += n1;
		N.row(F.row(i)[1]) += n1;
		N.row(F.row(i)[2]) += n1;

		verts[F.row(i)[0]] += 1;
		verts[F.row(i)[1]] += 1;
		verts[F.row(i)[2]] += 1;
	}
	for(unsigned i=0; i<N.rows(); ++i){
		N.row(i) = (N.row(i)/verts[i]).normalized();
	}
}

void Object::getDataArray(float *data)
{
	for(int i=0; i<V.rows(); ++i)
    {
        data[6*i] = V.row(i)[0];
        data[6*i+1] = V.row(i)[1];
        data[6*i+2] = V.row(i)[2];
        data[6*i+3] = N.row(i)[0];
        data[6*i+4] = N.row(i)[1];
        data[6*i+5] = N.row(i)[2];
    }
}

void Object::getIndices(unsigned int *indices)
{
	for(int i=0; i<F.rows(); ++i)
    {
        indices[3*i] = F.row(i)[0];
        indices[3*i+1] = F.row(i)[1];
        indices[3*i+2] = F.row(i)[2];
    }
}