#include "Mat.h"

Matrix::Matrix(int row, int col, bool rand_init)
{
	rowNum = row;
	colNum = col;
	size = row * col;
	//mat.reserve(size);
	mat.resize(size);
	srand((int)time(0));
	if (rand_init)
	{
		for (int i = 0; i < size; i++)
		{
			//mat.emplace_back((double)rand() / (double)RAND_MAX);
			mat[i] = (double)rand() / (double)RAND_MAX;
			mat[i] /= 1000.0;
		}
	}
	else
	{
		for (int i = 0; i<size; i++)
		{
			//mat.emplace_back(0.0);
			mat[i] = 0;
		}
	}
}
Matrix::Matrix(const Matrix &a)

{
	//cout << "copy called !!!!!!!!" << endl;
	rowNum = a.rowNum;
	colNum = a.colNum;
	size = a.getSize();
	mat = a.mat;
}
void Matrix::print()
{
	cout << "the matrix is: " << endl;
	for (int i = 0; i<rowNum; i++)
	{
		for (int j = 0; j<colNum; j++)
		{
			cout << mat[i*colNum + j] << " ";
		}
		cout << endl;
	}
}
void Matrix::Add(const Matrix& a, const Matrix&b)
{
	if (a.getRow() == b.getRow())
	{
		this->size = a.getSize();
		this->rowNum = a.getRow();
		this->colNum = a.getCol();
		//this->mat.reserve(this->size);
		this->mat.resize(this->size);

		if (a.getCol() == b.getCol())
		{
			for (int i = 0; i < this->size; i++)
			{
				//this->mat.emplace_back(a.mat[i]+b.mat[i]);
				this->mat[i] = a.mat[i] + b.mat[i];
			}
		}
		else if (b.getCol() == 1)
		{
			for (int i = 0; i < this->size; i++)
			{

				this->mat[i] = a.mat[i] + b.mat[i / this->colNum];

			}
		}
		else
		{
			cout << "Matrix dimensions do not match!" << endl;
		}

	}
	else
	{
		cout << "Matrix dimensions do not match!" << endl;
	}
}
void Matrix::Add(const Matrix& a, double b)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();

	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	for (int i = 0; i < this->size; i++)
	{
		//this->mat.emplace_back(a.mat[i] + b);
		this->mat[i] = a.mat[i] + b;
	}
}
void Matrix::Times(const Matrix& a, const Matrix& b)
{
	if (a.getRow() == b.getRow() && a.getCol() == b.getCol())
	{
		this->size = a.getSize();
		this->rowNum = a.getRow();
		this->colNum = a.getCol();
		//this->mat.reserve(this->size);
		this->mat.resize(this->size);
		for (int i = 0; i < size; i++)
		{
			//this->mat.emplace_back(a.mat[i] * b.mat[i]);
			this->mat[i] = a.mat[i] * b.mat[i];
		}
	}
	else
	{
		cout << "Matrix dimensions do not match!" << endl;
	}
}
void Matrix::Times(const Matrix& a, double b)
{
	//this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	this->size = this->rowNum*this->colNum;
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	for (int i = 0; i < size; i++)
	{
		//this->mat.emplace_back(a.mat[i] * b);
		this->mat[i] = a.mat[i] * b;
	}
}
void Matrix::Divide(const Matrix& a, const Matrix& b)
{
	if (a.getRow() == b.getRow() && a.getCol() == b.getCol())
	{
		this->size = a.getSize();
		this->rowNum = a.getRow();
		this->colNum = a.getCol();
		//this->mat.reserve(this->size);
		this->mat.resize(this->size);
		for (int i = 0; i < size; i++)
		{
			//this->mat.emplace_back(a.mat[i] / b.mat[i]);
			this->mat[i] = a.mat[i] / b.mat[i];
		}
	}
	else
	{
		cout << "Matrix dimensions do not match!" << endl;
	}
}
bool Matrix::operator==(const Matrix& a) const
{
	if (this->getRow() == a.getRow() && this->getCol() == a.getCol())
	{
		for (int i = 0; i < this->size; i++)
		{
			if (this->mat[i] != a.mat[i])
			{
				return false;
			}
		}
	}
	return true;
}
void Matrix::operator=(const Matrix& a)
{
	this->size = a.getSize();
	this->colNum = a.getCol();
	this->rowNum = a.getRow();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	for (int i = 0; i < this->size; i++)
	{
		//this->mat.emplace_back(a.mat[i]);
		this->mat[i] = a.mat[i];
	}
}

void Matrix::Multiply(const Matrix& a, const Matrix& b)
{
	int a_row = a.getRow();
	int a_col = a.getCol();
	int b_row = b.getRow();
	int b_col = b.getCol();

	if (a_col == b_row)
	{
		this->rowNum = a_row;
		this->colNum = b_col;
		this->size = a_row * b_col;
		this->mat.resize(this->size);

		double temp = 0.0;
		for (int i = 0; i<a_row; i++)
		{
			for (int j = 0; j<b_col; j++)
			{
				temp = 0;
				for (int k = 0; k<b_row; k++)
				{
					//tempv += this->getV(i, k)*other.getV(k, j);
					temp += a.getV(i, k)* b.getV(k, j);
				}
				this->setV(i, j, temp);
			}
		}
	}
	else
	{
		cout << "Matrix dimensions do not match!" << endl;
	}
}
void Matrix::Transpose(const Matrix& a)
{
	int row = a.getRow();
	int col = a.getCol();
	this->rowNum = col;
	this->colNum = row;
	this->size = row * col;
	this->mat.resize(this->size);

	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			this->setV(i, j, a.getV(j, i));
		}
	}
}

void Matrix::Translate(const Matrix& a, Vector3 b)
{
	if (a.rowNum != 4 || a.colNum != 4)
	{
		cout << "the input matrix is not 4x4 matrix!" << endl;
	}
	else
	{
		this->rowNum = a.rowNum;
		this->colNum = a.colNum;
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);
		this->mat = a.mat;

		this->mat[3] = a.mat[3] + b.x;
		this->mat[7] = a.mat[7] + b.y;
		this->mat[11] = a.mat[11] + b.z;
	}	
}

void Matrix::Rotate(const Matrix& a, double b, Vector3 c)
{
	
	if (a.rowNum != 4 || a.colNum != 4)
	{
		cout << "the input matrix is not 4x4 matrix!" << endl;
	}
	else
	{
		//important to convert euler angle to radians 
		b = 3.1415926*b / 180;
		this->rowNum = a.rowNum;
		this->colNum = a.colNum;
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);
		this->mat = a.mat;

		Matrix temp; // use to store the rotation matrix
		temp = a;
		//http://www.brainm.com/software/pubs/math/Rotation_matrix.pdf
		double cxcy = c.x*c.y;
		double cxcz = c.x*c.z;
		double cycz = c.y*c.z;
		double cosb = cos(b);
		double l_cosb = 1 - cosb;
		double sinb = sin(b);

		temp.mat[0] = cosb + l_cosb * c.x*c.x;
		temp.mat[1] = l_cosb * cxcy - sinb * c.z;
		temp.mat[2] = l_cosb * cxcz + sinb * c.y;
		temp.mat[3] = 0;
		temp.mat[4] = l_cosb * cxcy + sinb * c.z;
		temp.mat[5] = cosb + l_cosb * c.y*c.y;
		temp.mat[6] = l_cosb * cycz - sinb * c.x;
		temp.mat[7] = 0;
		temp.mat[8] = l_cosb * cxcz - sinb * c.y;
		temp.mat[9] = l_cosb * cycz + sinb * c.x;
		temp.mat[10] = cosb + l_cosb * c.z*c.z;
		temp.mat[11] = 0;
		temp.mat[12] = 0;
		temp.mat[13] = 0;
		temp.mat[14] = 0;
		temp.mat[15] = 1;

		this->Multiply(temp,a); // apply rotation to the current joint matrix
	}
}

void Matrix::Min(const Matrix& a, const Matrix& b)
{
	if (a.getRow() == b.getRow() && a.getCol() == b.getCol())
	{
		this->rowNum = a.getRow();
		this->colNum = a.getCol();
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);
		for (int row = 0; row < this->rowNum; row++)
		{
			for (int col = 0; col < this->colNum; col++)
			{
				if (a.getV(row, col) < b.getV(row, col)) this->setV(row, col, a.getV(row, col));
				else this->setV(row, col, b.getV(row, col));
			}
		}
	}
	else
	{
		cout << "matrix dimension does not match" << endl;
	}
}

void Matrix::Clip(const Matrix& a, const double& clipE)
{
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	this->size = this->rowNum*this->colNum;
	this->mat.resize(this->size);

	double lowerbound = 1 - clipE;
	double upperbound = 1 + clipE;
	double value = 0;
	for (int i = 0; i < this->rowNum; i++)
	{
		for (int j = 0; j < this->colNum; j++)
		{
			value = a.getV(i, j);
			if (value < lowerbound) this->setV(i, j, lowerbound);
			else if (value > upperbound) this->setV(i, j, upperbound);
			else this->setV(i, j, value);
			//this->setV(i, j, a.getV(i, j));
			//if (a.getV(i, j) < lowerbound) this->setV(i, j, lowerbound);
			//if (a.getV(i, j) > upperbound) this->setV(i, j, upperbound);
		}
	}
}

void Matrix::Times(const Matrix& a, const vector<double>& b)
{
	if (a.getCol() == b.size())
	{
		this->rowNum = a.getRow();
		this->colNum = a.getCol();
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);

		for (int j = 0; j < this->colNum; j++)
		{
			for (int i = 0; i < this->rowNum; i++)
			{
				this->setV(i, j, a.getV(i, j)*b[j]);
			}
		}
	}
	else
	{
		cout << "matrix and vector row dimension do not match" << endl;
	}
}
void Matrix::PushEndColumn(const Matrix& a, const Matrix& b)
{
	int row = a.rowNum;
	int col = a.colNum;

	if (col < 1)
	{
		this->rowNum = b.rowNum;
		this->colNum = b.colNum;
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);
		this->mat = b.mat;
	}
	else
	{
		if (row == b.rowNum)
		{
			this->rowNum = row;
			this->colNum = col + 1;
			this->size = this->rowNum*this->colNum;
			this->mat.resize(this->size);

			for (int i = 0; i < this->rowNum; i++)
			{
				for (int j = 0; j < this->colNum - 1; j++)
				{
					this->mat[i*this->colNum + j] = a.mat[i*col + j]; // assign all the columns first
				}
				this->mat[i*this->colNum + this->colNum - 1] = b.mat[i*b.colNum]; // then add the last columns with the vector b value
			}
		}
		else
		{
			cout << "The vector size does not match with the matrix row number!" << endl;
		}
	}
}

void Matrix::PopFrontColumn(const Matrix& a, int b)
{
	int col = a.colNum;

	if (b < col)
	{
		this->rowNum = a.rowNum;
		this->colNum = a.colNum - b;
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);

		for (int i = 0; i < this->rowNum; i++)
		{
			for (int j = 0; j < this->colNum; j++)
			{
				this->mat[i*this->colNum + j] = this->mat[i*col + j + b];
			}
		}
	}
	else
	{
		cout << "pop out columns bigger than the matrix column number" << endl;
	}
}
void Matrix::Softmax(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	this->mat.resize(this->size);

	double sum;
	for (int col = 0; col < this->colNum; col++)
	{
		sum = 0;
		for (int row = 0; row < this->rowNum; row++)
		{
			sum += exp(a.getV(row, col));
		}
		for (int row = 0; row < this->rowNum; row++)
		{
			this->setV(row, col, a.getV(row, col) / sum);
		}
	}
	

}
double Matrix::Exp(double x) const
{
	double sum = 1.0; // initialize sum of series
	int item = 10; //to add up the first 10 items
				   //e^x=1 + (x/1) (1 + (x/2) (1 + (x/3) (........) ) )
	for (int i = 10 - 1; i > 0; --i)
		sum = 1.0 + x * sum / (double)i;
	return sum;
}
void Matrix::Log(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	for (int i = 0; i < this->size; i++)
	{
		//this->mat.emplace_back(a.mat[i]);
		this->mat[i] = log(a.mat[i]);
	}
}
void Matrix::Relu(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	double temp;
	for (int i = 0; i < this->size; i++)
	{
		if (a.mat[i] < 0.0) temp = 0.0;
		else temp = a.mat[i];
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}

void Matrix::Sigmoid(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	double temp, temp1;
	for (int i = 0; i < this->size; i++)
	{
		temp1 = 1.0 + exp(a.mat[i] * (-1.0));
		temp = 1.0 / temp1;
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}
void Matrix::Tanh(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	double temp, temp1;
	for (int i = 0; i < this->size; i++)
	{
		temp1 = 1.0 + exp(2.0*a.mat[i] * (-1.0));
		temp = 2.0 / temp1 - 1.0;
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}
void Matrix::DRelu(const Matrix& a)
{
	this->size = a.getSize();
	this->rowNum = a.getRow();
	this->colNum = a.getCol();
	//this->mat.reserve(this->size);
	this->mat.resize(this->size);
	double temp;
	for (int i = 0; i < this->size; i++)
	{
		if (a.mat[i] < 0.0) temp = 0.0;
		else temp = 1.0;
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}
void Matrix::DSigmoid(const Matrix& a)
{
	this->Sigmoid(a);
	double temp;
	for (int i = 0; i < this->size; i++)
	{
		temp = this->mat[i] * (1.0 - this->mat[i]);
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}
void Matrix::DTanh(const Matrix& a)
{
	this->Tanh(a);
	double temp;
	for (int i = 0; i < this->size; i++)
	{
		temp = 1.0 - this->mat[i] * this->mat[i];
		//this->mat.emplace_back(temp);
		this->mat[i] = temp;
	}
}
void Matrix::Reassign(const Matrix& a)
{
	/*
	int row = a.getRow();
	int col = a.getCol();

	this->rowNum = row;
	this->colNum = col;
	this->size = row * col;
	this->mat.resize(this->size);

	double temp;
	for (int i = 0; i < row; i++)
	{
	temp = 0;
	for (int j = 0; j < col; j++)
	{
	temp += a.mat[i*col + j];
	}
	temp /= (double)col;

	for (int j = 0; j < col; j++)
	{
	this->mat[i*col + j] = temp;
	}
	}
	*/
	int row = a.getRow();
	int col = a.getCol();

	this->rowNum = row;
	this->colNum = 1;
	this->size = row * 1;
	this->mat.resize(this->size);

	double temp;
	for (int i = 0; i < row; i++)
	{
		temp = 0;
		for (int j = 0; j < col; j++)
		{
			temp += a.mat[i*col + j];
		}
		temp /= (double)col;

		this->mat[i] = temp;
	}
}
void Matrix::SplitCol(const Matrix& a, int start, int end)
{
	int row = a.getRow();
	int col = a.getCol();

	if (start >= 0 && end >= 0 && start <= end && end < col)
	{
		this->rowNum = row;
		this->colNum = end - start + 1;
		this->size = this->rowNum*this->colNum;
		this->mat.resize(this->size);

		for (int i = 0; i < this->rowNum; i++)
		{
			for (int j = 0; j < this->colNum; j++)
			{
				this->mat[i*this->colNum + j] = a.mat[i*col + j + start];
			}
		}
	}
	else
	{
		cout << "Split range is wrong!!" << endl;
	}
}
Matrix::~Matrix()
{
	//delete mat;
	//cout << "called" << endl;
	//delete temp;
}
