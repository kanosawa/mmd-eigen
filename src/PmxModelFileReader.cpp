#include "PmxModelFileReader.h"

using namespace std;
using namespace mmd;


PmxModelFileReader::PmxModelFileReader(const string &filename)
        : ModelFileReader(filename) {
}


PmxModelFileReader::~PmxModelFileReader() {
}


void PmxModelFileReader::readFile(PmxModel& model) {

    // ヘッダの読み込み
    if (!readHeader(model)) {
        cout << "PmxModelFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // モデル情報の読み込み
    if (!readModelInfo()) {
        cout << "PmxModelFileReader::readFile() : ModelInfo Error\n";
        exit(0);
    }

    // 頂点の読み込み
    if (!readVertices(model)) {
        cout << "PmxModelFileReader::readFile() : Vertices Error\n";
        exit(0);
    }

    // サーフェスの読み込み
    if (!readSurfaces(model)) {
        cout << "PmxModelFileReader::readFile() : Surfaces Error\n";
        exit(0);
    }

    // テクスチャの読み込み
    if (!readTextures(model)) {
        cout << "PmxModelFileReader::readFile() : Textures Error\n";
        exit(0);
    }

    // マテリアルの読み込み
    if (!readMaterials(model)) {
        cout << "PmxModelFileReader::readFile() : Materials Error\n";
        exit(0);
    }

    // ボーンの読み込み
    if (!readBones(model)) {
        cout << "PmxModelFileReader::readFile() : Bones Error\n";
        exit(0);
    }

    // 子ボーンインデックスの算出
    if (!calcChildBoneIndices(model)) {
        cout << "PmxModelFileReader::readFile() : ChileBoneIndices Error\n";
        exit(0);
    }
}

bool PmxModelFileReader::readHeader(PmxModel &model) {
    // "PMX "の確認(ASCII)
    const int PMX_LENGTH = 4;
    char charPmx[PMX_LENGTH];

    fileReader_.read(reinterpret_cast<char *>(charPmx), PMX_LENGTH);
    if (strncmp(charPmx, "PMX ", PMX_LENGTH) != 0) {
        return false;
    }

    // バージョンの確認
    float pmxVersion;
    fileReader_.read(reinterpret_cast<char *>(&pmxVersion), 4);
    if (pmxVersion != 2) return false;

    // データ列のバイトサイズの確認
    unsigned char byteSize;
    fileReader_.read(reinterpret_cast<char *>(&byteSize), 1);
    if (byteSize != 8) return false;

    // エンコード方式
    fileReader_.read(reinterpret_cast<char *>(&headerInfo_.encodeType), 1);
    if (headerInfo_.encodeType != 0 && headerInfo_.encodeType != 1) return false;

    // 追加UV数
    fileReader_.read(reinterpret_cast<char *>(&headerInfo_.appendixUVNum), 1);
    if (headerInfo_.appendixUVNum != 0) {
        std::cout << "Thie code does not support appndix uv\n";
        return false;
    }

    // インデックスサイズ
    if (!readAllIndexSize()) return false;

    return true;
}


bool PmxModelFileReader::readAllIndexSize() {
    // 頂点インデックスサイズ
    if (!readIndexSize(headerInfo_.vertexIndexSize)) return false;

    // テクスチャインデックスサイズ
    if (!readIndexSize(headerInfo_.textureIndexSize)) return false;

    // マテリアルインデックスサイズ
    if (!readIndexSize(headerInfo_.materialIndexSize)) return false;

    // ボーンインデックスサイズ
    if (!readIndexSize(headerInfo_.boneIndexSize)) return false;

    // モーフインデックスサイズ
    if (!readIndexSize(headerInfo_.morphIndexSize)) return false;

    // 剛体インデックスサイズ
    if (!readIndexSize(headerInfo_.rigidbodyIndexSize)) return false;

    return true;
}


bool PmxModelFileReader::readIndexSize(unsigned char &indexSize) {
    fileReader_.read(reinterpret_cast<char *>(&indexSize), 1);
    if (indexSize != 1 && indexSize != 2 && indexSize != 4) return false;
    return true;
}


bool PmxModelFileReader::readModelInfo() {
    const int MODEL_INFO_NUM = 4; // モデル名、モデル名英、コメント、コメント英
    int infoSize;
    for (int i = 0; i < MODEL_INFO_NUM; ++i) {
        // バイト数
        fileReader_.read(reinterpret_cast<char *>(&infoSize), 4);

        readFromUTF(infoSize, headerInfo_.encodeType);
    }

    return true;
}


bool PmxModelFileReader::readVertices(PmxModel &model) {
    // 頂点数
    int vertexNum;
    fileReader_.read(reinterpret_cast<char *>(&vertexNum), 4);

    for (int n = 0; n < vertexNum; ++n) {

        // 三次元座標
        Eigen::Vector3f pos;
        fileReader_.read(reinterpret_cast<char *>(&pos), 12);

        // 法線
        Eigen::Vector3f normal;
        fileReader_.read(reinterpret_cast<char *>(&normal), 12);

        // UV
        Eigen::Vector2f uv;
        fileReader_.read(reinterpret_cast<char *>(&uv), 8);

        // weightType
        unsigned char weightType;
        fileReader_.read(reinterpret_cast<char *>(&weightType), 1);

        // 参照ボーンインデックスと重み
        vector<int> boneIndices;
        vector<float> boneWeightList;
        if (weightType == 0) {
            boneIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
            boneWeightList.push_back(1.0f);
        } else if (weightType == 1) {
            for (int i = 0; i < 2; ++i) {
                boneIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
            }
            float weight;
            fileReader_.read(reinterpret_cast<char *>(&weight), 4);
            boneWeightList.push_back(weight);
            boneWeightList.push_back(1.0f - weight);
        } else if (weightType == 2) {
            for (int i = 0; i < 4; ++i) {
                boneIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
            }
            for (int i = 0; i < 4; ++i) {
                float weight;
                fileReader_.read(reinterpret_cast<char *>(&weight), 4);
                boneWeightList.push_back(weight);
            }
        }
        else {
            cout << "This code does not support SDEF\n";
        }

        model.pushBackVertex(Vertex(pos, uv, boneIndices, boneWeightList));

        // edge
        float edge;
        fileReader_.read(reinterpret_cast<char *>(&edge), 4);
    }

    return true;
}


bool PmxModelFileReader::readSurfaces(PmxModel &model) {
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


bool PmxModelFileReader::readTextures(PmxModel &model) {
    // テクスチャ数
    int textureNum;
    fileReader_.read(reinterpret_cast<char *>(&textureNum), 4);

    for (int n = 0; n < textureNum; ++n) {
        // テクスチャファイル名取得
        int nameSize;
        fileReader_.read(reinterpret_cast<char *>(&nameSize), 4);
        string textureName = readFromUTF(nameSize, headerInfo_.encodeType);

        // テクスチャ画像取得
        cv::Mat textureImage = cv::imread((fileReader_.getFolderName() + textureName).c_str());
        cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);
        model.pushBackTexture(textureImage);
    }

    return true;
}

bool PmxModelFileReader::readMaterials(PmxModel &model) {
    // マテリアル数
    int materialNum;
    fileReader_.read(reinterpret_cast<char *>(&materialNum), 4);

    for (int n = 0; n < materialNum; ++n) {

        int tmpSize;
        float tmpFloat;
        unsigned char tmpByte;

        // マテリアル名
        for (int i = 0; i < 2; ++i) {
            fileReader_.read(reinterpret_cast<char *>(&tmpSize), 4);
            readFromUTF(tmpSize, headerInfo_.encodeType);
        }

        // ディフューズ
        Eigen::Vector4f diffuse;
        fileReader_.read(reinterpret_cast<char *>(&diffuse), 16);

        // スペキュラー
        Eigen::Vector3f specular;
        fileReader_.read(reinterpret_cast<char *>(&specular), 12);

        // スペキュラー係数
        float specularCoef;
        fileReader_.read(reinterpret_cast<char *>(&specularCoef), 4);

        // アンビエント
        Eigen::Vector3f ambient;
        fileReader_.read(reinterpret_cast<char *>(&ambient), 12);

        // 描画フラグ
        fileReader_.read(reinterpret_cast<char *>(&tmpByte), 1);

        // エッジ情報
        for (int i = 0; i < 5; ++i) {
            fileReader_.read(reinterpret_cast<char *>(&tmpFloat), 4);
        }

        // 通常テクスチャインデックス
        int ordinaryTextureIndex = fileReader_.readVariableSizeSignedData(headerInfo_.textureIndexSize);

        // 読み飛ばし
        for (int i = 0; i < 4; ++i) {
            fileReader_.read(reinterpret_cast<char *>(&tmpByte), 1);
        }

        // メモ
        fileReader_.read(reinterpret_cast<char *>(&tmpSize), 4);
        readFromUTF(tmpSize, headerInfo_.encodeType);

        // 面数
        int vertexNum;
        fileReader_.read(reinterpret_cast<char *>(&vertexNum), 4);
        if (vertexNum % 3 != 0) {
            std::cout << "vertexNum Error\n";
            return false;
        }

        model.pushBackMaterial(Material(diffuse, specular, specularCoef, ambient, ordinaryTextureIndex, vertexNum / 3));
    }

    return true;
}

bool PmxModelFileReader::readBones(PmxModel &model) {
    // ボーン数
    int boneNum;
    fileReader_.read(reinterpret_cast<char *>(&boneNum), 4);

    for (int n = 0; n < boneNum; ++n) {

        // ボーン名サイズ
        int boneNameSize;
        fileReader_.read(reinterpret_cast<char *>(&boneNameSize), 4);

        // ボーン名（日本語）
        string boneName = readFromUTF(boneNameSize, headerInfo_.encodeType);
        // ボーン名（英語）
        fileReader_.read(reinterpret_cast<char *>(&boneNameSize), 4);
        readFromUTF(boneNameSize, headerInfo_.encodeType);

        // 三次元座標
        Eigen::Vector3f position;
        fileReader_.read(reinterpret_cast<char *>(&position), 12);

        // 親ボーンインデックス
        int parentBoneIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);

        // ボーンの生成
        Bone bone(boneName, position, parentBoneIndex);

        // 読み飛ばし
        float tmp4;
        fileReader_.read(reinterpret_cast<char *>(&tmp4), 4);

        // ボーンフラグ
        short boneFlag;
        fileReader_.read(reinterpret_cast<char *>(&boneFlag), 2);

        // ボーンフラグ分解
        short destinationMask = 0x0001;
        short IKMask = 0x0020;
        short addRotMask = 0x0100;
        short addShiftMask = 0x0200;
        short axisFixMask = 0x0400;
        short localAxisMask = 0x0800;
        short extParentChangeMask = 0x2000;
        bool destinationFlag = ((boneFlag & destinationMask) != 0);
        bool IKFlag = ((boneFlag & IKMask) != 0);
        bone.setIKFlag(IKFlag);
        bool assignRotFlag = ((boneFlag & addRotMask) != 0);
        bone.setAssignRotFlag(assignRotFlag);
        bool assignShiftFlag = ((boneFlag & addShiftMask) != 0);
        bone.setAssignShiftFlag(assignShiftFlag);
        bool axisFixFlag = ((boneFlag & axisFixMask) != 0);
        bool localAxisFlag = ((boneFlag & localAxisMask) != 0);
        bool extParentChageFlag = ((boneFlag & extParentChangeMask) != 0);
        bone.setDestinationFlag(destinationFlag);

        // ボーンフラグに沿って各種データ取得
        if (destinationFlag) {
            bone.setDestinationBoneIndex(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));
        } else {
            Eigen::Vector3f offset;
            fileReader_.read(reinterpret_cast<char *>(&offset), 12);
            bone.setOffset(offset);
        }

        if (assignRotFlag || assignShiftFlag) {
            int assignParentIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);
            float assignRatio;
            fileReader_.read(reinterpret_cast<char *>(&assignRatio), 4);
            bone.setAssignParentIndex(assignParentIndex);
            bone.setAssignRatio(assignRatio);
        }

        Eigen::Vector3f tmp12;
        if (axisFixFlag) {
            fileReader_.read(reinterpret_cast<char *>(&tmp12), 12);
        }

        if (localAxisFlag) {
            fileReader_.read(reinterpret_cast<char *>(&tmp12), 12);
            fileReader_.read(reinterpret_cast<char *>(&tmp12), 12);
        }

        if (extParentChageFlag) {
            fileReader_.read(reinterpret_cast<char *>(&tmp4), 4);
        }

        if (IKFlag) {
            Bone::IK ik;
            ik.targetIndex = fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize);
            fileReader_.read(reinterpret_cast<char *>(&ik.loopNum), 4);
            fileReader_.read(reinterpret_cast<char *>(&ik.unitAngle), 4);
            int IKLinkNum;
            fileReader_.read(reinterpret_cast<char *>(&IKLinkNum), 4);
            for (int i = 0; i < IKLinkNum; ++i) {
                ik.linkIndices.push_back(fileReader_.readVariableSizeSignedData(headerInfo_.boneIndexSize));

                bool angleLimitFlag;
                fileReader_.read(reinterpret_cast<char *>(&angleLimitFlag), 1);
                ik.angleLimitFlags.push_back(angleLimitFlag);
                if (angleLimitFlag) {
                    fileReader_.read(reinterpret_cast<char *>(&tmp12), 12);
                    fileReader_.read(reinterpret_cast<char *>(&tmp12), 12);
                }
            }
            bone.setIK(ik);
        }

        model.pushBackBone(bone);
    }

    return true;
}


const string PmxModelFileReader::readFromUTF(const int size, const char encodeType)
{
    if (encodeType == 0) {
        return fileReader_.readFromUTF16(size);
    } else {
        return fileReader_.readFromUTF8(size);
    }
}