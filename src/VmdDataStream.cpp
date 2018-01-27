#include <iostream>
#include <Eigen/Dense>
#include "VmdDataStream.h"

using namespace std;
using namespace mmd;

int searchSuperParentBone(const vector<Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};


VmdDataStream::VmdDataStream() {
}


VmdDataStream::~VmdDataStream() {
}


void VmdDataStream::insertBoneInfoList(const int frameNo, const BoneInfo &boneInfo) {
    boneInfoListMap_.insert(map<int, BoneInfo>::value_type(frameNo, boneInfo));
}


const multimap<int, VmdDataStream::BoneInfo> &VmdDataStream::getBoneInfoList() const {
    return boneInfoListMap_;
}


void VmdDataStream::moveChildBones(vector<Bone> &bones, const int parentBoneIndex,
                                   const vector<BoneInfo> &boneInfoList, const Eigen::Vector3f &parentBeforePos) {
    vector<int> childBoneIndices = bones[parentBoneIndex].getChildBoneIndices();
    for (unsigned int i = 0; i < childBoneIndices.size(); ++i) {
        int childBoneIndex = childBoneIndices[i];

        // 子ボーンの移動後位置の算出に親ボーンの元の移動前位置が必要
        Eigen::Vector3f childBeforePos = bones[childBoneIndex].getPosition();

        // 移動後の位置 = 親の回転 * (元の位置 - 親の元の位置 + シフト) + 親の移動後の位置
        // 自分自身の回転では位置は変わらない。親の回転によって自分の位置が変わる
        // シフトしてから回転する
        // 親の移動後の位置を原点として回転から、ワールド座標に変換（親の移動後の位置分だけシフト）
        bones[childBoneIndex].setPosition(bones[parentBoneIndex].getQuaternion() *
                                          (bones[childBoneIndex].getPosition() - parentBeforePos +
                                           boneInfoList[childBoneIndex].shift) +
                                          bones[parentBoneIndex].getPosition());

        // 移動後の回転 = 親の回転 * 回転
        // 親が回転すると、その子供は全て回転する(VMDに書かれているのは親との相対的な回転）
        bones[childBoneIndex].setQuartanion(
                bones[parentBoneIndex].getQuaternion() * boneInfoList[childBoneIndex].quarternion);

        // 子ボーンを新たな親ボーンとして再帰的に全ボーンの位置姿勢を算出する
        moveChildBones(bones, childBoneIndex, boneInfoList, childBeforePos);
    }
}

bool VmdDataStream::calcStream(BoneStream &boneStream, VertexStream &vertexStream, const vector<Bone> &initialBones,
                               const vector<Vertex> &initialVertices) {
    // ボーン情報リストをフレームごとに分割
    vector<pair<int, vector<BoneInfo>>> boneInfoListVec;
    splitBoneInfoList(boneInfoListVec);

    // 全ての親ボーンの探索
    int superParentIndex = searchSuperParentBone(initialBones);
    if (superParentIndex == -1) {
        cout << "全ての親ボーンが見つかりません\n";
        return false;
    }

    // 全フレームについて
    vector<Bone> currentBones = initialBones;
    for (unsigned int f = 0; f < boneInfoListVec.size(); ++f) {

        int frameNo = boneInfoListVec[f].first;
        vector<BoneInfo> boneInfoList = boneInfoListVec[f].second;

        // 全ボーンに関するボーン情報を作る
        vector<BoneInfo> allBoneInfoList(initialBones.size());
        for (unsigned int b = 0; b < allBoneInfoList.size(); ++b) {
            allBoneInfoList[b].boneIndex = b;
            allBoneInfoList[b].shift = Eigen::Vector3f(0, 0, 0);
            allBoneInfoList[b].quarternion = Eigen::Quaternionf(1, 0, 0, 0);
        }

        // VMDファイルに記載のあったボーンについては、記載されていたボーン情報を上書き
        // VMDファイルに記載されていないボーンはシフトと回転が無いということなので初期値のまま
        for (unsigned int i = 0; i < boneInfoList.size(); ++i) {
            allBoneInfoList[boneInfoList[i].boneIndex] = boneInfoList[i];
        }

        // ボーンストリーム
        // currentBonesはフレームが進むたびに更新されていく
        calcBoneStream(boneStream, currentBones, allBoneInfoList, frameNo, superParentIndex);

        // 頂点ストリーム
        calcVertexStream(vertexStream, boneStream, initialVertices, initialBones, frameNo);
    }

    return true;
}


void VmdDataStream::calcBoneStream(BoneStream &boneStream, vector<Bone> &bones,
                                   const vector<BoneInfo> &boneInfoList, const int frameNo, const int superParentIndex) {
    // 子ボーンの移動後位置の算出に親ボーンの元の移動前位置が必要
    Eigen::Vector3f parentBeforePos = bones[superParentIndex].getPosition();

    // 全ての親ボーンについては単純に姿勢を変換
    bones[superParentIndex].setPosition(bones[superParentIndex].getPosition() + boneInfoList[superParentIndex].shift);
    bones[superParentIndex].setQuartanion(
            bones[superParentIndex].getQuaternion() * boneInfoList[superParentIndex].quarternion);

    moveChildBones(bones, superParentIndex, boneInfoList, parentBeforePos);

    /*
	//for (int i = 0; i < boneInfoList.size(); ++i) {
	for (int i = 0; i < 3; ++i) {
		int boneIndex = boneInfoList[i].boneIndex;

		// �q�{�[���̈ړ���ʒu�̎Z�o�ɐe�{�[���̌��̈ړ��O�ʒu���K�v
		Eigen::Vector3f parentBeforePos = bones[boneIndex].getPosition();

		// �ŏ��̃{�[���ɂ��Ă͒P���ȕ��s�ړ��̂�
		bones[boneIndex].setPosition(bones[boneIndex].getPosition() + boneInfoList[i].shift);
		bones[boneIndex].setQuartanion(bones[boneIndex].getQuaternion() * boneInfoList[i].quarternion);
		
		moveChildBones(bones, boneIndex, boneInfoList[i], parentBeforePos);
	}
    */

    boneStream.pushBackBones(frameNo, bones);
}


void VmdDataStream::calcVertexStream(VertexStream &vertexStream, const BoneStream &boneStream,
                                     const vector<Vertex> &initialVertices, const vector<Bone> &initialBones,
                                     const int frameNo) {
    vector<Bone> afterBones;
    boneStream.getBones(afterBones, frameNo);
    vector<Vertex> vertices = initialVertices;
    for (unsigned int i = 0; i < initialVertices.size(); ++i) {
        vector<int> refBoneIndices = initialVertices[i].getRefBoneIndices();
        vector<float> refBoneWeightList = initialVertices[i].getRefBoneWeightList();
        Eigen::Vector3f pos(0, 0, 0);
        for (unsigned int b = 0; b < refBoneIndices.size(); ++b) {

            int boneIndex = refBoneIndices[b];

            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (afterBones[boneIndex].getQuaternion() *
                    (vertices[i].getPosition() - initialBones[boneIndex].getPosition()) +
                    afterBones[boneIndex].getPosition()) *
                   refBoneWeightList[b];
        }
        // refBoneIndices.size()で割る必要があるのでは？
        vertices[i].setPosition(pos);
    }
    vertexStream.pushBackVertices(frameNo, vertices);
}


void VmdDataStream::splitBoneInfoList(vector<pair<int, vector<BoneInfo>>> &boneInfoListVec) {
    int beforeFrameNo = 0;
    auto b = boneInfoListMap_.begin();
    while (1) {
        vector<BoneInfo> boneInfoList;
        while (b != boneInfoListMap_.end() && b->first == beforeFrameNo) {
            boneInfoList.push_back(b->second);
            ++b;
        }
        boneInfoListVec.push_back(pair<int, vector<BoneInfo>>(beforeFrameNo, boneInfoList));
        if (b == boneInfoListMap_.end()) break;
        beforeFrameNo = b->first;
    }
}