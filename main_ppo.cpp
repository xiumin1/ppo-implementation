#pragma once
#include<stdlib.h>
#include<vector>
#include "BVH.h"
#include "PPO.h"
void Actor_Sample()
{
	//vector<double> states;
	//vector<double> actions;
	//vector<double> rewards;
	//vector<double> next_states;

}


int main(int argc, const char * argv[])
{
	BVH* bvh;
	bvh = new BVH();
	//D:\Box Sync\Research Projects\7.07.2019-projects\TEST\test_ppo\data
	bvh->load("D:/Box Sync/Research Projects/7.07.2019-projects/TEST/test_ppo/data/mycut.bvh");
	bvh->moveTo(0);
	vector<Vector3> jangles = bvh->getRelativeJointAngles();
	int num = bvh->getJointNumber(); // jointnum does not have value until call getrelativejointangles()
	PPO* ppo;
	ppo = new PPO(bvh);
	ppo->MainLoop();
	
	////bvh->load("../data/56_01.bvh");
	//cout << bvh->getNumFrames() << endl;
	////bvh->printJoint(bvh->getRootJoint());

	//bvh->moveTo(0);
	//Vector3* jangles = bvh->getRelativeJointAngles();
	//Vector3* jpos=bvh->getJointPositions();
	//Vector3 facedir = bvh->getFaceDirection();
	//Vector3 rootpos = bvh->getRootPosition();

	//cout << bvh->getjointnum() << endl;
	//cout << "facedirection = ";
	//facedir.print();
	//cout << "rootposition = ";
	//rootpos.print();
	

	/*int size;
	size = bvh->getRootJoint()->children.size();
	for (int i = 0; i < size; i++)
	{
		Matrix m;
		m = bvh->getRootJoint()->children[i]->matrix;
		cout << bvh->getRootJoint()->children[i]->name << endl;
		m.print();
	}
*/
	/*for (int k = 0; k < 5; k++)
	{
		bvh->moveTo(k + 1);
		size = bvh->getRootJoint()->children.size();
		for (int i = 0; i < size; i++)
		{
			Matrix m;
			m = bvh->getRootJoint()->children[i]->matrix;
			cout << bvh->getRootJoint()->children[i]->name << endl;
			m.print();
		}
	}*/
	//bvh->moveTo(5);
	//bvh->printJoint(bvh->getRootJoint());
	
	

	getchar();

	return 0;
}