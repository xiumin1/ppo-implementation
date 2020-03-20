#include "PPO.h"
#include "Struct.h"
#include <vector>


PPO::PPO(BVH* bvh)
{
	record_buffer.reserve(10000);
	// init actor network parameters
	this->bvh = bvh;
	this->jointnumber = bvh->getJointNumber();
	this->frames = bvh->getNumFrames();

	int actor_xdim=stateDim;
	int actor_layernum=2;
	vector<int> actor_neuronnums;// = new int[actor_layernum];
	actor_neuronnums.resize(actor_layernum);
	actor_neuronnums[0] = 520;
	actor_neuronnums[1] = actionDim;
	vector<int> actor_actfunnums;// = new int[actor_layernum];
	actor_actfunnums.resize(actor_layernum);
	actor_actfunnums[0] = 0;
	actor_actfunnums[1] = 0;
	double actor_learningrate=0.95;
	int actor_batchsize=32;

	actor = NeuronNetwork(actor_xdim, actor_layernum, actor_neuronnums, actor_actfunnums, actor_learningrate, actor_batchsize);
	actor_old = NeuronNetwork(actor_xdim, actor_layernum, actor_neuronnums, actor_actfunnums, actor_learningrate, actor_batchsize);

	//NeuronNetwork actor (actor_xdim, actor_layernum, actor_neuronnums, actor_actfunnums, actor_learningrate, actor_batchsize);
	//NeuronNetwork actor_old (actor_xdim, actor_layernum, actor_neuronnums, actor_actfunnums, actor_learningrate, actor_batchsize);

	//actor_old.get_important_part(actor);

	// init critic network parameters
	int critic_xdim=stateDim; 
	int critic_layernum=2;
	vector<int> critic_neuronnums;// = new int[critic_layernum];
	critic_neuronnums.resize(critic_layernum);
	critic_neuronnums[0] = 520;
	critic_neuronnums[1] = 1;
	vector<int> critic_actfunnums;// = new int[critic_layernum];
	critic_actfunnums.resize(critic_layernum);
	critic_actfunnums[0] = 0;
	critic_actfunnums[1] = 0;
	double critic_learningrate=0.95;
	int critic_batchsize=32;
	critic = NeuronNetwork(critic_xdim,critic_layernum, critic_neuronnums, critic_actfunnums, critic_learningrate, critic_batchsize );

	vector<double> sample_init_state;
	int state_size = 24; // for example
	
	//srand((int)time(0));
	double temp;
	for (int i = 0; i < state_size; i++)
	{
		temp = (double)rand() / (double)RAND_MAX;
		sample_init_state.push_back(temp);
	}

	//Matrix* sample_init_state = new Matrix(sample_init_state.size, 1, true);

}

Matrix PPO::VectorToMatrix(const vector<double> vec)
{
	int size = vec.size();
	Matrix mat = Matrix(1, size, false);
	//mat = new Matrix(size, 1, false);
	
	for (int i = 0; i < size; i++)
	{
		mat.setV(0, i, vec[i]);
	}
	return mat;
}

void PPO::MatrixToVector(const Matrix& mat, vector<double> vec)
{
	int row = mat.getRow();
	int col = mat.getCol();
	if (col > 1) { cout << "this is not a one column matrix!" << endl; }
	else
	{
		for (int i = 0; i < row; i++)
		{
			vec.push_back(mat.getV(i,0));
		}
	}
}
StateStruct PPO::GetStateInfo()
{
	Vector3 agent_position = bvh->getRootPosition();
	Vector3 agent_face_direction = bvh->getFaceDirection();
	vector<Vector3> joint_angles = bvh->getRelativeJointAngles();
	vector<Vector3> joint_positions = bvh->getJointPositions();

	StateStruct state(agent_position,
		agent_face_direction,
		joint_angles,
		joint_positions);

	return state;
	/*state.agent_postion = agent_position;
	state.agent_facing_direction = agent_face_direction;
	state.joint_angles = joint_angles;
	state.joint_positions = joint_positions;*/
	/*state(agent_position,
		agent_face_direction,
		joint_angles,
		joint_positions);*/
	/*state = new StateStruct(agent_position,
		agent_face_direction,
		joint_angles,
		joint_positions);*/

	//delete[] joint_angles, joint_positions;
}

double PPO::Distance(const Vector3& a, const Vector3& b)
{
	double dis = sqrt(
		  (a.x - b.x)*(a.x - b.x)
		+ (a.y - b.y)*(a.y - b.y)
		+ (a.z - b.z)*(a.z - b.z));
	return dis;
}
double PPO::GetReward(const StateStruct& ref_next_state, const StateStruct& next_state)
{
	double reward;
	double pos_difference = 0, angle_difference = 0;
	double pos_weight, angle_weight; // need to be given
	pos_weight = 0.3;
	angle_weight = 0.5;
	for (int i = 0; i < jointnum; i++)
	{
		pos_difference += Distance(
			ref_next_state.joint_positions[i], 
			next_state.joint_positions[i]);

		angle_difference += Distance(
			ref_next_state.joint_angles[i],
			next_state.joint_angles[i]);
	}
	double posd[jointnum];
	double angd[jointnum];
	double pd = 0, ad = 0;
	for (int i = 0; i < jointnum; i++)
	{
		posd[i] = Distance(ref_next_state.joint_positions[i],
			next_state.joint_positions[i]);
		angd[i]= Distance(
			ref_next_state.joint_angles[i], // when i=9, joint_angles.x is super large
			next_state.joint_angles[i]);
		pd += posd[i];
		ad += angd[i];
	}

	reward = pos_weight * pos_difference + angle_weight * angle_difference;
	return reward;
}
void PPO::ActorGenerateTrajectory(int init_frame, int frames, int looptime)
{
	StateStruct state;// = new StateStruct;
	StateStruct next_state;// = new StateStruct;
	ActionStruct action;// = new ActionStruct;

	Matrix mat_state;// = new Matrix();
	Matrix mat_next_state;// = new Matrix();
	Matrix mat_action;// = new Matrix();

	StateStruct ref_next_state;// = new StateStruct;
	Matrix ref_mat_next_state;// = new Matrix();

	int frame;
	for (int i = 0; i < looptime; i++)
	{
		//run through reference motion
		// so the first frame should be the second frame in motion data
		frame = (init_frame + i + 1) % frames;
		bvh->moveTo(frame);
		ref_next_state=GetStateInfo();
		// rollout the trajectory
		if (i == 0)
		{
			frame = init_frame;
			bvh->moveTo(frame);
			state=GetStateInfo();
		}
		else
		{			
			state = next_state;			
		}
		mat_state = State_StructToMatrix(state);  
		actor.FeedForward(mat_state);
		mat_action = actor.getOutput();
		action=Action_MatrixToStruct(mat_action);
		Vector3 delta_agent_position = action.delta_agent_position;
		vector<Vector3> delta_joint_angles = action.delta_joint_angles;
		// the environment move one more frame step
		bvh->moveTo(state.agent_postion,
			delta_agent_position,
			state.joint_angles,		
			delta_joint_angles); 
		next_state = GetStateInfo();
		double reward = GetReward(ref_next_state, next_state);
		//Record* record = new Record(state,action,reward,next_state);
		Record record(state, action, reward, next_state);
		//record_buffer[i] = record;
		// make sure the record buffer always has a size of 10000
		if (record_buffer.size() > 10000)
			record_buffer.erase(record_buffer.begin());
		record_buffer.push_back(record);
	}
	
	/*
	Matrix* state_mat=new Matrix();
	Matrix* action_mat=new Matrix();
	StateStruct state_struct;
	ActionStruct action_struct;
	StateStruct next_state_struct;
	Matrix* next_state_mat=new Matrix();

	double reward;
	State_StructToMatrix(init_state, state_mat);
	int iter = 100;
	for (int i = 0; i < iter; i++)
	{
		//call getcurrentstate function from c#

		temp = state_buffer;
		state_buffer.PushEndColumn(temp, *state_mat);

		actor->FeedForward(*state_mat);
		actor->getOutput(action_mat);
		Action_MatrixToStruct(*action_mat, &action_struct);

		temp = action_buffer;
		action_buffer.PushEndColumn(temp, *action_mat);

		//SimulateEnvironment(*state_struct, *action_struct, &reward, *next_state_struct);
		// call applycurrentaction function from c#, and recieve reward and nextstate
		
		reward_buffer.push_back(reward);

		State_StructToMatrix(next_state_struct, next_state_mat);
		temp = next_state_buffer;
		next_state_buffer.PushEndColumn(temp, *next_state_mat);

		state_mat = next_state_mat;

	}
	*/
}
vector<int> PPO::SampleIndex(int num)
{
	vector<int> indexs;
	srand((int)time(0));

	for (int i = 0; i < num; i++)
	{
		indexs.push_back(rand() % record_buffer.size());
	}
	return indexs;
}
void PPO::SampleMinibatch(int batchnum, Matrix& mat_states, Matrix& mat_actions, vector<double>& rewards, Matrix& mat_next_states)
{
	vector<int> indexs;
	indexs = SampleIndex(batchnum);

	//rewards = new double[batchnum];

	vector<Record> batch_record_buffer;
	batch_record_buffer.resize(batchnum);
	for (int i = 0; i < batchnum; i++)
	{
		Record sample_record;// = new Record;
		sample_record = record_buffer[indexs[i]];
		batch_record_buffer[i]= sample_record;
	}
	//mat_states = new Matrix((2 + 2 * jointnum) * 3, batchnum, false);
	//mat_actions = new Matrix((1 + jointnum) * 3, batchnum, false);
	//mat_next_states = new Matrix((2 + 2 * jointnum) * 3, batchnum, false);
	Matrix mat_state;// = new Matrix();
	Matrix mat_action;// = new Matrix();
	Matrix mat_next_state;// = new Matrix();
	for (int i = 0; i < batchnum; i++)
	{
		mat_state = State_StructToMatrix(batch_record_buffer[i].state);
		mat_next_state = State_StructToMatrix(batch_record_buffer[i].next_state );
		mat_action = Action_StructToMatrix(batch_record_buffer[i].action );
		rewards[i] = batch_record_buffer[i].reward;

		for (int j = 0; j < (2 + 2 * jointnum) * 3; j++)
		{
			mat_states.setV(j, i, mat_state.getV(j, 0));
			mat_next_states.setV(j, i, mat_next_state.getV(j, 0));
		}

		for (int j = 0; j < (1 + jointnum) * 3; j++)
		{
			mat_actions.setV(j, i, mat_action.getV(j, 0));
		}
		
	}

}
void PPO::ValueFunUpdate_critic(NeuronNetwork& critic, const vector<double>& rewards, const Matrix& mat_states )
{
	// this part is used to calculate the target value
	int batchnum = rewards.size();
	vector<double> state_values;
	state_values.resize(batchnum + 1);
	for (int i = 0; i < batchnum + 1; i++)
	{
		state_values[i] = 0.0;
	}
	vector <double> eligibility;
	eligibility.resize(batchnum + 1);
	double lambda = 0.95; // those values need to be reassigned later.
	double gamma = 0.1; 
	double td_error = 0;
	double alpha = 0.5;
	for (int i = 0; i < batchnum; i++)
	{
		for (int j = 0; j < batchnum; j++)
		{
			eligibility[j] = eligibility[j] * lambda * gamma;
		}
		eligibility[i] += 1.0;
		td_error = rewards[i] + gamma * state_values[i + 1] - state_values[i];
		state_values[i] = state_values[i] + alpha * td_error*eligibility[i];
	}
	// then need to update the critic network weights
	state_values.erase(state_values.end() - 1);
	Matrix td_state_values;// = new Matrix();
	td_state_values = VectorToMatrix(state_values);
	//Matrix td_state_values_trans;
	//td_state_values_trans.Transpose(*td_state_values);
	int epoches = 3;
	Matrix a; //dummy variable
	vector<double> b; //dummy variable
	critic.Training(2, epoches, mat_states, td_state_values, a, b); // here 2->multiclass regression

}

void PPO::PolicyFunUpdate_actor(NeuronNetwork& actor, NeuronNetwork& actor_old, const Matrix& mat_states, const Matrix& mat_actions, const vector<double>& rewards, const Matrix& mat_next_states)
{
	int batchnum = rewards.size();
	vector<double> td_residual;
	vector<double> adv_values;
	vector<double> gamma_lamda;
	vector<double> adv_GAE_values;
	vector<double> reward;

	td_residual.reserve(batchnum);
	adv_values.reserve(batchnum);
	gamma_lamda.reserve(batchnum);
	adv_GAE_values.reserve(batchnum);
	reward.reserve(batchnum);

	Matrix predict_next_statesvalue;// = new Matrix();
	Matrix predict_statesvalue;// = new Matrix();
	double gamma = 0.5;
	double lamda = 0.99;

	predict_statesvalue = critic.Predict(mat_states );
	predict_next_statesvalue = critic.Predict(mat_next_states );
	for (int i = 0; i < batchnum; i++)
	{
		if (i == 0) gamma_lamda.push_back(gamma*lamda);
		else gamma_lamda.push_back(gamma*lamda*gamma_lamda[i - 1]);
		// down here has problems
		double res = rewards[i] + gamma * predict_next_statesvalue.getV(0, i) - predict_statesvalue.getV(0, i);
		td_residual.push_back(res);
		//td_residual.push_back(rewards[i] + gamma * predict_next_statesvalue->getV(i, 0) - predict_statesvalue->getV(i, 0));
		adv_values.push_back(gamma_lamda[i] * td_residual[i]);

		if (i == 0) adv_GAE_values.push_back(adv_values[i]);
		else adv_GAE_values.push_back(adv_GAE_values[i - 1] + adv_values[i]);
	}
	// calculate the cross entropy of the old policy
	Matrix predict_action_old;// = new Matrix();
	predict_action_old = actor_old.Predict(mat_states);
	Matrix pre_soft_old;// = new Matrix();
	Matrix pre_log_old;// = new Matrix();
	Matrix cross_entropy_old;// = new Matrix();
	pre_soft_old.Softmax(predict_action_old);
	pre_log_old.Log(pre_soft_old);
	cross_entropy_old.Times(mat_actions, pre_log_old);
	// calculate the cross entropy of the current policy
	Matrix predict_action;// = new Matrix();
	predict_action = actor.Predict(mat_states);
	Matrix pre_soft;// = new Matrix();
	Matrix pre_log;// = new Matrix();
	Matrix cross_entropy;// = new Matrix();
	pre_soft.Softmax(predict_action);
	pre_log.Log(pre_soft);
	cross_entropy.Times(mat_actions, pre_log); // actDim*batchnum matrix
	// calculate the ratio of two policies
	Matrix w_ratio= Matrix(actionDim, batchnum, false);
	double temp = 0;
	for (int i = 0; i < batchnum; i++)
	{
		for (int j = 0; j < actionDim; j++)
		{
			temp = exp(cross_entropy.getV(j, i) - cross_entropy_old.getV(j, i));
			w_ratio.setV(j, i, temp);
		}		
	}
	int epoches = 3;
	actor.Training(3, epoches, mat_states, mat_actions, w_ratio, adv_GAE_values);
}
void PPO::MainLoop()
{
	int epoches = 5;// 1000;
	int random_init = 0;
	//int total_frames = 61;
	int collectmdp_loop = 400;

	int steps = 5;// 1000;

	int batchnum = 32;

	Matrix mat_states =  Matrix((2 + 2 * jointnum) * 3, batchnum, false);
	Matrix mat_actions =  Matrix((1 + jointnum) * 3, batchnum, false);
	Matrix mat_next_states =  Matrix((2 + 2 * jointnum) * 3, batchnum, false);
	vector<double> rewards;
	//rewards.reserve();
	rewards.resize(batchnum);
	//double rewards[batchnum];

	for (int epoch = 0; epoch < epoches; epoch++)
	{
		random_init = rand() % frames;
		ActorGenerateTrajectory(random_init, frames, collectmdp_loop);

		//NeuronNetwork actor_old (actor);
		//NeuronNetwork temp = actor;
		actor_old.copyNetworkWeights(actor);
		for (int i = 0; i < steps; i++)
		{
			SampleMinibatch(batchnum, mat_states, mat_actions, rewards, mat_next_states);
			ValueFunUpdate_critic(critic, rewards, mat_states);
			critic.SaveModel("valueFun_model_" + to_string(epoch*steps + i));
			PolicyFunUpdate_actor(actor, actor_old, mat_states, mat_actions, rewards, mat_next_states);
			critic.SaveModel("policyFun_model_" + to_string(epoch*steps + i));
			cout << "loop steps "<< i << endl;
		}
		//actor = temp;
		cout << "loop epoches " << epoch<<endl;
	}
	cout << "mainloop called" << endl;
}

void PPO::ReceiveState(StateStruct*& state)
{
	mystate = *state;
}

void PPO::SendAction(ActionStruct*& action)
{
	
}

Matrix PPO::State_StructToMatrix(const StateStruct& state)
{
	Matrix mat_state(3 * (2 + 2 * jointnum), 1, false);
	//mat_state = new Matrix(3 * (2 + 2 * jointnum), 1, false);

	mat_state.setV(0, 0, state.agent_postion.x);
	mat_state.setV(1, 0, state.agent_postion.y);
	mat_state.setV(2, 0, state.agent_postion.z);

	mat_state.setV(3, 0, state.agent_facing_direction.x);
	mat_state.setV(4, 0, state.agent_facing_direction.y);
	mat_state.setV(5, 0, state.agent_facing_direction.z);


	for (int i = 0; i < jointnum; i++)
	{
		mat_state.setV(i * 3 + 2 * 3 + 0, 0, state.joint_angles[i].x);
		mat_state.setV(i * 3 + 2 * 3 + 1, 0, state.joint_angles[i].y);
		mat_state.setV(i * 3 + 2 * 3 + 2, 0, state.joint_angles[i].z);

	}
	for (int i = 0; i < jointnum; i++)
	{
		mat_state.setV(i * 3 + (2 + jointnum) * 3 + 0, 0, state.joint_positions[i].x);
		mat_state.setV(i * 3 + (2 + jointnum) * 3 + 1, 0, state.joint_positions[i].y);
		mat_state.setV(i * 3 + (2 + jointnum) * 3 + 2, 0, state.joint_positions[i].z);

	}

	return mat_state;
}

ActionStruct PPO::Action_MatrixToStruct(const Matrix& mat_action)
{
	ActionStruct action;
	Vector3 delta_pos;
	vector<Vector3> delta_ang;// = new Vector3[jointnum];
	//delta_ang.reserve(jointnum);
	//delta_ang.reserve(100);
	delta_ang.resize(jointnum);
	//int test = jointnum;

	delta_pos.x = mat_action.getV(0, 0);
	delta_pos.y = mat_action.getV(1, 0);
	delta_pos.z = mat_action.getV(2, 0);
	for (int i = 1; i < jointnum + 1; i++)
	{
		delta_ang[i-1].x = mat_action.getV(i * 3 + 0, 0);
		delta_ang[i-1].y = mat_action.getV(i * 3 + 1, 0);
		delta_ang[i-1].z = mat_action.getV(i * 3 + 2, 0);
	}
	action.delta_agent_position = delta_pos;
	action.delta_joint_angles = delta_ang;

	return action;
}
Matrix PPO::Action_StructToMatrix(const ActionStruct& action)
{
	Matrix mat_action(3 * (2 + 2 * jointnum), 1, false);
	//mat_action = new Matrix(3 * (2 + 2 * jointnum), 1, false);

	mat_action.setV(0, 0, action.delta_agent_position.x);
	mat_action.setV(1, 0, action.delta_agent_position.y);
	mat_action.setV(2, 0, action.delta_agent_position.z);

	for (int i = 0; i < jointnum; i++)
	{
		mat_action.setV(i * 3 + 1 * 3 + 0, 0, action.delta_joint_angles[i].x);
		mat_action.setV(i * 3 + 1 * 3 + 1, 0, action.delta_joint_angles[i].y);
		mat_action.setV(i * 3 + 1 * 3 + 2, 0, action.delta_joint_angles[i].z);
	}

	return mat_action;
}


PPO::~PPO()
{
}
