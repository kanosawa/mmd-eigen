#include <iostream>
#include "PmxFileReader.h"
#include "VmdFileReader.h"
#include "MotionStreamGenerator.h"
#include "Renderer.h"

using namespace std;

int main(int argc, char *argv[]) {
    // PMXファイルの入力
    mmd::PmxFileReader pmxFileReader("/home/kanosawa/src/mmd-eigen/data/maya/maya.pmx");
    mmd::PmxModel model;
    pmxFileReader.readFile(model);

    // VMDファイルの入力
    vector<mmd::Motion> motions;
    mmd::VmdFileReader vmdFileReader("/home/kanosawa/src/mmd-eigen/data/fish.vmd", model.getBones());
    vmdFileReader.readFile(motions);

    // VMDのモーションデータをボーンごとのモーションストリームに変換
    mmd::MotionStreamGenerator motionStreamGenerator;
    vector<mmd::MotionStream> motionStreams;
    motionStreamGenerator.generate(motionStreams, motions, model.getBones());

    mmd::Renderer::getInstance().setParam(model, motionStreams);
    mmd::Renderer::getInstance().start();

    return 0;
}
