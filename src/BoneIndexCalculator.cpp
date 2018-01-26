#include "BoneIndexCalculator.h"

using namespace mmd;

BoneIndexCalculator::BoneIndexCalculator(const vector<Bone>& bones)
{
	for (unsigned int i = 0; i < bones.size(); ++i) {
		name2Index_.insert(map<string, int>::value_type(bones[i].getBoneName(), i));
	}
}


BoneIndexCalculator::~BoneIndexCalculator()
{
}


bool BoneIndexCalculator::process(int& index, const string& boneName)
{
	auto itr = name2Index_.find(boneName);
	if (itr != name2Index_.end()) {
		index = itr->second;
		return true;
	}
	return false;
}