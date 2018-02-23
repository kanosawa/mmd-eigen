#include "PmdModelFileReader.h"

using namespace std;
using namespace mmd;


PmdModelFileReader::PmdModelFileReader(const string &filename)
        : ModelFileReader(filename) {
}


PmdModelFileReader::~PmdModelFileReader() {
}


void PmdModelFileReader::readFile(PmxModel& model) {

    // ヘッダの読み込み
    if (!readHeader(model)) {
        cout << "PmdModelFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // モデル情報の読み込み
    if (!readModelInfo()) {
        cout << "PmdModelFileReader::readFile() : ModelInfo Error\n";
        exit(0);
    }

    // 頂点の読み込み
    if (!readVertices(model)) {
        cout << "PmdModelFileReader::readFile() : Vertices Error\n";
        exit(0);
    }

    // サーフェスの読み込み
    if (!readSurfaces(model)) {
        cout << "PmdModelFileReader::readFile() : Surfaces Error\n";
        exit(0);
    }

    // テクスチャの読み込み
    if (!readMaterials(model)) {
        cout << "PmdModelFileReader::readFile() : Materials Error\n";
        exit(0);
    }

    // ボーンの読み込み
    if (!readBones(model)) {
        cout << "PmdModelFileReader::readFile() : Bones Error\n";
        exit(0);
    }

    // 子ボーンインデックスの算出
    if (!calcChildBoneIndices(model)) {
        cout << "PmxFileReader::readFile() : ChileBoneIndices Error\n";
        exit(0);
    }
}


bool PmdModelFileReader::readHeader(PmxModel &model) {
    // "Pmd"の確認(ASCII)
    const int PMD_LENGTH = 3;
    char charPmd[PMD_LENGTH];

    fileReader_.read(reinterpret_cast<char *>(charPmd), PMD_LENGTH);
    if (strncmp(charPmd, "Pmd", PMD_LENGTH) != 0) {
        return false;
    }

    // バージョンの確認
    float pmdVersion;
    fileReader_.read(reinterpret_cast<char *>(&pmdVersion), 4);
    if (pmdVersion != 1) return false;

    return true;
}


bool PmdModelFileReader::readModelInfo() {
    fileReader_.readFromCP932(20, false); // モデル名
    fileReader_.readFromCP932(256, false); // コメント
    return true;
}


bool PmdModelFileReader::readVertices(PmxModel &model) {
    // 頂点数
    unsigned int vertexNum;
    fileReader_.read(reinterpret_cast<char *>(&vertexNum), 4);

    for (unsigned int n = 0; n < vertexNum; ++n) {

        // 三次元座標
        Eigen::Vector3f pos;
        fileReader_.read(reinterpret_cast<char *>(&pos), 12);

        // 法線
        Eigen::Vector3f normal;
        fileReader_.read(reinterpret_cast<char *>(&normal), 12);

        // UV
        Eigen::Vector2f uv;
        fileReader_.read(reinterpret_cast<char *>(&uv), 8);

        // 参照ボーンインデックスと重み
        vector<int> boneIndices;
        vector<float> boneWeightList;
        for (int i = 0; i < 2; ++i) {
            boneIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
        }
        char weight;
        fileReader_.read(&weight, 1);
        boneWeightList.push_back(float(weight) / 100.0f);
        boneWeightList.push_back(1.0f - float(weight) / 100.0f);
        model.pushBackVertex(Vertex(pos, uv, boneIndices, boneWeightList));

        // edge
        char edge;
        fileReader_.read(&edge, 1);
    }

    return true;
}


bool PmdModelFileReader::readSurfaces(PmxModel &model) {
    int vertexIndexNum; // インデックス数。3インデックスで1面
    fileReader_.read(reinterpret_cast<char *>(&vertexIndexNum), 4);

    for (int n = 0; n < vertexIndexNum / 3; ++n) {
        Eigen::Vector3i vertexIndexies;
        for (int i = 0; i < 3; ++i) {
            vertexIndexies[i] = fileReader_.readVariableSizeUnsignedData(headerInfo_.vertexIndexSize);
        }
        Surface surface(vertexIndexies);
        model.pushBackSurface(surface);
    }
    return true;
}


bool PmdModelFileReader::readMaterials(PmxModel &model) {
    // マテリアル数
    int materialNum;
    fileReader_.read(reinterpret_cast<char *>(&materialNum), 4);

    for (int n = 0; n < materialNum; ++n) {

        unsigned char tmpByte;

        // ディフューズ
        Eigen::Vector4f diffuse;
        fileReader_.read(reinterpret_cast<char *>(&diffuse), 16);

        // スペキュラー係数
        float specularCoef;
        fileReader_.read(reinterpret_cast<char *>(&specularCoef), 4);

        // スペキュラー
        Eigen::Vector3f specular;
        fileReader_.read(reinterpret_cast<char *>(&specular), 12);

        // アンビエント
        Eigen::Vector3f ambient;
        fileReader_.read(reinterpret_cast<char *>(&ambient), 12);

        // toonインデックス
        fileReader_.read(reinterpret_cast<char *>(&tmpByte), 1);

        // エッジフラグ
        fileReader_.read(reinterpret_cast<char *>(&tmpByte), 1);

        // 面数
        int vertexNum;
        fileReader_.read(reinterpret_cast<char *>(&vertexNum), 4);
        if (vertexNum % 3 != 0) {
            std::cout << "vertexNum Error\n";
            return false;
        }

        // テクスチャ画像
        string textureName = fileReader_.readFromCP932(20, true);
        auto pos = textureName.find("*");
        if (pos != string::npos) {
            textureName.resize(pos);
        }
        if (strlen(textureName.c_str()) != 0) {
            cv::Mat textureImage = cv::imread((fileReader_.getFolderName() + textureName).c_str());
            cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);
            model.pushBackTexture(textureImage);
            model.pushBackMaterial(Material(diffuse, specular, specularCoef, ambient, n, vertexNum / 3));
        } else {
            model.pushBackMaterial(Material(diffuse, specular, specularCoef, ambient, -1, vertexNum / 3));
        }
    }

    return true;
}


bool PmdModelFileReader::readBones(PmxModel &model) {

    // ボーン数
    short boneNum;
    fileReader_.read(reinterpret_cast<char *>(&boneNum), 2);
    vector<Bone> bones(boneNum);

    for (int n = 0; n < boneNum; ++n) {
        // ボーン名（日本語）
        string boneName = fileReader_.readFromCP932(20, false);

        // 親ボーンインデックス
        int parentBoneIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);

        // 接続先ボーンインデックス
        int destinationBoneIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);

        // ボーンの種類(0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動)
        char bone_type;
        fileReader_.read(&bone_type, 1);

        // IKターゲットインデックス
        fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);

        // 三次元座標
        Eigen::Vector3f position;
        fileReader_.read(reinterpret_cast<char *>(&position), 12);

        // ボーンの生成
        Bone bone(boneName, position, parentBoneIndex);
        if (destinationBoneIndex != 0) {
            bone.setDestinationFlag(true);
            bone.setDestinationBoneIndex(destinationBoneIndex);
        } else {
            bone.setDestinationFlag(false);
            bone.setOffset(Eigen::Vector3f(0, 0, 0));
        }
        bone.setIKFlag(false);
        bones[n] = bone;
    }

    // IKデータ数
    short ikNum;
    fileReader_.read(reinterpret_cast<char *>(&ikNum), 2);

    for (int n = 0; n < ikNum; ++n) {
        Bone::IK ik;
        int boneIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);
        ik.targetIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);
        int linkNum = fileReader_.readVariableSizeSignedData(1);
        ik.loopNum = fileReader_.readVariableSizeSignedData(2);
        float unitAngle;
        fileReader_.read(reinterpret_cast<char *>(&unitAngle), 4);
        ik.unitAngle = unitAngle * 4;
        for (int i = 0; i < linkNum; ++i) {
            ik.linkIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
            if ((bones[boneIndex].getBoneName().find("左足ＩＫ\0") != std::string::npos ||
                    bones[boneIndex].getBoneName().find("右足ＩＫ\0") != std::string::npos) && i == 0) {
                ik.angleLimitFlags.push_back(true);
            } else {
                ik.angleLimitFlags.push_back(false);
            }
        }
        bones[boneIndex].setIK(ik);
        bones[boneIndex].setIKFlag(true);
    }

    for (int i = 0; i < boneNum; ++i) {
        model.pushBackBone(bones[i]);
    }

    return true;
}
