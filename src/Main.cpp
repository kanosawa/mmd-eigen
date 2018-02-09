#include <iostream>
#include "PmxFileReader.h"
#include "VmdFileReader.h"
#include "MotionStream.h"
#include "Renderer.h"

using namespace std;

int main(int argc, char *argv[]) {
    // PMXファイルの入力
    mmd::PmxFileReader pmxFileReader("/home/kanosawa/src/mmd-eigen/data/maya.pmx");
    mmd::PmxModel model;
    pmxFileReader.readFile(model);

    // VMDファイルの入力
    vector<mmd::Motion> motions;
    mmd::VmdFileReader vmdFileReader("/home/kanosawa/src/mmd-eigen/data/maya.vmd", model.getBones());
    vmdFileReader.readFile(motions);

    // MotionをBoneごとに分ける
    vector<vector<mmd::Motion>> boneMotions(model.getBones().size());
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

    vector<mmd::MotionStream> motionStreams;
    for (unsigned int i = 0; i < boneMotions.size(); ++i) {
        motionStreams.push_back(mmd::MotionStream(boneMotions[i]));
    }

    mmd::Renderer::getInstance().setParam(model, motionStreams);
    mmd::Renderer::getInstance().start();

    return 0;
}
