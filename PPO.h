#pragma once
#include <iostream>
#include<stdlib.h>
#include <time.h>
#include<vector>
#include"Mat.h"
#include"NN.h"
#include "Struct.h"
#include "BVH.h"
using namespace std;

#define MACRO_PPO _declspec(dllexport)
class PPO
{
	// the total human joint numbers is 19
	// the total action numbers is 5
	//const static int jointNum = 19;
	//const static int actionNum = 2;
	// so the total state dimension is 120
	// the total action dimension is 16
	const static int stateDim = 132;
	const static int actionDim = 66;// 126;

	Matrix state_buffer;
	Matrix action_buffer;
	vector<double> reward_buffer;
	Matrix next_state_buffer;
	vector<Record> record_buffer;

	Matrix temp;
	
	NeuronNetwork actor;
	NeuronNetwork critic;
	NeuronNetwork actor_old;

	StateStruct mystate;
	ActionStruct myaciton;
	BVH* bvh;
	int jointnumber;
	int frames;
public:
	// constructor will need to pass all the parameters need 
	PPO() ;
	//PPO(BVH* bvh, int num_epoch, int num_minibatch, double actor_loss_coef, double value_loss_coef, float entropy_coef, float learningrate, float epsilon, float max_grad_norm); 
	PPO(BVH* bvh);
	void ActorGenerateTrajectory(int init_frame, int total_frame, int looptime);
	StateStruct GetStateInfo();
	double GetReward(const StateStruct& ref_next_state, const StateStruct& next_state);
	double Distance(const Vector3& a, const Vector3&b);
	void SampleMinibatch(int batchnum, Matrix& mat_states, Matrix& mat_actions, vector<double>& rewards, Matrix& mat_next_states);
	vector<int> SampleIndex(int num);
	void ValueFunUpdate_critic(NeuronNetwork& critic, const vector<double>& rewards, const Matrix& mat_states);
	void PolicyFunUpdate_actor(NeuronNetwork& actor, NeuronNetwork& actor_old, const Matrix& mat_states, const Matrix& mat_actions, const vector<double>& rewards, const Matrix& mat_next_states   );
	void MainLoop();
	/////// commuicate with unity
	void ReceiveState(StateStruct*& state); //receive state information from unity
	void SendAction(ActionStruct*& action); // send calculated action information back to unity
	//////
	// this function use to parse the struct state information to matrix for NN to calculate
	Matrix State_StructToMatrix(const StateStruct& state);
	// this function use to parse the matrix action information to struct to pass back to unity to calculate
	ActionStruct Action_MatrixToStruct(const Matrix& mat_action);
	Matrix Action_StructToMatrix(const ActionStruct& action);
	//delete function
	Matrix VectorToMatrix(const vector<double> vec );
	void MatrixToVector(const Matrix& mat, vector<double> vec);


	~PPO();
};

//// C++ compilers "mangle" the names of functions by default
//// This prevents that from happening
//// If we didn't do this, C# wouldn't be able to find our functions
//extern "C"
//{
//
//	MACRO_PPO PPO* CreatePPO(float clip_param, int num_epoch, int num_minibatch, double actor_loss_coef, double value_loss_coef, float entropy_coef, float learningrate, float epsilon, float max_grad_norm);
//	MACRO_PPO void ActorSample(PPO* ppo, const vector<double>& sample_init_state);
//	// Function pointer to the C# function
//	// The syntax is like this: ReturnType (*VariableName)(ParamType ParamName, ...)
//	int(*CsharpFunction)(int a, float b);
//
//	// C++ function that C# calls
//	// Takes the function pointer for the C# function that C++ can call
//	void Init(int(*csharpFunctionPtr)(int, float))
//	{
//		CsharpFunction = csharpFunctionPtr;
//	}
//
//	// Example function that calls into C#
//	void Foo()
//	{
//		// It's just a function call like normal!
//		int retVal = CsharpFunction(2, 3.14f);
//	}
//}

/*

#pragma once
#include <iostream>
#include<stdlib.h>
#include <time.h>
#include<vector>
#include<tuple>
using namespace std;

#define MACRO_RL _declspec(dllexport)

class MyRL_QLearning
{
int state_dim;
int action_dim;
int grid_rows;
int grid_cols;
int target_state;

int obst1, obst2;

double alpha; //0.1
double gamma; //0.6
double epsilon; //0.1

//tuple<double, double> pos_target;
//Point pos_agent_init;
//Point pos_groud;
double** Qtable;
double** Rtable;

public:
//MyRL_QLearning(int State_dim, int Action_dim, int grid_rows, int grid_cols, double Alpha, double Gamma, double Epsilon, tuple<double, double> Pos_target);
MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, int* target_s, int* obt1, int* obt2);
void Create_Qtable_Rtable();

void ArgumentMax(int state, double* qvalue, int* act );
int Greedy_action(int st);
void Run_env(int state, int act, int* next_state, double* reward, bool* done);
//return next_state,reward,done
int Reset_env(); //return a state number
void RL_train();
//void RL_run();
~MyRL_QLearning();
};

extern "C"
{
MACRO_RL MyRL_QLearning* Create_MyRL_QLearning(int State_dim, int Action_dim, int Grid_rows, int Grid_cols, double Alpha, double Gamma, double Epsilon, int* target_s, int* obt1, int* obt2);
MACRO_RL void Get_RL_train(MyRL_QLearning* myrl);
MACRO_RL void Get_ArgumentMax(MyRL_QLearning* myrl, int state, double* Q_max, int* act_max);
MACRO_RL int Get_Reset_env(MyRL_QLearning* myrl);
MACRO_RL void Get_Run_env(MyRL_QLearning* myrl, int state, int act, int* next_state, double* reward, bool* done);

}

*/
