#include "BVH.h"

/**
* This code has been introduced in
* http://www.gamedev.net/page/resources/_/technical/game-programming/bvh-file-loading-and-displaying-r3295
* so it is licenced with GDOL (Gamedeve.net Open Licence)
*/

#include "bvh.h"
Matrix* BVH::identity_matrix;
string endnames[5] = { "head","rHand","lHand","rFoot","lFoot" };
BVH::BVH(): rootJoint(NULL)
{
	motionData.data = 0;
	// setup the identity matrix
	identity_matrix = new Matrix(4, 4, false);
	for (int i = 0; i < 4; i++)
	{
		identity_matrix->setV(i, i, 1);
	}

	counter = 0;
}

void deleteJoint(JOINT* joint)
{
	if (joint == NULL)
		return;

	for (JOINT* child : joint->children)
		deleteJoint(child);

	if (joint->channels_order != NULL)
		delete joint->channels_order;

	//    if( joint->name != NULL && joint->name != "EndSite" ){
	//        klog.l() << joint->name;
	//        delete joint->name;
	//    }

	delete joint;
}

BVH::~BVH()
{
	cout << "BVH destructor" << endl;
	deleteJoint(rootJoint);

	//    for(JOINT* joint : allJoints)
	//    {

	//        delete joint->channels_order;
	//        delete joint->name;
	//        delete joint;
	//    }


	if (motionData.data != NULL)
		delete[] motionData.data;
}

void BVH::load(const std::string& filename)
{
	std::fstream file;
	file.open(filename.c_str(), std::ios_base::in);

	if (file.is_open())
	{
		std::string line;

		while (file.good())
		{
			file >> line;
			cout << "load" << line<<endl;
			//klog.l("load") << line;
			if (trim(line) == "HIERARCHY")
				loadHierarchy(file);
			break;
		}
		file.close();
	}
}
vector<Vector3> BVH::getRelativeJointAngles()
{
	//int size = 9;
	//static Vector3 v[size];

//std:vector<int> vv; vv.size(999);

	//memory framentation

	//jointnum = relativeJointAngles.size();
	//int size = relativeJointAngles.size();
	//Vector3* jointangles = new Vector3[size];
	//cout << "jointangles init" << endl;
	//for (int i = 0; i < size; i++)
	//{
	//	jointangles[i] = relativeJointAngles[i];
	//}
	//copy(relativeJointAngles.begin(), relativeJointAngles.end(), jointangles);
	return relativeJointAngles;
}

vector<Vector3> BVH::getJointPositions()
{
	/*int size = jointPositions.size();
	Vector3* jointpositions = new Vector3[size];
	for (int i = 0; i < size; i++)
	{
		jointpositions[i] = jointPositions[i];
	}
	*/
	//copy(jointPositions.begin(), jointPositions.end(), jointpositions);
	return jointPositions;
}
Vector3 BVH::getRootPosition() const
{
	Vector3 pos;
	pos.x = rootPosition[0];
	pos.y = rootPosition[1];
	pos.z = rootPosition[2];
	return pos;
}
void BVH::printJoint(const JOINT* const joint) const
{
	//klog.l("joint") << "print joint" << joint->name << joint->channel_start;
	cout<<"joint " << joint->name <<" "<< joint->channel_start<<endl;
	for (std::vector<JOINT*>::const_iterator ct = joint->children.begin();
		ct != joint->children.end(); ++ct)
	{
		JOINT* _tmp = *ct;

		if (_tmp->children.size() > 0)
			printJoint(_tmp);
	}

}
bool checkJointCut(string name)
{
	for (int i = 0; i < 5; i++)
	{
		if (endnames[i]==name) // here compare find if there is chars the same, but == compare exactly the same string
		{
			return true;
		}
	}
	return false;
}
Vector3 BVH::calcJointPos(Matrix mat)
{
	Vector3 jointpos;
	//Matrix* newpos_matrix=new Matrix(4, 1, false);
	//Matrix* rootpos_matrix=new Matrix(4, 1, false);
	//for (int i = 0; i < 3; i++)
	//{
	//	//rootpos_matrix->setV(i, 0, rootPosition[i]);
	//	// here use the origin (0,0,0) as the point, so matrix is (0,0,0,1)
	//	rootpos_matrix->setV(i, 0, 0);
	//}

	Matrix newpos_matrix(4, 1, false);
	Matrix rootpos_matrix(4, 1, false);
	for (int i = 0; i < 3; i++)
	{
		//rootpos_matrix->setV(i, 0, rootPosition[i]);
		// here use the origin (0,0,0) as the point, so matrix is (0,0,0,1)
		rootpos_matrix.setV(i, 0, 0);
	}
	rootpos_matrix.setV(3, 0, 1);
	newpos_matrix.Multiply(mat, rootpos_matrix);
	jointpos.x = newpos_matrix.getV(0, 0) / newpos_matrix.getV(3, 0);
	jointpos.y = newpos_matrix.getV(1, 0) / newpos_matrix.getV(3, 0);
	jointpos.z = newpos_matrix.getV(2, 0) / newpos_matrix.getV(3, 0);


	/*rootpos_matrix->setV(3, 0, 1);
	newpos_matrix->Multiply(mat,*rootpos_matrix);
	jointpos.x = newpos_matrix->getV(0, 0) / newpos_matrix->getV(3, 0);
	jointpos.y = newpos_matrix->getV(1, 0) / newpos_matrix->getV(3, 0);
	jointpos.z = newpos_matrix->getV(2, 0) / newpos_matrix->getV(3, 0);
*/
	return jointpos;
}
Vector3 BVH::crossProduct(const Vector3& A, const Vector3& B)
{
	Vector3 C;
	C.x = A.y*B.z - A.z*B.y;
	C.y = A.z*B.x - A.x*B.z;
	C.z = A.x*B.y - A.y*B.x;
	return C;
}
Vector3 BVH::getFaceDirection() // does not work!!!!!!!!
{
	Vector3 chest;
	Vector3 leftshoulder;
	Vector3 rightshoulder;
	//Vector3* rootpos = new Vector3;
	//findJointPos(rootJoint, "hip", rootpos);
	//rootpos->print();

	chest = findJointPos(rootJoint, "chest");
	leftshoulder = findJointPos(rootJoint, "lCollar");
	rightshoulder = findJointPos(rootJoint, "rCollar");

	/*if (chest == NULL) cout << "ches is null!" << endl;
	else chest->print();
	if (leftshoulder == NULL) cout << "leftshoudler is null" << endl;
	else leftshoulder->print();
	if (rightshoulder == NULL) cout << "rightshoulder is null" << endl;
	else rightshoulder->print();
*/
	Vector3 t1 = leftshoulder - chest;
	Vector3 t2 = rightshoulder - leftshoulder;
	Vector3 facedirection = crossProduct(t1, t2);
	return facedirection;
}

//JOINT* HandJnt = 0;
Vector3 BVH::findJointPos(JOINT* joint, string target)
{
	Vector3 myvector;
	
	if (joint->name == target)
	{
		//cout << "find the name is = " << joint->name << " pos = ";
		//joint->position.print();
		myvector.x = joint->position.x;
		myvector.y = joint->position.y;
		myvector.z = joint->position.z;
	}
	else
	{
		if (!checkJointCut(joint->name))
		{
			for (auto& child : joint->children)
			{
				findJointPos(child, target);
			}
		}			
	}
	return myvector;
}
// need a search function to find the joint by its name
// calculate local transformation matrix for specific frame - revisit this
void BVH::moveJoint(JOINT* joint, MOTION* motionData, int frame_starts_index)
{
	Matrix temp;// to hold the middle matrix during the calculation
	int start_index = frame_starts_index + joint->channel_start;

	joint->matrix.Translate(*BVH::identity_matrix,//
		Vector3(joint->offset.x, 
			joint->offset.y, 
			joint->offset.z));

	double angles[3];
	for (unsigned int i = 0; i < joint->num_channels; i++)
	{
		const short& channel = joint->channels_order[i];

		double value = motionData->data[start_index + i];
		// this if statement use to store the joint relative angle informaiton
		if (joint == rootJoint)
		{ 
			if (i < 3) rootPosition[i] = value;
			else angles[i - 3] = value;
		}
		else
		{
			angles[i] = value;
		}

		if (channel & Xposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(value, 0, 0));
			temp.Translate(joint->matrix, Vector3(value, 0, 0));
			joint->matrix = temp;
		}
		if (channel & Yposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(0, value, 0));
			temp.Translate(joint->matrix, Vector3(0,value,0));
			joint->matrix = temp;
		}
		if (channel & Zposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(0, 0, value));
			temp.Translate(joint->matrix, Vector3(0, 0, value));
			joint->matrix = temp;
		}
		// important, for rotation, has to convert euler angle to radian angle
		if (channel & Xrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(1, 0, 0));
			temp.Rotate(joint->matrix, value, Vector3(1, 0, 0));
			joint->matrix = temp;
		}
		if (channel & Yrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0, 1, 0));
			temp.Rotate(joint->matrix, value, Vector3(0, 1, 0));
			joint->matrix = temp;
		}
		if (channel & Zrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0, 0, 1));
			temp.Rotate(joint->matrix, value, Vector3(0, 0, 1));
			joint->matrix = temp;
		}
	}
	joint->angle = Vector3(angles[0], angles[1], angles[2]);
	if (relativeJointAngles.size() > 20) relativeJointAngles.clear();
	relativeJointAngles.push_back(Vector3(angles[0], angles[1], angles[2]));
	// mul current matrix with one of a parent
	if (joint->parent != NULL)
	{
		temp.Multiply(joint->parent->matrix, joint->matrix);
		joint->matrix = temp;
	}
	//after the matrix has been updated to global matrix, use this to calculate the global positon of each joint
	Vector3 temp_jpos = calcJointPos(joint->matrix);
	joint->position = temp_jpos;
	if (jointPositions.size() > 20) jointPositions.clear();
	jointPositions.push_back(temp_jpos);
	// put the current value to the current joint position
	//joint->position = jointPositions.back();
	// if this is the end effector as defined by endnames[] then stop visiting its children
	if (!checkJointCut(joint->name)) 
	{
		for (auto& child : joint->children)
			moveJoint(child, motionData, frame_starts_index);
	}
	
}

void BVH::moveTo(unsigned frame)
{
	// calculate start index for specific frame for motion data
	unsigned start_index = frame * motionData.num_motion_channels;

	// skeleton transformation
	moveJoint(rootJoint, &motionData, start_index);
}
void BVH::moveTo(Vector3 rootposition, Vector3 delta_rootposition, vector<Vector3> relativejointangle, vector<Vector3> delta_relativejointangle)
{
	
	//int size = 21; //jointnum
	//int arraysize = 66;// (jointnum + 1) * 3;
	double framedata[(jointnum + 1) * 3];
	double delta_framedata[(jointnum + 1) * 3];
	double add_framedata[(jointnum + 1) * 3];
	//delta_framedata = new double[(size + 1) * 3];
	//framedata = new double[(size + 1) * 3];
	//add_framedata = new double[(size + 1) * 3];

	framedata[0] = rootposition.x;
	framedata[1] = rootposition.y;
	framedata[2] = rootposition.z;

	delta_framedata[0] = delta_rootposition.x;
	delta_framedata[1] = delta_rootposition.y;
	delta_framedata[2] = delta_rootposition.z;

	for (int i = 0; i < jointnum; i++)
	{
		framedata[i * 3 + 3] = relativejointangle[i].x;
		framedata[i * 3 + 4] = relativejointangle[i].y;
		framedata[i * 3 + 5] = relativejointangle[i].z;

		delta_framedata[i * 3 + 3] = delta_relativejointangle[i].x;
		delta_framedata[i * 3 + 4] = delta_relativejointangle[i].y;
		delta_framedata[i * 3 + 5] = delta_relativejointangle[i].z;

	}

	for (int i = 0; i < (jointnum + 1) * 3; i++)
	{
		add_framedata[i] = framedata[i] + delta_framedata[i];
	}
	counter = 0;
	moveJoint(rootJoint, add_framedata);
}
void BVH::moveJoint(JOINT* joint, const double (&add_framedata)[(jointnum+1)*3])// double* add_framedata)
{
	//cout << "joint name= " << joint->name <<", channel number= "<<joint->num_channels<< ", counter = "<<counter<<endl;
	Matrix temp;// to hold the middle matrix during the calculation
	//int start_index = joint->channel_start;

	joint->matrix.Translate(*BVH::identity_matrix,//
		Vector3(joint->offset.x,
			joint->offset.y,
			joint->offset.z));

	double angles[3];
	for (unsigned int i = 0; i < joint->num_channels; i++)
	{
		const short& channel = joint->channels_order[i];

		counter++;
		double value = add_framedata[counter-1];
		//double value = add_framedata[start_index + i];
		// this if statement use to store the joint relative angle informaiton
		if (joint == rootJoint)
		{
			if (i < 3) rootPosition[i] = value;
			else angles[i - 3] = value;
		}
		else
		{
			angles[i] = value;
		}

		if (channel & Xposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(value, 0, 0));
			temp.Translate(joint->matrix, Vector3(value, 0, 0));
			joint->matrix = temp;
		}
		if (channel & Yposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(0, value, 0));
			temp.Translate(joint->matrix, Vector3(0, value, 0));
			joint->matrix = temp;
		}
		if (channel & Zposition)
		{
			//joint->matrix = glm::translate(joint->matrix, glm::vec3(0, 0, value));
			temp.Translate(joint->matrix, Vector3(0, 0, value));
			joint->matrix = temp;
		}

		if (channel & Xrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(1, 0, 0));
			temp.Rotate(joint->matrix, value, Vector3(1, 0, 0));
			joint->matrix = temp;
		}
		if (channel & Yrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0, 1, 0));
			temp.Rotate(joint->matrix, value, Vector3(0, 1, 0));
			joint->matrix = temp;
		}
		if (channel & Zrotation)
		{
			//joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0, 0, 1));
			temp.Rotate(joint->matrix, value, Vector3(0, 0, 1));
			joint->matrix = temp;
		}
	}
	joint->angle = Vector3(angles[0], angles[1], angles[2]);
	if (relativeJointAngles.size() > 20) relativeJointAngles.clear();
	relativeJointAngles.push_back(Vector3(angles[0], angles[1], angles[2]));
	// mul current matrix with one of a parent
	if (joint->parent != NULL)
	{
		temp.Multiply(joint->parent->matrix, joint->matrix);
		joint->matrix = temp;
	}
	//after the matrix has been updated to global matrix, use this to calculate the global positon of each joint
	Vector3 temp_jpos = calcJointPos(joint->matrix);
	joint->position = temp_jpos;
	if (jointPositions.size() > 20) jointPositions.clear();
	jointPositions.push_back(temp_jpos);
	// if this is the end effector as defined by endnames[] then stop visiting its children
	if (!checkJointCut(joint->name))
	{
		for (auto& child : joint->children)
			moveJoint(child, add_framedata);
	}
}
void BVH::testOutput() const
{
	if (rootJoint == 0)
		return;
	cout << "output" << endl;
	printJoint(rootJoint);

	cout << "num frames: " << motionData.num_frames << endl;
	cout << "num motion channels: " << motionData.num_motion_channels << endl;
}

void BVH::loadHierarchy(std::istream& stream)
{
	std::string tmp;

	while (stream.good())
	{
		stream >> tmp;
		cout << "hier" << tmp<<endl;

		if (trim(tmp) == "ROOT")
			rootJoint = loadJoint(stream);
		else if (trim(tmp) == "MOTION")
			loadMotion(stream);
	}
}

void BVH::loadMotion(std::istream& stream)
{
	std::string tmp;
	while (stream.good())
	{
		stream >> tmp;

		if (trim(tmp) == "Frames:")
		{
			stream >> motionData.num_frames;
		}
		else if (trim(tmp) == "Frame")
		{
			float frame_time;
			stream >> tmp >> frame_time;

			int num_frames = motionData.num_frames;
			int num_channels = motionData.num_motion_channels;

			cout << "num frames:" << num_frames << endl;
			cout << "num channels:" << num_channels << endl;
			motionData.data = new double[num_frames * num_channels];

			for (int frame = 0; frame < num_frames; frame++)
			{
				for (int channel = 0; channel < num_channels; channel++)
				{
					double x;
					std::stringstream ss;
					stream >> tmp;
					ss << tmp;
					ss >> x;

					int index = frame * num_channels + channel;
					motionData.data[index] = x;
				}
			}
		}
	}
}

JOINT* BVH::loadJoint(std::istream& stream, JOINT* parent)
{
	JOINT* joint = new JOINT;
	joint->parent = parent;

	//    stats.num_total_joints++;
	//    allJoints.insert(joint);

	// load joint name
	std::string* name = new std::string;
	stream >> *name;
	joint->name = name->c_str();

	std::string tmp;
	//joint->matrix = glm::mat4(1.0);
	joint->matrix = *identity_matrix;

	static int _channel_start = 0;
	unsigned channel_order_index = 0;
	while (stream.good())
	{
		stream >> tmp;
		tmp = trim(tmp);

		// setting channels
		char c = tmp.at(0);
		if (c == 'X' || c == 'Y' || c == 'Z')
		{
			if (tmp == "Xposition")
			{
				joint->channels_order[channel_order_index++] = Xposition;
			}
			if (tmp == "Yposition")
			{
				joint->channels_order[channel_order_index++] = Yposition;
			}
			if (tmp == "Zposition")
			{
				joint->channels_order[channel_order_index++] = Zposition;
			}

			if (tmp == "Xrotation")
			{
				joint->channels_order[channel_order_index++] = Xrotation;
			}
			if (tmp == "Yrotation")
			{
				joint->channels_order[channel_order_index++] = Yrotation;
			}
			if (tmp == "Zrotation")
			{
				joint->channels_order[channel_order_index++] = Zrotation;
			}
		}

		if (tmp == "OFFSET")
		{
			stream >> joint->offset.x
				>> joint->offset.y
				>> joint->offset.z;
		}
		else if (tmp == "CHANNELS")
		{
			// loading num of channels
			stream >> joint->num_channels;

			// adding to statistics
			motionData.num_motion_channels += joint->num_channels;

			// increasing static counter of channel index startin motion section
			joint->channel_start = _channel_start;
			_channel_start += joint->num_channels;

			// creating array for channel order specification
			joint->channels_order = new short[joint->num_channels];

		}
		else if (tmp == "JOINT")
		{
			JOINT* tmp_joint = loadJoint(stream, joint);

			tmp_joint->parent = joint;
			joint->children.push_back(tmp_joint);

			//            allJoints.insert(tmp_joint);

		}
		else if (tmp == "End")
		{
			// End Site {
			stream >> tmp >> tmp;

			JOINT* tmp_joint = new JOINT;

			tmp_joint->parent = joint;
			tmp_joint->num_channels = 0;
			tmp_joint->name = "EndSite";
			joint->children.push_back(tmp_joint);

			//            allJoints.insert(tmp_joint);

			stream >> tmp;
			if (tmp == "OFFSET")
				stream >> tmp_joint->offset.x
				>> tmp_joint->offset.y
				>> tmp_joint->offset.z;

			// ucitavanje }
			stream >> tmp;
		}
		else if (tmp == "}")
			return joint;

	}
}

