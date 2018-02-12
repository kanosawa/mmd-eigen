#include <Eigen/Dense>
#include "ModelUpdater.h"

using namespace std;
using namespace mmd;

ModelUpdater::ModelUpdater() {
}


ModelUpdater::~ModelUpdater() {

}


void ModelUpdater::update()
{
    ++currentFrameNo_;
    cout << currentFrameNo_ << endl;

    // frameMotion
    vector<mmd::Motion> frameMotions(model_.getBones().size());
    for (unsigned int i = 0; i < frameMotions.size(); ++i) {
        if (motionStreams_[i].getLatestMotion().getFrameNo() == currentFrameNo_) {
            frameMotions[i] = motionStreams_[i].getLatestMotion();
        } else if (!motionStreams_[i].isLastMotion() && motionStreams_[i].getNextMotion().getFrameNo() == currentFrameNo_) {
            frameMotions[i] = motionStreams_[i].getNextMotion();
            motionStreams_[i].incrementPointer();
        } else if (motionStreams_[i].getLatestMotion().getFrameNo() < currentFrameNo_ && !motionStreams_[i].isLastMotion()) {
            float ratio = float(currentFrameNo_ - motionStreams_[i].getLatestMotion().getFrameNo()) /
                          ((motionStreams_[i].getNextMotion().getFrameNo() -
                            motionStreams_[i].getLatestMotion().getFrameNo()));
            Eigen::Vector3f shift = motionStreams_[i].getLatestMotion().getShift() * (1.0f - ratio) +
                                    motionStreams_[i].getNextMotion().getShift() * ratio;
            Eigen::Quaternionf quaternion = motionStreams_[i].getLatestMotion().getQuaternion().slerp(
                    ratio, motionStreams_[i].getNextMotion().getQuaternion());
            frameMotions[i] = mmd::Motion(i, currentFrameNo_, shift, quaternion);
        } else if (motionStreams_[i].getLatestMotion().getFrameNo() < currentFrameNo_ && motionStreams_[i].isLastMotion()) {
            frameMotions[i] = motionStreams_[i].getLatestMotion();
        } else {
            cout << "error\n";
        }
    }

    // Bone
    model_.setBoneTemporalPosition(superParentIndex_,
                                   model_.getBones()[superParentIndex_].getInitialPosition() + frameMotions[superParentIndex_].getShift());
    model_.setBoneTemporalQuaternion(superParentIndex_, frameMotions[superParentIndex_].getQuaternion());
    moveChildBones(superParentIndex_, frameMotions);

    // IK
    for (unsigned int b = 0; b < model_.getBones().size(); ++b) {
        if (model_.getBones()[b].getIKFlag()) {

            int targetIndex = model_.getBones()[b].getIK().targetIndex;
            for (int i = 0; i < model_.getBones()[b].getIK().loopNum; ++i) {
                for (int l = 0; l < model_.getBones()[b].getIK().linkIndices.size(); ++l) {

                    Bone::IK ik = model_.getBones()[b].getIK();

                    int linkIndex = model_.getBones()[b].getIK().linkIndices[l];

                    Eigen::Vector3f ikPos = model_.getBones()[b].getTemporalPosition();
                    Eigen::Vector3f targetPos = model_.getBones()[targetIndex].getTemporalPosition();
                    Eigen::Vector3f linkPos = model_.getBones()[linkIndex].getTemporalPosition();

                    Eigen::Vector3f targetVec = targetPos - linkPos;
                    Eigen::Vector3f ikVec = ikPos - linkPos;

                    targetVec.normalize();
                    ikVec.normalize();
                    float angle = acos(targetVec.dot(ikVec));
                    Eigen::Vector3f cross = targetVec.cross(ikVec);
                    cross.normalize();
                    Eigen::AngleAxisf mat(angle, cross);

                    frameMotions[linkIndex].setQuaternion(
                            mat * frameMotions[linkIndex].getQuaternion());

                    model_.setBoneTemporalPosition(superParentIndex_,
                                                   model_.getBones()[superParentIndex_].getInitialPosition() + frameMotions[superParentIndex_].getShift());
                    model_.setBoneTemporalQuaternion(superParentIndex_, frameMotions[superParentIndex_].getQuaternion());
                    moveChildBones(superParentIndex_, frameMotions);
                }
            }
        }
    }

    // Vertex
    for (unsigned int i = 0; i < model_.getVertices().size(); ++i) {
        vector<int> refBoneIndices = model_.getVertices()[i].getRefBoneIndices();
        vector<float> refBoneWeightList = model_.getVertices()[i].getRefBoneWeightList();
        Eigen::Vector3f pos(0, 0, 0);
        for (unsigned int b = 0; b < refBoneIndices.size(); ++b) {

            int boneIndex = refBoneIndices[b];

            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (model_.getBones()[boneIndex].getTemporalQuaternion() *
                    (model_.getVertices()[i].getInitialPosition() - model_.getBones()[boneIndex].getInitialPosition()) +
                    model_.getBones()[boneIndex].getTemporalPosition()) *
                   refBoneWeightList[b];
        }
        model_.setVertexTemporalPosition(i, pos);
    }
}


void ModelUpdater::setParam(const PmxModel& model, const vector<MotionStream>& motionStreams) {
    model_ = model;
    motionStreams_ = motionStreams;
    superParentIndex_ = searchSuperParentBone(model_.getBones());
}


const PmxModel& ModelUpdater::getModel() const {
    return model_;
}


//void ModelUpdater::moveChildBones(vector<mmd::Bone> &bones, const int parentBoneIndex,
void ModelUpdater::moveChildBones(const int parentBoneIndex,
                              const vector<mmd::Motion> &frameMotions) {

    //vector<int> childBoneIndices = bones[parentBoneIndex].getChildBoneIndices();
    vector<int> childBoneIndices = model_.getBones()[parentBoneIndex].getChildBoneIndices();
    for (unsigned int i = 0; i < childBoneIndices.size(); ++i) {
        int childBoneIndex = childBoneIndices[i];

        // 移動後の位置 = 親の回転 * (元の位置 - 親の元の位置 + シフト) + 親の移動後の位置
        // 自分自身の回転では位置は変わらない。親の回転によって自分の位置が変わる
        // シフトしてから回転する
        // 親の移動後の位置を原点として回転から、ワールド座標に変換（親の移動後の位置分だけシフト）
        model_.setBoneTemporalPosition(childBoneIndex, model_.getBones()[parentBoneIndex].getTemporalQuaternion() *
                (model_.getBones()[childBoneIndex].getInitialPosition() - model_.getBones()[parentBoneIndex].getInitialPosition() +
                frameMotions[childBoneIndex].getShift()) +
                model_.getBones()[parentBoneIndex].getTemporalPosition());

        // 移動後の回転 = 親の回転 * 回転
        // 親が回転すると、その子供は全て回転する(VMDに書かれているのは親との相対的な回転）
        /*
        bones[childBoneIndex].setTemporalQuaternion(
                bones[parentBoneIndex].getTemporalQuaternion() * frameMotions[childBoneIndex].getQuaternion());
                */
        model_.setBoneTemporalQuaternion(childBoneIndex,
            model_.getBones()[parentBoneIndex].getTemporalQuaternion() * frameMotions[childBoneIndex].getQuaternion());

        // 子ボーンを新たな親ボーンとして再帰的に全ボーンの位置姿勢を算出する
        //moveChildBones(bones, childBoneIndex, frameMotions);
        moveChildBones(childBoneIndex, frameMotions);
    }
}


int ModelUpdater::searchSuperParentBone(const vector<mmd::Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};