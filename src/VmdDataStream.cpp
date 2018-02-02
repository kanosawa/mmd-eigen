#include <iostream>
#include <Eigen/Dense>
#include "VmdDataStream.h"

using namespace std;
using namespace mmd;

VmdDataStream::VmdDataStream() {
}


VmdDataStream::~VmdDataStream() {
}


void VmdDataStream::insertBoneInfoList(const int frameNo, const Motion &motion) {
    boneInfoListMap_.insert(map<int, Motion>::value_type(frameNo, motion));
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
                               const vector<Vertex> &initialVertices) {
    // ボーン情報リストをフレームごとに分割
    vector<pair<int, vector<Motion>>> boneInfoListVec;
    splitBoneInfoList(boneInfoListVec);

    // 全ての親ボーンの探索
    int superParentIndex = searchSuperParentBone(initialBones);
    if (superParentIndex == -1) {
        cout << "全ての親ボーンが見つかりません\n";
        return false;
    }

    // 全フレームについて
    vector<Motion> beforeAllBoneInfoList(initialBones.size());
    for (unsigned int f = 0; f < boneInfoListVec.size(); ++f) {

        int frameNo = boneInfoListVec[f].first;
        vector<Motion> boneInfoList = boneInfoListVec[f].second;
        sort(boneInfoList.begin(), boneInfoList.end());

        // 全ボーンに関するボーン情報を作る
        vector<Motion> allBoneInfoList(initialBones.size());

        if (f == 0) {
            // 移動なしで初期化して
            for (unsigned int b = 0; b < allBoneInfoList.size(); ++b) {
                allBoneInfoList[b] = Motion(b, 0, Eigen::Vector3f(0, 0, 0), Eigen::Quaternionf(1, 0, 0, 0));
            }

            // VMDファイルに記載のあったボーン情報を上書き
            for (unsigned int i = 0; i < boneInfoList.size(); ++i) {
                allBoneInfoList[boneInfoList[i].getBoneIndex()] = boneInfoList[i];
            }
        } else {
            int b = 0;
            for (unsigned int i = 0; i < boneInfoList.size(); ++i) {
                // VMDファイルに記載がない場合は前フレームのボーン情報を書き込む
                while(b < boneInfoList[i].getBoneIndex()) {
                    allBoneInfoList[b] = beforeAllBoneInfoList[b];
                    ++b;
                }
                // VMDファイルに記載がある場合は、記載されたボーン情報を書き込む
                allBoneInfoList[b] = boneInfoList[i];
                ++b;
            }
            while(b < static_cast<int>(allBoneInfoList.size())) {
                allBoneInfoList[b] = beforeAllBoneInfoList[b];
                ++b;
            }
        }

        beforeAllBoneInfoList = allBoneInfoList;

        vector<Bone> bones = initialBones;

        // ボーンストリーム
        calcBoneStream(boneStream, bones, allBoneInfoList, frameNo, superParentIndex);

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


void VmdDataStream::splitBoneInfoList(vector<pair<int, vector<Motion>>> &boneInfoListVec) {
    int beforeFrameNo = 0;
    auto b = boneInfoListMap_.begin();
    while (1) {
        //vector<BoneInfo> boneInfoList;
        vector<Motion> boneInfoList;
        while (b != boneInfoListMap_.end() && b->first == beforeFrameNo) {
            boneInfoList.push_back(b->second);
            ++b;
        }
        boneInfoListVec.push_back(pair<int, vector<Motion>>(beforeFrameNo, boneInfoList));
        if (b == boneInfoListMap_.end()) break;
        beforeFrameNo = b->first;
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