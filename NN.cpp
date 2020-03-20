#include "NN.h"
#include "Mat.h"
#include <fstream>
#include <string>
//implementation for Layer class
Layer::Layer(const Matrix& w, const Matrix& b, int actfun)
{
	//weight = new Matrix();
	//bias = new Matrix();
	weight = w;
	bias = b;
	activateFun = actfun;
	// to initialize
	//linearValue = new Matrix();
	//activatedValue = new Matrix();

	//d_weight = new Matrix();
	//d_bias = new Matrix();
	//d_linearValue = new Matrix();
	//d_activatedValue = new Matrix();
}
void Layer::calcValues(const Matrix& batchinput)
{
	batchNum = batchinput.getCol();
	Matrix result_mult;// = new Matrix();
	result_mult.Multiply(weight, batchinput);
	linearValue.Add(result_mult, bias);
	if (activateFun == none) { activatedValue = linearValue; }
	//f(x)=max(0,x)
	if (activateFun == relu) { activatedValue.Relu(linearValue); }
	//f(x)=1/(1+e-x)
	if (activateFun == sigmoid) { activatedValue.Sigmoid(linearValue); }
	//f(x) = tanh(x) = 2/(1 + e-2x) - 1 OR tanh(x) = 2 * sigmoid(2x) - 1
	if (activateFun == tanh) { activatedValue.Tanh(linearValue); }

	//delete result_mult;
}
Layer::Layer(const Layer& copy)
{
	this->activateFun = copy.activateFun;
	this->batchNum = copy.batchNum;
	this->weight = copy.weight;
	this->bias = copy.bias;
	//this->activatedValue = copy.activatedValue;
	//this->weight = &Matrix(*copy.weight);
	//this->bias = &Matrix(*copy.bias);
}
//implementation for NeuronNetwork class
NeuronNetwork::NeuronNetwork()
{

}
NeuronNetwork::NeuronNetwork(int& x_dim, int& layernum, vector<int>& neuronnums, vector<int>& actfunnums, double& learningrate, int& batchsize)
{
	neuronNums = neuronnums;
	actfunNums = actfunnums;
	layerNum = layernum;
	this->x_dim = x_dim; // to put the input matrix into the variable
	learningRate = learningrate;
	batchSize = batchsize;

	cost.reserve(1000); // how many iterates
	Layers.resize(layernum);
	Matrix m;  // use 12 matrix for holding temporary calculation result
	for (int i = 0; i < 12; i++)
	{
		m = Matrix();
		temp.push_back(m);
	}
	//add all the layers, all the weight and bias will be initialized
	// start from the first hidden layer
	int t_v;
	for (int i = 0; i <layerNum; i++)
	{
		if (i == 0) t_v = x_dim;
		else t_v = neuronnums[i - 1];

		Layer layer = AddLayer(neuronnums[i], t_v, actfunnums[i]);
		Layers[i] = layer;
	}
}

void NeuronNetwork::copyNetworkWeights(const NeuronNetwork& other)
{
	/*this->neuronNums = other.neuronNums;
	this->actfunNums = other.actfunNums;
	this->layerNum = other.layerNum;
	this->learningRate = other.learningRate;
	this->batchSize = other.batchSize;
	this->x_dim = other.x_dim;
	this->cost = other.cost;
	this->temp = other.temp;
	this->Layers.resize(this->layerNum);*/
	for (int i = 0; i < this->layerNum; i++)
	{
		this->Layers[i] = Layer(other.Layers[i]);
	}
}
//NeuronNetwork::NeuronNetwork(const NeuronNetwork& copy)
//{
//	this->neuronNums = copy.neuronNums;
//	this->actfunNums = copy.actfunNums;
//	this->layerNum = copy.layerNum;
//	this->learningRate = copy.learningRate;
//	this->batchSize = copy.batchSize;
//	this->x_dim = copy.x_dim;
//	this->cost = copy.cost;
//	this->temp = copy.temp;
//
//	for (int i = 0; i < this->layerNum; i++)
//	{
//		//this->Layers[i]->setWeight(*copy.Layers[i]->getWeight());
//		//this->Layers[i]->setBias(*copy.Layers[i]->getBias());
//
//		//this->Layers[i] = Layer(copy.Layers[i]);
//	}
//}
//void NeuronNetwork::operator=(const NeuronNetwork& copy)
//{
//	this->neuronNums = copy.neuronNums;
//	this->actfunNums = copy.actfunNums;
//	this->layerNum = copy.layerNum;
//	this->learningRate = copy.learningRate;
//	this->batchSize = copy.batchSize;
//	this->x_dim = copy.x_dim;
//	this->cost = copy.cost;
//	this->temp = copy.temp;
//	this->Layers.resize(this->layerNum);
//	for (int i = 0; i < this->layerNum; i++)
//	{
//		//this->Layers[i]->setWeight(*copy.Layers[i]->getWeight());
//		//this->Layers[i]->setBias(*copy.Layers[i]->getBias());
//
//		//this->Layers[i] = Layer(copy.Layers[i]);
//		//this->Layers[i]
//		this->Layers[i] = Layer(copy.Layers[i]);
//		//this->Layers[i] = copy.Layers[i];
//		//this->Layers[i]->setBias(copy.Layers[i]->getBias());
//	}
//	return *this;
//	cout << "neuronnetwork copy operator called!" << endl;
//}
const Matrix& NeuronNetwork::getOutput()
{
	return Layers[layerNum - 1].getActivatedValue();
}
Layer NeuronNetwork::AddLayer(int neuron, int neuronNext, int actFun)
{
	//Matrix* weight = new Matrix(neuron, neuronNext + 1, true); // new Matrix(neuron, neuronNext, true)
	Matrix weight = Matrix(neuron, neuronNext, true);
	Matrix bias = Matrix(neuron, 1, true);	 //bias only has one colounmn
	Layer layer =  Layer(weight, bias, actFun);
	return layer;
}
void NeuronNetwork::FeedForward(const Matrix& x_train)
{
	//Matrix* prelayer_y; prelayer_y=temp[0];
	for (int i = 0; i < layerNum; i++)
	{
		if (i == 0) temp[0] = x_train;
		else temp[0] = Layers[i - 1].getActivatedValue();

		Layers[i].calcValues(temp[0]);
	}
}
double NeuronNetwork::CrossEntropyCost(const Matrix& y_train, const Matrix& y_activate)
{
	// cost = -1/m*sigma[log(a).* y+ log(1-a).*(1-y)]
	temp[1].Log(y_activate);  //log(a) //not useful
	temp[2].Times(temp[1], y_train); //log(a).* y

	temp[3].Times(y_activate, -1.0); //-a; // not useful
	temp[4].Add(temp[3], 1.0);  //1-a // not useful
	temp[5].Log(temp[4]); // log(1-a)
	temp[6].Times(y_train, -1.0); // -y // not useful
	temp[7].Add(temp[6], 1.0); // 1-y // not useful
	temp[8].Times(temp[5], temp[7]); //log(1-a).*(1-y)
	temp[9].Add(temp[2], temp[8]); //log(a).* y+ log(1-a).*(1-y)

	temp[10].Reassign(temp[9]);
	temp[11].Times(temp[10], -1.0); // add a negative simple in front
	double sum = 0;
	int row = temp[11].getRow();
	for (int i = 0; i < row; i++)
	{
		sum += temp[11].getV(i, 0);
	}
	sum /= (double)row;

	return sum;
}

double NeuronNetwork::CrossEntropyCost_multiclass(const Matrix& y_train, const Matrix& y_activate)
{
	// cost = -1/m*sigma[(a-y)*(a-y)]
	temp[1].Times(y_train, -1.0); // -y // not useful
	temp[2].Add(y_activate, temp[1]);
	temp[3].Times(temp[2], temp[2]);


	//temp[1].Log(y_activate);  //log(a) //not useful
	//temp[2].Times(temp[1], y_train); //log(a).* y

	//temp[3].Times(y_activate, -1.0); //-a; // not useful
	//temp[4].Add(temp[3], 1.0);  //1-a // not useful
	//temp[5].Log(temp[4]); // log(1-a)
	//temp[6].Times(y_train, -1.0); // -y // not useful
	//temp[7].Add(temp[6], 1.0); // 1-y // not useful
	//temp[8].Times(temp[5], temp[7]); //log(1-a).*(1-y)
	//temp[9].Add(temp[2], temp[8]); //log(a).* y+ log(1-a).*(1-y)

	temp[10].Reassign(temp[3]);
	temp[11].Times(temp[10], -1.0/1000000); // add a negative simple in front
	double sum = 0;
	int row = temp[11].getRow();
	for (int i = 0; i < row; i++)
	{
		sum += temp[11].getV(i, 0);
	}
	sum /= (double)row;

	return sum;
}
double NeuronNetwork::CrossEntropyCost_ppoclip(const Matrix& y_train, const Matrix& y_activate, const Matrix& w_ratio, const vector<double>& adv_GAE)
{
	// cost = -1/m*sigma[clip(1-e,1+e).*adv^2]
	temp[1].Clip(w_ratio, 0.2);  // clip(r, 1-e,1+e)
	temp[2].Times(temp[1], adv_GAE); //clip(r,1-e,1+e)*adv
	temp[3].Times(temp[2], adv_GAE);

	//temp[1].Log(y_activate);  //log(a) //not useful
	//temp[2].Times(temp[1], y_train); //log(a).* y

	//temp[3].Times(y_activate, -1.0); //-a; // not useful
	//temp[4].Add(temp[3], 1.0);  //1-a // not useful
	//temp[5].Log(temp[4]); // log(1-a)
	//temp[6].Times(y_train, -1.0); // -y // not useful
	//temp[7].Add(temp[6], 1.0); // 1-y // not useful
	//temp[8].Times(temp[5], temp[7]); //log(1-a).*(1-y)
	//temp[9].Add(temp[2], temp[8]); //log(a).* y+ log(1-a).*(1-y)

	temp[10].Reassign(temp[3]);
	temp[11].Times(temp[10], -1.0); // add a negative simple in front
	double sum = 0;
	int row = temp[11].getRow();
	for (int i = 0; i < row; i++)
	{
		sum += temp[11].getV(i, 0);
	}
	sum /= (double)row;

	return sum;
}
void NeuronNetwork::PrintCost()
{
	//int row = cost->getRow();
	//int col = cost->getCol();
	//double sum = 0;
 //	for (int i = 0; i < cost.size(); i++)
	//{
	//	//cout << cost[i] << endl;	
	//}
	//sum /=(double) row;
	//cout << sum<< endl; //print out the first coloumn of all rows
}
void NeuronNetwork::BackPropagation(const Matrix& x_train, const Matrix& y_train)
{
	for (int i = layerNum - 1; i >= 0; i--)
	{
		if (i == layerNum - 1) //the last layer, also the output layer is different from other hidden layers
		{
			temp[1].Times(y_train, -1.0); // -y			
			temp[2].Add(temp[1], 1.0); //1-y
			temp[3].Times(Layers[layerNum - 1].getActivatedValue(), -1.0); //-al
			temp[4].Add(temp[3], 1.0);// 1-al
			temp[5].Divide(temp[2], temp[4]); //(1-y)/(1-al)
			temp[6].Divide(y_train, Layers[layerNum - 1].getActivatedValue()); // y/al
			temp[7].Times(temp[6], -1.0); // -y/al											//d_activatedValue->Add(*temp4, *temp2); //-y/al+(1-y)/(1-al)
			temp[8].Add(temp[5], temp[7]);
			Layers[i].setDActivatedValue(temp[8]);  // d_activatedValue=temp3
		}
		else
		{
			temp[9].Transpose(Layers[i + 1].getWeight());
			//temp[10]->Multiply(*temp[9], *Layers[i + 1]->getLinearValue());
			temp[10].Multiply(temp[9], Layers[i + 1].getDLinearValue());
			Layers[i].setDActivatedValue(temp[10]);  // d_activatedValue=temp3
		}

		if (Layers[i].getActFun() == relu)
			temp[1].DRelu(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == sigmoid)
			temp[1].DSigmoid(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == tanh)
			temp[1].DTanh(Layers[i].getLinearValue());

		//d_linearValue ->Times(*temp1, *d_activatedValue);
		if (Layers[i].getActFun() == none)
			temp[2] = Layers[i].getLinearValue();
		else
			temp[2].Times(temp[1], Layers[i].getDActivatedValue());
		//setback the d_linearValue to the current layer in the NN
		Layers[i].setDLinearValue(temp[2]);  //d_linearValue=temp2

		if (i - 1 < 0)
			temp[3] = x_train;
		else
			temp[3] = (Layers[i - 1].getActivatedValue());
		////***********************************************/////
		// this error caused me 4 days to solve
		// and out put the dimensions line by line is a good way to debug
		// can not use temp[3] = Layers[i - 1]->getActivatedValue();
		// because use pointer directly, later temp[3] change, the Layers[i-1]->getActivatedValue() will be automatically changed too
		/////////******************************************/////
		temp[4].Transpose(temp[3]);  // mark to check the dim
		temp[5].Multiply(temp[2], temp[4]);
		temp[6].Times(temp[5], 1.0 / (double)temp[2].getCol()); //d_weight=temp1?																  
		Layers[i].setDWeight(temp[6]);
		//to calculate d_bias, change the matrix to vector first, then expanded vector back to matrix
		temp[7].Reassign(temp[2]); //d_bias=temp4
		Layers[i].setDBias(temp[7]);
	}
}
void NeuronNetwork::BackPropagation_ppoclip(const Matrix& x_train, const Matrix& y_train, const Matrix& w_ratio, const vector<double>& adv_GAE)
{
	for (int i = layerNum - 1; i >= 0; i--)
	{
		if (i == layerNum - 1) //the last layer, also the output layer is different from other hidden layers
		{
			temp[1].Clip(w_ratio, 0.2);  // clip(r, 1-e,1+e)
			temp[2].Times(temp[1], adv_GAE); //clip(r,1-e,1+e)*adv
			temp[3].Times(w_ratio, adv_GAE); //r*adv
			temp[4].Min(temp[3], temp[2]);
			Layers[i].setDActivatedValue(temp[4]);

		}
		else
		{
			temp[9].Transpose(Layers[i + 1].getWeight());
			//temp[10]->Multiply(*temp[9], *Layers[i + 1]->getLinearValue());
			temp[10].Multiply(temp[9], Layers[i + 1].getDLinearValue());
			Layers[i].setDActivatedValue(temp[10]);  // d_activatedValue=temp3
		}

		if (Layers[i].getActFun() == relu)
			temp[1].DRelu(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == sigmoid)
			temp[1].DSigmoid(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == tanh)
			temp[1].DTanh(Layers[i].getLinearValue());

		//d_linearValue ->Times(*temp1, *d_activatedValue);
		if (Layers[i].getActFun() == none)
			temp[2] = Layers[i].getLinearValue();
		else
			temp[2].Times(temp[1], Layers[i].getDActivatedValue());
		//setback the d_linearValue to the current layer in the NN
		Layers[i].setDLinearValue(temp[2]);  //d_linearValue=temp2

		if (i - 1 < 0)
			temp[3] = x_train;
		else
			(temp[3]) = (Layers[i - 1].getActivatedValue());
		////***********************************************/////
		// this error caused me 4 days to solve
		// and out put the dimensions line by line is a good way to debug
		// can not use temp[3] = Layers[i - 1]->getActivatedValue();
		// because use pointer directly, later temp[3] change, the Layers[i-1]->getActivatedValue() will be automatically changed too
		/////////******************************************/////
		temp[4].Transpose(temp[3]);  // mark to check the dim
		temp[5].Multiply(temp[2], temp[4]);
		temp[6].Times(temp[5], 1.0 / (double)temp[2].getCol()); //d_weight=temp1?																  
		Layers[i].setDWeight(temp[6]);
		//to calculate d_bias, change the matrix to vector first, then expanded vector back to matrix
		temp[7].Reassign(temp[2]); //d_bias=temp4
		Layers[i].setDBias(temp[7]);
	}
}
void NeuronNetwork::BackPropagation_multiclass(const Matrix& x_train, const Matrix& y_train)
{
	for (int i = layerNum - 1; i >= 0; i--)
	{
		if (i == layerNum - 1) //the last layer, also the output layer is different from other hidden layers
		{
			temp[1].Times(Layers[layerNum - 1].getActivatedValue(), -1.0); //-a
			temp[2].Add(y_train, temp[1]);// y-a
			temp[3].Times(Layers[layerNum - 1].getActivatedValue(), temp[2]); //a*(y-a)
			Layers[i].setDActivatedValue(temp[3]);
		}
		else
		{
			temp[9].Transpose(Layers[i + 1].getWeight());
			//temp[10]->Multiply(*temp[9], *Layers[i + 1]->getLinearValue());
			temp[10].Multiply(temp[9], Layers[i + 1].getDLinearValue());
			Layers[i].setDActivatedValue(temp[10]);  // d_activatedValue=temp3
		}

		if (Layers[i].getActFun() == relu)
			temp[1].DRelu(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == sigmoid)
			temp[1].DSigmoid(Layers[i].getLinearValue());
		else if (Layers[i].getActFun() == tanh)
			temp[1].DTanh(Layers[i].getLinearValue());
		
		if (Layers[i].getActFun() == none)
			temp[2] = Layers[i].getLinearValue();
		else
			temp[2].Times(temp[1], Layers[i].getDActivatedValue());
		//d_linearValue ->Times(*temp1, *d_activatedValue);
		
		//setback the d_linearValue to the current layer in the NN
		Layers[i].setDLinearValue(temp[2]);  //d_linearValue=temp2

		if (i - 1 < 0)
			temp[3] = x_train;
		else
			(temp[3]) = (Layers[i - 1].getActivatedValue());
		////***********************************************/////
		// this error caused me 4 days to solve
		// and out put the dimensions line by line is a good way to debug
		// can not use temp[3] = Layers[i - 1]->getActivatedValue();
		// because use pointer directly, later temp[3] change, the Layers[i-1]->getActivatedValue() will be automatically changed too
		/////////******************************************/////
		temp[4].Transpose(temp[3]);  // mark to check the dim
		temp[5].Multiply(temp[2], temp[4]);
		temp[6].Times(temp[5], 1.0 / (double)temp[2].getCol()); //d_weight=temp1?																  
		Layers[i].setDWeight(temp[6]);
		//to calculate d_bias, change the matrix to vector first, then expanded vector back to matrix
		temp[7].Reassign(temp[2]); //d_bias=temp4
		Layers[i].setDBias(temp[7]);
	}
}
void NeuronNetwork::ParameterUpdate()
{
	for (int i = 0; i < layerNum; i++)
	{
		temp[1].Times(Layers[i].getDWeight(), learningRate*(-1.0f));
		temp[2].Add(Layers[i].getWeight(), temp[1]);
		Layers[i].setWeight(temp[2]);

		temp[1].Times(Layers[i].getDBias(), learningRate*(-1.0f));
		temp[2].Add(temp[1], Layers[i].getBias());
		Layers[i].setBias(temp[2]);
	}
}
void NeuronNetwork::Training(int backprop_num, int n_iteration, const Matrix& input, const Matrix& output, const Matrix& w_ratio, const vector<double> adv_GAE)
{
	int col = input.getCol();
	int extra = col % batchSize;
	int n_iter_batch;
	if (extra != 0) n_iter_batch = col / batchSize + 1;
	else n_iter_batch = col / batchSize;

	Matrix x_train;// = new Matrix();
	Matrix y_train; //= new Matrix();

	for (int i = 0; i < n_iteration; i++)
	{
		//cout << "i = " << i << endl;
		double sum = 0;
		for (int j = 0; j < n_iter_batch; j++)
		{

			if (j == n_iter_batch - 1) //the last batch

			{
				x_train.SplitCol(input, j * batchSize, col - 1);
				y_train.SplitCol(output, j * batchSize, col - 1);
			}
			else
			{
				x_train.SplitCol(input, j * batchSize, (j + 1) * batchSize - 1);
				y_train.SplitCol(output, j * batchSize, (j + 1) * batchSize - 1);
			}
			FeedForward(x_train);
			

			if (backprop_num == 1)
			{
				sum += CrossEntropyCost(y_train, Layers[layerNum - 1].getActivatedValue()); //here the activated value changed
				BackPropagation(x_train, y_train);
			}
			if (backprop_num == 2)
			{
				sum += CrossEntropyCost_multiclass(y_train, Layers[layerNum - 1].getActivatedValue()); //here the activated value changed
				BackPropagation_multiclass(x_train, y_train);
			}
			if (backprop_num == 3)
			{
				sum += CrossEntropyCost_ppoclip(y_train, Layers[layerNum - 1].getActivatedValue(), w_ratio, adv_GAE ); //here the activated value changed
				BackPropagation_ppoclip(x_train, y_train, w_ratio, adv_GAE);
			}
			

			ParameterUpdate();
		}

		cost.push_back(sum);
		//if (i % 10 == 0)
		//{
		cout << i << " iterations cost is: " << sum << endl;
		//PrintCost();
		//}
	}

	//SaveModel();
}

void NeuronNetwork::Evaluation(const Matrix& x_test, const Matrix& y_test)
{


	FeedForward(x_test);
	Matrix y_cal = Layers[layerNum - 1].getActivatedValue();
	int row = y_cal.getRow();
	int col = y_cal.getCol();


	vector<double> max;
	vector<int> max_id;
	for (int i = 0; i < col; i++)
	{
		double temp_max = -1;
		int temp_id = -1;
		for (int j = 0; j < row; j++)
		{
			double temp_ycal = y_cal.getV(j, i);
			if (temp_max < temp_ycal)
			{
				temp_max = temp_ycal;
				temp_id = j;
			}
		}

		max.push_back(temp_max);
		max_id.push_back(temp_id);
	}
	double accuracy = 0;

	cout << "predicts : " << endl;
	for (int i = 0; i < col; i++)
	{
		cout << max_id[i] << " ";
	}
	cout << endl;

	cout << "real nums : " << endl;
	for (int i = 0; i < col; i++)
	{
		cout << (int)y_test.getV(0, i) << " ";
	}
	cout << endl;

	for (int i = 0; i < col; i++)
	{
		if (max_id[i] == (int)y_test.getV(0, i)) accuracy += 1;
	}
	accuracy /= (double)col;

	cout << "Samples = " << col << ", accuarcy = " << accuracy << endl;


}

void NeuronNetwork::Prediction(const Matrix& x_data)
{
	FeedForward(x_data);
	Matrix y_predict = Layers[layerNum - 1].getActivatedValue();

	int row = y_predict.getRow();
	int col = y_predict.getCol();

	vector<double> max;
	vector<int> max_id;
	for (int i = 0; i < col; i++)
	{
		double temp_max = -1;
		int temp_id = -1;
		for (int j = 0; j < row; j++)
		{
			if (temp_max < y_predict.getV(j, i))
			{
				temp_max = y_predict.getV(j, i);
				temp_id = j;
			}
		}

		max.push_back(temp_max);
		max_id.push_back(temp_id);
	}
	cout << "Predict results = ";
	for (int i = 0; i < col; i++)
	{
		cout << max_id[i] << " ";
	}
	cout << endl;
}

const Matrix& NeuronNetwork::Predict(const Matrix& x_data)
{
	FeedForward(x_data);
	return  Layers[layerNum - 1].getActivatedValue();
}

void NeuronNetwork::SaveModel(string modelname)
{
	cout << "start saving model: "+ modelname << endl;
	string filename = "..\\test_ppo\\model\\"+modelname+".txt";
	ofstream file(filename);

	int row, col;

	if (file.is_open())
	{
		file << "layers: " << layerNum << endl;
		file << "learningrate: " << learningRate << endl;
		for (int k = 0; k < layerNum; k++)
		{
			row = Layers[k].getWeight().getRow();
			col = Layers[k].getWeight().getCol();

			file << "weight_" << k << ": " << row << " " << col << endl;
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < col; j++)
				{
					file << Layers[k].getWeight().getV(i, j) << " ";
				}
				file << endl;
			}

			row = Layers[k].getBias().getRow();
			col = Layers[k].getBias().getCol();

			file << "bias_" << k << ": " << row << " " << col << endl;
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < col; j++)
				{
					file << Layers[k].getBias().getV(i, j) << " ";
				}
				file << endl;
			}
		}
	}
	else
	{
		cout << "error while opening the file" << endl;
	}
	file.close();
}

void NeuronNetwork::LoadModel()
{
	cout << "start loading the model" << endl;

	string filename = "..\\model\\model.txt";
	ifstream file(filename);
	int layers; //store the layers of the neuronnetwork
	double learningrate; // store the learning rate of the neuronnetwork

	vector<Matrix*> model; // here store the weight of this layer, store the bias of the layer

	string line;
	int row, col;
	if (file.is_open())
	{
		file >> line;
		file >> layers;
		file >> line;
		file >> learningrate;
		// here init the 2*2 matrix to store the load variables, each layer has one weight and one bias
		Matrix* m;
		double temp;
		for (int k = 0; k < layers; k++)
		{
			file >> line;
			file >> row;
			file >> col;

			m = new Matrix(row, col, false);

			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < col; j++)
				{
					file >> temp;
					m->setV(i, j, temp);
				}
			}

			model.push_back(m);

			file >> line;
			file >> row;
			file >> col;

			m = new Matrix(row, col, false);
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < col; j++)
				{
					file >> temp;
					m->setV(i, j, temp);
				}
			}

			model.push_back(m);
		}
	}
	else
	{
		cout << "error while opening the file" << endl;
	}
	file.close();
}

//NeuronNetwork::~NeuronNetwork()
//{

//}

