#pragma once
#pragma once
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include "Struct.h"

using namespace std;
class Matrix
{
private:
	int rowNum;
	int colNum;
	int size;
	vector<double> mat;
public:
	Matrix() { rowNum = 0; colNum = 0; size = 0; }
	Matrix(int row, int col, bool rand_init);
	Matrix(const Matrix &a); //copy construnctor

	vector<double> getMat() const { return mat; }
	int getRow() const { return rowNum; }
	int getCol() const { return colNum; }
	int getSize() const { return size; }

	void setV(int row, int col, double v) { mat[row*colNum + col] = v; }
	double getV(int row, int col) const { return mat[row*colNum + col]; }
	void print();

	//operator overloading
	//element-wise add to matrix a+b
	void Add(const Matrix& a, const Matrix&b);
	//add a number to matrix a+b
	void Add(const Matrix& a, double b);
	//element-wise dot product of matrix a*b
	void Times(const Matrix& a, const Matrix& b);
	// time a number to matrix a*b
	void Times(const Matrix& a, double b);
	//element-wise matrix division a/b
	void Divide(const Matrix& a, const Matrix& b);
	bool operator==(const Matrix& a) const;
	void operator=(const Matrix& a);
	//matrix production, axb
	void Multiply(const Matrix& a, const Matrix& b);
	void Transpose(const Matrix& a);

	//write the rotation matrix and translation for BVH file joint position and angle change
	void Translate(const Matrix& a, Vector3 b);
	void Rotate(const Matrix& a, double b, Vector3 c);
	// for ppo policy gradient
	void Min(const Matrix& a, const Matrix& b);
	void Clip(const Matrix& a, const double& clipE);
	void Times(const Matrix& a, const vector<double>& b);

	// so here to push more columns into the existed matrix
	// for the deep reinforcement learning buffer purpose
	void PushEndColumn(const Matrix& a, const Matrix& b);
	// here to pop up the first b columns out of the matrix
	void PopFrontColumn(const Matrix& a, int b);

	void Softmax(const Matrix& a);

	double Exp(double x) const;
	void Log(const Matrix& a);
	//relu, sigmoid, tanh, 3 activation functions for neuron network layer parameters calculation
	void Relu(const Matrix& a);
	void Sigmoid(const Matrix& a);
	void Tanh(const Matrix& a);
	void DRelu(const Matrix& a);
	void DSigmoid(const Matrix& a);
	void DTanh(const Matrix& a);

	// for bias b to average the cololumns and then averaged to the same value, then expand the same value back to all cololums
	//example, b=[2 3 5 6 8]==> b=[4.8 4.8 4.8 4.8 4.8]
	void Reassign(const Matrix& a);

	void SplitCol(const Matrix& a, int start, int end);  //[start, end]

	~Matrix();
};


