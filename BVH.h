#pragma once
/**
* This code has been introduced in
* http://www.gamedev.net/page/resources/_/technical/game-programming/bvh-file-loading-and-displaying-r3295
* so it is licenced with GDOL (Gamedeve.net Open Licence)
*/

#ifndef BVH_H
#define BVH_H

#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"

//#include "../klib/logger.h"

#include "util.h"
#include "Mat.h"
#define Xposition 0x01
#define Yposition 0x02
#define Zposition 0x04
#define Zrotation 0x10
#define Xrotation 0x20
#define Yrotation 0x40

using namespace std;
typedef struct
{
	double x, y, z;
} OFFSET;

typedef struct JOINT JOINT;

struct JOINT
{
	// joint name
	const char* name = NULL;

	// joint parent
	JOINT* parent = NULL;

	// offset data
	OFFSET offset;

	// num of channels joint has
	unsigned int num_channels = 0;

	// binary or-ed channels - obsolete
	//    short channels;

	// ordered list of channels
	short* channels_order = NULL;

	// joint's children
	std::vector<JOINT*> children;

	// local transofrmation matrix (premultiplied with parents'
	Matrix matrix;
	
	// index of joint's channel data in motion array
	unsigned int channel_start = 0;
	// use to store the current time step joint angle and joint position information
	Vector3 angle;
	Vector3 position;
	void operator=(const JOINT& other)
	{
		this->matrix = other.matrix;
		this->angle = other.angle;
		this->position = other.position;
	}
};

typedef struct
{
	JOINT* rootJoint;
	int num_channels;
} HIERARCHY;

typedef struct
{
	unsigned int num_frames;            // koliko frameova motion ima
	unsigned int num_motion_channels = 0;   // koliko channela je upisan u jedan frame
	double* data = NULL;                        // pocetak motion float-ova
	unsigned* joint_channel_offsets;    // broj kanala od pocetka hijerarhije za i-ti joint
} MOTION;

typedef struct
{
	unsigned int num_hierarchies;
	unsigned int num_total_joints;
	unsigned int num_motion_channels = 0;
	unsigned int num_frames;

} bvh_statistics;

class BVH
{
	JOINT* loadJoint(std::istream& stream, JOINT* parent = NULL);
	void loadHierarchy(std::istream& stream);
	void loadMotion(std::istream& stream);
	void moveJoint(JOINT* joint, MOTION* motionData, int frame_starts_index);
	Vector3 calcJointPos(Matrix mat);
	Vector3 crossProduct(const Vector3& A, const Vector3& B);
	Vector3 findJointPos(JOINT* joint, string target);
	void moveJoint(JOINT* joint, const double (&add_framedata)[(jointnum + 1) * 3]);
	//void moveJoint(JOINT* joint, double* add_framedata);
public:
	BVH();
	~BVH();

	static Matrix* identity_matrix;
	
	void load(const std::string& filename);
	void testOutput() const;
	void printJoint(const JOINT* const joint) const;

	void moveTo(unsigned frame);
	void moveTo(
		Vector3 rootposition, 
		Vector3 delta_rootposition,
		vector<Vector3> relativejointangle, 		
		vector<Vector3> delta_relativejointangle);

	const JOINT* getRootJoint() const { return rootJoint; }
	unsigned getNumFrames() const { return motionData.num_frames; }

	vector<Vector3> getRelativeJointAngles();// { return relativeJointAngles; }
	vector<Vector3> getJointPositions();
	Vector3 getRootPosition() const;
	Vector3 getFaceDirection();
	int getJointNumber() { return jointNumber; }
private:
	JOINT * rootJoint;
	//    std::set<JOINT*> allJoints;
	MOTION motionData;
	//    bvh_statistics stats;
	vector<Vector3> relativeJointAngles;
	vector<Vector3> jointPositions;
	double rootPosition[3];
	int jointNumber;
	int counter;
};

#endif // BVH_H
