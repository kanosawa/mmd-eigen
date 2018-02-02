#include <iostream>
#include <Eigen/Dense>
#include "VmdDataStream.h"

using namespace std;
using namespace mmd;

VmdDataStream::VmdDataStream() {
}


VmdDataStream::~VmdDataStream() {
}


void VmdDataStream::moveChildBones(vector<Bone> &bones, const int parentBoneIndex,
                                   const vector<Motion> &frameMotions) {

    vector<int> childBoneIndices = bones[parentBoneIndex].getChildBoneIndices();
    for (unsigned int i = 0; i < childBoneIndices.size(); ++i) {
        int childBoneIndex = childBoneIndices[i];

        // 移動後の位置 = 親の回転 * (元の位置 - 親の元の位置 + シフト) + 親の移動後の位置
        // 自分自身の回転では位置は変わらない。親の回転によって自分の位置が変わる
        // シフトしてから回転する
        // 親の移動後の位置を原点として回転から、ワールド座標に変換（親の移動後の位置分だけシフト）
        bones[childBoneIndex].setTemporalPosition(bones[parentBoneIndex].getTemporalQuaternion() *
                                              (bones[childBoneIndex].getInitialPosition() - bones[parentBoneIndex].getInitialPosition() +
                                               frameMotions[childBoneIndex].getShift()) +
                                              bones[parentBoneIndex].getTemporalPosition());

        // 移動後の回転 = 親の回転 * 回転
        // 親が回転すると、その子供は全て回転する(VMDに書かれているのは親との相対的な回転）
        bones[childBoneIndex].setTemporalQuaternion(
                bones[parentBoneIndex].getTemporalQuaternion() * frameMotions[childBoneIndex].getQuaternion());

        // 子ボーンを新たな親ボーンとして再帰的に全ボーンの位置姿勢を算出する
        moveChildBones(bones, childBoneIndex, frameMotions);
    }
}

bool VmdDataStream::calcStream(BoneStream &boneStream, VertexStream &vertexStream, const vector<Bone> &initialBones,
                               const vector<Vertex> &initialVertices, const vector<Motion>& motions) {
    // ボーン情報リストをフレームごとに分割
    vector<pair<int, vector<Motion>>> allFrameMotions;
    splitBoneInfoList(allFrameMotions, motions);

    // 全ての親ボーンの探索
    int superParentIndex = searchSuperParentBone(initialBones);
    if (superParentIndex == -1) {
        cout << "全ての親ボーンが見つかりません\n";
        return false;
    }

    // 全フレームについて
    vector<Motion> frameMotions_tmp(initialBones.size());
    for (unsigned int f = 0; f < allFrameMotions.size(); ++f) {

        int frameNo = allFrameMotions[f].first;
        vector<Motion> boneInfoList = allFrameMotions[f].second;
        std::sort(boneInfoList.begin(), boneInfoList.end(), [](const Motion& a, const Motion& b){
            return a.getBoneIndex() < b.getBoneIndex();
        });

        // 全ボーンに関するボーン情報を作る
        vector<Motion> frameMotions(initialBones.size());

        if (f == 0) {
            // 移動なしで初期化して
            for (unsigned int b = 0; b < frameMotions.size(); ++b) {
                frameMotions[b] = Motion(b, 0, Eigen::Vector3f(0, 0, 0), Eigen::Quaternionf(1, 0, 0, 0));
            }

            // VMDファイルに記載のあったボーン情報を上書き
            for (unsigned int i = 0; i < boneInfoList.size(); ++i) {
                frameMotions[boneInfoList[i].getBoneIndex()] = boneInfoList[i];
            }
        } else {
            int b = 0;
            for (unsigned int i = 0; i < boneInfoList.size(); ++i) {
                // VMDファイルに記載がない場合は前フレームのボーン情報を書き込む
                while(b < boneInfoList[i].getBoneIndex()) {
                    frameMotions[b] = frameMotions_tmp[b];
                    ++b;
                }
                // VMDファイルに記載がある場合は、記載されたボーン情報を書き込む
                frameMotions[b] = boneInfoList[i];
                ++b;
            }
            while(b < static_cast<int>(frameMotions.size())) {
                frameMotions[b] = frameMotions_tmp[b];
                ++b;
            }
        }

        frameMotions_tmp = frameMotions;

        vector<Bone> bones = initialBones;

        // ボーンストリーム
        calcBoneStream(boneStream, bones, frameMotions, frameNo, superParentIndex);

        // 頂点ストリーム
        calcVertexStream(vertexStream, initialVertices, bones, frameNo);
    }

    return true;
}


void VmdDataStream::calcBoneStream(BoneStream &boneStream, vector<Bone> &bones,
                                   const vector<Motion> &frameMotions, const int frameNo,
                                   const int superParentIndex) {
    // 全ての親ボーンについては単純に姿勢を変換
    bones[superParentIndex].setTemporalPosition(
            bones[superParentIndex].getInitialPosition() + frameMotions[superParentIndex].getShift());
    bones[superParentIndex].setTemporalQuaternion(frameMotions[superParentIndex].getQuaternion());

    moveChildBones(bones, superParentIndex, frameMotions);

    boneStream.pushBackBones(frameNo, bones);
}


void VmdDataStream::calcVertexStream(VertexStream &vertexStream,
                                     const vector<Vertex> &initialVertices, const vector<Bone> &bones,
                                     const int frameNo) {
    vector<Vertex> vertices = initialVertices;
    for (unsigned int i = 0; i < initialVertices.size(); ++i) {
        vector<int> refBoneIndices = initialVertices[i].getRefBoneIndices();
        vector<float> refBoneWeightList = initialVertices[i].getRefBoneWeightList();
        Eigen::Vector3f pos(0, 0, 0);
        for (unsigned int b = 0; b < refBoneIndices.size(); ++b) {

            int boneIndex = refBoneIndices[b];

            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (bones[boneIndex].getTemporalQuaternion() *
                    (vertices[i].getPosition() - bones[boneIndex].getInitialPosition()) +
                    bones[boneIndex].getTemporalPosition()) *
                   refBoneWeightList[b];
        }
        // refBoneIndices.size()で割る必要があるのでは？
        vertices[i].setPosition(pos);
    }
    vertexStream.pushBackVertices(frameNo, vertices);
}


void VmdDataStream::splitBoneInfoList(vector<pair<int, vector<Motion>>> &boneInfoListVec, const vector<Motion>& motions_in) {

    // フレーム番号でmotionsをソート
    vector<Motion> motions = motions_in;
    std::sort(motions.begin(), motions.end(), [](const Motion& a, const Motion& b){
        return a.getFrameNo() < b.getFrameNo();
    });

    // フレーム番号ごとにmotionsを分割
    int frameNo = 0;
    auto motion = motions.begin();
    while (1) {
        vector<Motion> frameMotions;
        while (motion != motions.end() && motion->getFrameNo() == frameNo) {
            frameMotions.push_back(*motion);
            ++motion;
        }
        boneInfoListVec.push_back(pair<int, vector<Motion>>(frameNo, frameMotions));
        if (motion == motions.end()) break;
        frameNo = motion->getFrameNo();
    }
}


int VmdDataStream::searchSuperParentBone(const vector<Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};