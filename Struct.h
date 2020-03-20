#pragma once
# define jointnum 21
using namespace std;
struct Vector3
{
public: 
	double x;
	double y;
	double z;
	Vector3() { x = 0; y = 0; z = 0; };
	Vector3(double x, double y, double z) 
	{ 
		this->x = x; 
		this->y = y; 
		this->z = z; 
	}
	void print()
	{
		cout << this->x << " , " << this->y << " , " << this->z << endl;
	}

	void operator=(const Vector3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}
	Vector3& operator-(const Vector3& other)
	{
		Vector3 result;
		result.x = this->x - other.x;
		result.y = this->y - other.y;
		result.z = this->z - other.z;
		return result;
	}
	Vector3& operator+(const Vector3& other)
	{
		Vector3 result;
		result.x = this->x + other.x;
		result.y = this->y + other.y;
		result.z = this->z + other.z;
		return result;
	}
};

//struct StateStruct
//{
//	Vector3 agent_postion; //3
//	Vector3 agent_facing_direction; //3
//	Vector3* joint_angles;//21*3=57
//	Vector3* joint_positions;//21*3=57
//	StateStruct(): agent_postion(), agent_facing_direction(){ };			 //int contact[jointNum];
//	StateStruct(Vector3 pos, Vector3 facedir, Vector3* ang, Vector3* poss)
//	{
//		this->agent_postion = pos;
//		this->agent_facing_direction = facedir;
//		this->joint_angles = ang;
//		this->joint_positions = poss;
//	}
//
//};

struct StateStruct
{
	Vector3 agent_postion; //3
	Vector3 agent_facing_direction; //3
	vector<Vector3> joint_angles;//21*3=57
	vector<Vector3> joint_positions;//21*3=57
	StateStruct() : agent_postion(), agent_facing_direction() 
	{ 
		joint_angles.resize(jointnum);
		joint_positions.resize(jointnum);
	};			 //int contact[jointNum];
	StateStruct(Vector3 pos, Vector3 facedir, vector<Vector3> ang, vector<Vector3> poss)
	{
		this->agent_postion = pos;
		this->agent_facing_direction = facedir;
		this->joint_angles = ang;
		this->joint_positions = poss;
	}

};

//struct statestruct
//{
//	Vector3 pos;
//	Vector3 face;
//	vector<Vector3> j_ang;
//	vector<Vector3> j_pos;
//	statestruct() : pos(), face(), j_ang(), j_pos() {};
//	statestruct(Vector3 pos, Vector3 face, vector<Vector3> j_ang, vector<Vector3> j_pos)
//	{
//		this->pos = pos;
//		this->face = face;
//		this->j_ang = j_ang;
//		this->j_pos = j_pos;
//	}
//};

//struct ActionStruct
//{
//	//Vector3 target_location;
//	Vector3 delta_agent_position;
//	Vector3* delta_joint_angles;
//	//int action_list[actionNum];
//	//float action_time[actionNum];
//	ActionStruct() :delta_agent_position() {};
//	ActionStruct(/*Vector3 t,*/ Vector3 delta_pos, Vector3* delta_angles)
//	{
//		//this->target_location = t;
//		this->delta_agent_position = delta_pos;
//		this->delta_joint_angles = delta_angles;
//	}
//};

struct ActionStruct
{
	//Vector3 target_location;
	Vector3 delta_agent_position;
	vector<Vector3> delta_joint_angles;
	//int action_list[actionNum];
	//float action_time[actionNum];
	ActionStruct() :delta_agent_position() { delta_joint_angles.resize(jointnum); };
	ActionStruct(/*Vector3 t,*/ Vector3 delta_pos, vector<Vector3> delta_angles)
	{
		//this->target_location = t;
		this->delta_agent_position = delta_pos;
		this->delta_joint_angles = delta_angles;
	}
};

struct Record
{
	StateStruct state;
	StateStruct next_state;
	ActionStruct action;
	double reward;
	Record() :state(),next_state(),action(),reward(0) {  };
	Record(StateStruct s, ActionStruct a, double r, StateStruct sn )
	{
		this->state = s;
		this->next_state = sn;
		this->action = a;
		this->reward = r;
	}
};