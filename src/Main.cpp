#include <iostream>
#include "ModelFileReaderFactory.h"
#include "VmdFileReader.h"
#include "MotionStreamGenerator.h"
#include "Renderer.h"

using namespace std;

int main(int argc, char *argv[]) {

    if (argc < 3) {
        cout << "usage : mmd-eigen <model_filepath> <vmd_filepath>\n";
        exit(0);
    }

    // モデルファイル読み込みクラスの生成
    mmd::ModelFileReaderFactory factory;
    unique_ptr<mmd::ModelFileReader> modelFileReader =
            factory.create(argv[1]);

    // モデルファイルの読み込み
    mmd::PmxModel model;
    modelFileReader->readFile(model);

    // VMDファイルの入力
    vector<mmd::Motion> motions;
    mmd::VmdFileReader vmdFileReader(argv[2], model.getBones());
    vmdFileReader.readFile(motions);

    // VMDのモーションデータをボーンごとのモーションストリームに変換
    mmd::MotionStreamGenerator motionStreamGenerator;
    vector<mmd::MotionStream> motionStreams;
    motionStreamGenerator.generate(motionStreams, motions, model.getBones());

    // レンダリング
    mmd::Renderer& renderer = mmd::Renderer::getInstance();
    renderer.setParam(model, motionStreams);
    renderer.start();

    return 0;
}
