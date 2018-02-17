#include "MotionStreamGenerator.h"

using namespace mmd;

MotionStreamGenerator::MotionStreamGenerator() {
}


MotionStreamGenerator::~MotionStreamGenerator() {
}


void MotionStreamGenerator::generate(vector<MotionStream>& motionStreams, const vector<Motion>& motions, const vector<Bone>& bones) {
    // MotionをBoneごとに分ける
    vector<vector<Motion>> boneMotions(bones.size());
    for (unsigned int i = 0; i < motions.size(); ++i) {
        boneMotions[motions[i].getBoneIndex()].push_back(motions[i]);
    }

    // Motionをフレーム順にソート
    for (unsigned int i = 0; i < boneMotions.size(); ++i) {
        std::sort(boneMotions[i].begin(), boneMotions[i].end(), [](const mmd::Motion& a, const mmd::Motion& b){
            return a.getFrameNo() < b.getFrameNo();
        });
    }

    // フレーム0のモーションが存在しなければ挿入する
    for (unsigned int boneIndex = 0; boneIndex < boneMotions.size(); ++boneIndex) {
        auto head = boneMotions[boneIndex].begin();
        if (head == boneMotions[boneIndex].end() || head->getFrameNo() != 0) {
            boneMotions[boneIndex].insert(
                    head, mmd::Motion(boneIndex, 0, Eigen::Vector3f(0, 0, 0), Eigen::Quaternionf(1, 0, 0, 0)));
        }
    }

    for (unsigned int i = 0; i < boneMotions.size(); ++i) {
        motionStreams.push_back(mmd::MotionStream(boneMotions[i]));
    }
}