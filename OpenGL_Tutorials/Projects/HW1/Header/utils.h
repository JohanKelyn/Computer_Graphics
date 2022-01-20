#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include <fstream>

using namespace Eigen;

void load_off(const std::string &filename, MatrixXf &V, MatrixXi &F)
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

void convert_to_array(MatrixXf &V, MatrixXi &F, float *data, unsigned int *indices)
{
	int counter = 0;

    for(int i=0; i < V.rows(); ++i)
    {
        for(int j=0; j < V.cols(); ++j)
        {
            data[counter] = V(i,j);
            counter++;
        }
    }

	counter = 0;
	for(int i=0; i < F.rows(); ++i)
    {
        for(int j=0; j < F.cols(); ++j)
        {
            indices[counter] = (unsigned int) F(i,j);
            counter++;
        }
    }
}

void get_per_face_normals(MatrixXf &V, MatrixXi &F, MatrixXf &N)
{
	for(unsigned i=0; i < F.rows(); ++i){
		Vector3f vert1((V.row(F.row(i)[0])[0]), (V.row(F.row(i)[0])[1]), (V.row(F.row(i)[0])[2]));
		Vector3f vert2((V.row(F.row(i)[1])[0]), (V.row(F.row(i)[1])[1]), (V.row(F.row(i)[1])[2]));
		Vector3f vert3((V.row(F.row(i)[2])[0]), (V.row(F.row(i)[2])[1]), (V.row(F.row(i)[2])[2]));
		Vector3f v1 = (vert2 - vert1).normalized();
		Vector3f v2 = (vert3 - vert2).normalized();
		Vector3f n1((v1[1]*v2[2])-(v2[1]*v1[2]), -(v1[0]*v2[2])-(v1[2]*v2[0]), (v1[0]*v2[1])-(v1[2]*v2[0]));
		
		N.row(F.row(i)[0])[0] += n1[0];
		N.row(F.row(i)[0])[1] += n1[1];
		N.row(F.row(i)[0])[2] += n1[2];
		N.row(F.row(i)[1])[0] += n1[0];
		N.row(F.row(i)[1])[1] += n1[1];
		N.row(F.row(i)[1])[2] += n1[2];
		N.row(F.row(i)[2])[0] += n1[0];
		N.row(F.row(i)[2])[1] += n1[1];
		N.row(F.row(i)[2])[2] += n1[2];
	}
}

#endif