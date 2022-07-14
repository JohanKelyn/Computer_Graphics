#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include<Eigen/Core>
#include<Eigen/Dense>
#include <fstream>

class Object
{
    public:
        Object(const char* filepath);
        void getIndices(unsigned int *indices);
        void getDataArray(float *data);
        void get_per_face_normals();
        void load_off(const std::string &filename);
        Eigen::MatrixXf V;
        Eigen::MatrixXf N;
        Eigen::MatrixXi F;
        int number_vertices;
        int number_indices;
};

#endif