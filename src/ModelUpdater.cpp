#include <Eigen/Dense>
#include "ModelUpdater.h"

using namespace std;
using namespace mmd;

ModelUpdater::ModelUpdater() {
}


ModelUpdater::~ModelUpdater() {
}


const PmxModel& ModelUpdater::getModel() const {
    return model_;
}


void ModelUpdater::update()
{
    ++currentFrameNo_;
    cout << currentFrameNo_ << endl;

    // 現フレームのモーションデータを補間処理によって生成
    vector<mmd::Motion> frameMotions(model_.getBones().size());
    for (unsigned int i = 0; i < frameMotions.size(); ++i) {
        // ちょうどそのフレームのモーションデータがある場合は、それをそのまま使う
        // 既にモーションデータの末尾まで到達してる場合も、その末尾のデータをそのまま使う
        if (motionStreams_[i].getLatestMotion().getFrameNo() == currentFrameNo_ ||
            motionStreams_[i].isLastMotion()) {
            frameMotions[i] = motionStreams_[i].getLatestMotion();
        } // モーションストリームの次のデータが現フレームのデータの場合は、次のデータを使い、ポインタも進める
        else if (motionStreams_[i].getNextMotion().getFrameNo() == currentFrameNo_) {
            frameMotions[i] = motionStreams_[i].getNextMotion();
            motionStreams_[i].incrementPointer();
        } // それ以外の場合は補間
        else {
            float ratio = float(currentFrameNo_ - motionStreams_[i].getLatestMotion().getFrameNo()) /
                          ((motionStreams_[i].getNextMotion().getFrameNo() -
                            motionStreams_[i].getLatestMotion().getFrameNo()));
            Eigen::Vector3f shift = motionStreams_[i].getLatestMotion().getShift() * (1.0f - ratio) +
                                    motionStreams_[i].getNextMotion().getShift() * ratio;
            Eigen::Quaternionf quaternion = motionStreams_[i].getLatestMotion().getQuaternion().slerp(
                    ratio, motionStreams_[i].getNextMotion().getQuaternion());
            frameMotions[i] = mmd::Motion(i, currentFrameNo_, shift, quaternion);
        }
    }


    // 付与親
    for (int i = 0; i < model_.getBones().size(); ++i) {
        Bone bone = model_.getBones()[i];
        if (bone.getAssignRotFlag()) {
            if (bone.getAssignRatio() >= 0) {
                frameMotions[i].setQuaternion(frameMotions[i].getQuaternion().slerp(
                        bone.getAssignRatio(), frameMotions[bone.getAssignParentIndex()].getQuaternion() *
                                frameMotions[i].getQuaternion()));
            } else {
                frameMotions[i].setQuaternion(frameMotions[i].getQuaternion().slerp(
                        -bone.getAssignRatio(), frameMotions[bone.getAssignParentIndex()].getQuaternion().inverse() *
                                frameMotions[i].getQuaternion()));
            }
        }
        if (bone.getAssignShiftFlag()) {
            frameMotions[i].setShift(frameMotions[i].getShift() +
                                     frameMotions[bone.getAssignParentIndex()].getShift() *
                                     bone.getAssignRatio());
        }
    }


    // ボーン更新
    model_.setBoneTemporalPosition(superParentIndex_,
                                   model_.getBones()[superParentIndex_].getInitialPosition() + frameMotions[superParentIndex_].getShift());
    model_.setBoneTemporalQuaternion(superParentIndex_, frameMotions[superParentIndex_].getQuaternion());
    moveChildBones(superParentIndex_, frameMotions);

    // IK
    for (auto bone = model_.getBones().begin(); bone != model_.getBones().end(); ++bone) {
        if (bone->getIKFlag()) {

            Bone::IK ik = bone->getIK();
            //for (int i = 0; i < ik.loopNum; ++i) {
            int loopNum = 2;
            if (ik.loopNum < loopNum) loopNum = ik.loopNum;
            for (int i = 0; i < loopNum; ++i) {
                for (int l = 0; l < ik.linkIndices.size(); ++l) {

                    int linkIndex = ik.linkIndices[l];

                    Eigen::Vector3f ikPos = bone->getTemporalPosition();
                    Eigen::Vector3f targetPos = model_.getBones()[ik.targetIndex].getTemporalPosition();
                    Eigen::Vector3f linkPos = model_.getBones()[linkIndex].getTemporalPosition();

                    Eigen::AngleAxisf mat;

                    if (ik.angleLimitFlags[l]) {
                        if (i != 0) continue;

                        Eigen::Vector3f targetVec = targetPos - linkPos;
                        int baseIndex = *(ik.linkIndices.end() - 1);
                        Eigen::Vector3f basePos = model_.getBones()[baseIndex].getTemporalPosition();
                        Eigen::Vector3f baseVec = linkPos - basePos;
                        Eigen::Vector3f ikVec = ikPos - basePos;

                        // 角度算出
                        float cos_value = (ikVec.norm() * ikVec.norm() - baseVec.norm() * baseVec.norm() -
                                           targetVec.norm() * targetVec.norm()) / (2.0f * baseVec.norm() * targetVec.norm());
                        if (cos_value < -1.0f) cos_value = -1.0f;
                        if (cos_value > 1.0f) cos_value = 1.0f;
                        float angle = acos(cos_value);

                        if (angle < 0) angle = 0;

                        if (angle > ik.unitAngle) {
                            angle = ik.unitAngle;
                        }

                        // 回転
                        Eigen::Vector3f axis(-1, 0, 0);
                        //Eigen::AngleAxisf mat(angle, axis);
                        mat = Eigen::AngleAxisf(angle, axis);
                        frameMotions[linkIndex].setQuaternion(mat * frameMotions[linkIndex].getQuaternion());
                    } else {
                        // リンクボーンの位置姿勢を座標系としたベクトルを算出
                        // ボーンの回転は親ボーンからの相対的なものなので、リンクボーンを基準にしないといけない
                        Eigen::Vector3f ikVec = model_.getBones()[linkIndex].getTemporalQuaternion().inverse()
                                                      * (ikPos - linkPos);
                        Eigen::Vector3f targetVec = model_.getBones()[linkIndex].getTemporalQuaternion().inverse()
                                                          * (targetPos - linkPos);
                        targetVec.normalize();
                        ikVec.normalize();

                        // 角度算出
                        float cos_value = targetVec.dot(ikVec);
                        if (cos_value < -1.0f) cos_value = -1.0f;
                        if (cos_value > 1.0f) cos_value = 1.0f;
                        float angle = acos(cos_value);

                        if (angle < 0) angle = 0;
                        if (angle > ik.unitAngle) {
                            angle = ik.unitAngle;
                        }

                        // 回転
                        Eigen::Vector3f cross = targetVec.cross(ikVec);
                        cross.normalize();
                        //Eigen::AngleAxisf mat(angle, cross);
                        mat = Eigen::AngleAxisf(angle, cross);
                        frameMotions[linkIndex].setQuaternion(mat * frameMotions[linkIndex].getQuaternion());
                    }

                    // 付与
                    for (int i = 0; i < model_.getBones().size(); ++i) {
                        Bone bone = model_.getBones()[i];
                        if (bone.getAssignParentIndex() == linkIndex && bone.getAssignRotFlag()) {
                            if (bone.getAssignRatio() >= 0) {
                                frameMotions[i].setQuaternion(frameMotions[i].getQuaternion().slerp(
                                        bone.getAssignRatio(), mat * frameMotions[i].getQuaternion()));
                            } else {
                                frameMotions[i].setQuaternion(frameMotions[i].getQuaternion().slerp(
                                     -bone.getAssignRatio(), mat.inverse() * frameMotions[i].getQuaternion()));
                            }
                        }
                    }

                    // ボーン更新
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

            auto bone = model_.getBones()[refBoneIndices[b]];
            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (bone.getTemporalQuaternion() *
                    (model_.getVertices()[i].getInitialPosition() - bone.getInitialPosition()) +
                    bone.getTemporalPosition()) * refBoneWeightList[b];
        }
        model_.setVertexTemporalPosition(i, pos);
    }
}


void ModelUpdater::setParam(const PmxModel& model, const vector<MotionStream>& motionStreams) {
    model_ = model;
    motionStreams_ = motionStreams;
    superParentIndex_ = searchSuperParentBone(model_.getBones());
}


int ModelUpdater::searchSuperParentBone(const vector<mmd::Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};


void ModelUpdater::moveChildBones(const int parentBoneIndex,
                                  const vector<mmd::Motion> &frameMotions) {
    auto parentBone = model_.getBones()[parentBoneIndex];

    vector<int> childBoneIndices = parentBone.getChildBoneIndices();
    for (unsigned int i = 0; i < childBoneIndices.size(); ++i) {
        int childBoneIndex = childBoneIndices[i];
        auto childBone = model_.getBones()[childBoneIndex];

        // 移動後の位置 = 親の回転 * (元の位置 - 親の元の位置 + シフト) + 親の移動後の位置
        // 自分自身の回転では位置は変わらない。親の回転によって自分の位置が変わる
        model_.setBoneTemporalPosition(childBoneIndex, parentBone.getTemporalQuaternion() *
                                                       (childBone.getInitialPosition() -
                                                        parentBone.getInitialPosition() +
                                                        frameMotions[childBoneIndex].getShift()) +
                                                       parentBone.getTemporalPosition());

        // 移動後の回転 = 親の回転 * 回転
        // 親が回転すると、その子供は全て回転する(VMDに書かれているのは親との相対的な回転）
        model_.setBoneTemporalQuaternion(childBoneIndex,
                                         parentBone.getTemporalQuaternion() *
                                         frameMotions[childBoneIndex].getQuaternion());

        // 子ボーンを新たな親ボーンとして再帰的に全ボーンの位置姿勢を算出する
        moveChildBones(childBoneIndex, frameMotions);
    }
}
