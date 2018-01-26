#include "PmxFileReader.h"

using namespace std;
using namespace mmd;

PmxFileReader::PmxFileReader(const string &filename) {
    multiCharaCodeFileReader_ = make_unique<MultiCharaCodeFileReader>("UTF-16");

    fileStream_.open(filename.c_str(), ios::binary);
    if (!fileStream_) {
        cout << "File Open Error in PmxFileReader()\n";
        exit(0);
    }
}


PmxFileReader::~PmxFileReader() {
}


std::unique_ptr<PmxModel> PmxFileReader::readFile() {
    // モデルの生成
    std::unique_ptr<PmxModel> model = make_unique<PmxModel>();

    // ヘッダの読み込み
    if (!readHeader(model)) {
        cout << "PmxFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // モデル情報の読み込み
    if (!readModelInfo()) {
        cout << "PmxFileReader::readFile() : ModelInfo Error\n";
        exit(0);
    }

    // 頂点の読み込み
    if (!readVertices(model)) {
        cout << "PmxFileReader::readFile() : Vertices Error\n";
        exit(0);
    }

    // 面の読み込み
    if (!readSurfaces(model)) {
        cout << "PmxFileReader::readFile() : Surfaces Error\n";
        exit(0);
    }

    // テクスチャの読み込み
    if (!readTextures(model)) {
        cout << "PmxFileReader::readFile() : Textures Error\n";
        exit(0);
    }

    // 材質の読み込み
    if (!readMaterials(model)) {
        cout << "PmxFileReader::readFile() : Materials Error\n";
        exit(0);
    }

    // ボーンの読み込み
    if (!readBones(model)) {
        cout << "PmxFileReader::readFile() : Bones Error\n";
        exit(0);
    }

    // 子ボーンインデックスの算出
    if (!calcChildBoneIndices(model)) {
        cout << "PmxFileReader::readFile() : ChileBoneIndices Error\n";
        exit(0);
    }

    return std::move(model);
}

bool PmxFileReader::readHeader(std::unique_ptr<PmxModel> &model) {
    // "PMX "の確認(ASCII)
    const int PMX_LENGTH = 4;
    unsigned char charPmx[PMX_LENGTH];
    fileStream_.read(reinterpret_cast<char *>(charPmx), PMX_LENGTH);
    if ((charPmx[0] != 0x50) || (charPmx[1] != 0x4d) || (charPmx[2] != 0x58) || (charPmx[3] != 0x20))
        return false;

    // バージョンの確認
    float pmxVersion;
    fileStream_.read(reinterpret_cast<char *>(&pmxVersion), 4);
    if (pmxVersion != 2) return false;

    // データ列のバイトサイズの確認
    unsigned char byteSize;
    fileStream_.read(reinterpret_cast<char *>(&byteSize), 1);
    if (byteSize != 8) return false;

    // エンコード方式
    fileStream_.read(reinterpret_cast<char *>(&pmxHeaderInfo_.encodeType), 1);
    if (pmxHeaderInfo_.encodeType != 0 && pmxHeaderInfo_.encodeType != 1) return false;

    // 追加UV数
    fileStream_.read(reinterpret_cast<char *>(&pmxHeaderInfo_.appendixUVNum), 1);
    if (pmxHeaderInfo_.appendixUVNum != 0) {
        std::cout << "Thie code does not support appndix uv\n";
        return false;
    }

    // インデックスサイズ
    if (!readAllIndexSize()) return false;

    return true;
}


bool PmxFileReader::readAllIndexSize() {
    // 頂点インデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.vertexIndexSize)) return false;

    // テクスチャインデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.textureIndexSize)) return false;

    // マテリアルインデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.materialIndexSize)) return false;

    // ボーンインデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.boneIndexSize)) return false;

    // モーフインデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.morphIndexSize)) return false;

    // 剛体インデックスサイズ
    if (!readIndexSize(pmxHeaderInfo_.rigidbodyIndexSize)) return false;

    return true;
}


bool PmxFileReader::readIndexSize(unsigned char &indexSize) {
    fileStream_.read(reinterpret_cast<char *>(&indexSize), 1);
    if (indexSize != 1 && indexSize != 2 && indexSize != 4) return false;
    return true;
}


bool PmxFileReader::readModelInfo() {
    const int MODEL_INFO_NUM = 4; // モデル名、モデル名英、コメント、コメント英
    int infoSize;

    bool needCodeTrans = false;
    if (pmxHeaderInfo_.encodeType == 0) {
        needCodeTrans = true;
    }

    for (int i = 0; i < MODEL_INFO_NUM; ++i) {
        // バイト数
        fileStream_.read(reinterpret_cast<char *>(&infoSize), 4);

        multiCharaCodeFileReader_->fread(fileStream_, infoSize, needCodeTrans, true);
    }

    return true;
}


bool PmxFileReader::readVertices(std::unique_ptr<PmxModel> &model) {
    // 頂点数
    int vertexNum;
    fileStream_.read(reinterpret_cast<char *>(&vertexNum), 4);

    for (int n = 0; n < vertexNum; ++n) {

        // 三次元座標
        Eigen::Vector3f pos;
        fileStream_.read(reinterpret_cast<char *>(&pos), 12);

        // 法線
        Eigen::Vector3f normal;
        fileStream_.read(reinterpret_cast<char *>(&normal), 12);

        // UV
        Eigen::Vector2f uv;
        fileStream_.read(reinterpret_cast<char *>(&uv), 8);

        // weightType
        unsigned char weightType;
        fileStream_.read(reinterpret_cast<char *>(&weightType), 1);

        // 参照ボーンインデックスと重み
        vector<int> boneIndices;
        vector<float> boneWeightList;
        if (weightType == 0) {
            boneIndices.push_back(readVariableSizeUnsignedData(pmxHeaderInfo_.boneIndexSize));
            boneWeightList.push_back(1.0f);
        } else if (weightType == 1) {
            for (int i = 0; i < 2; ++i) {
                boneIndices.push_back(readVariableSizeUnsignedData(pmxHeaderInfo_.boneIndexSize));
            }
            float weight;
            fileStream_.read(reinterpret_cast<char *>(&weight), 4);
            boneWeightList.push_back(weight);
            boneWeightList.push_back(1.0f - weight);
        } else {
            cout << "This code does not support BDEF4 and SDEF\n";
        }

        model->pushBackVertex(Vertex(pos, uv, boneIndices, boneWeightList));

        // edge
        float edge;
        fileStream_.read(reinterpret_cast<char *>(&edge), 4);
    }

    return true;
}


bool PmxFileReader::readSurfaces(std::unique_ptr<PmxModel> &model) {
    int vertexIndexNum; // インデックス数。3インデックスで1面
    fileStream_.read(reinterpret_cast<char *>(&vertexIndexNum), 4);

    for (int n = 0; n < vertexIndexNum / 3; ++n) {
        Eigen::Vector3i vertexIndexies;
        for (int i = 0; i < 3; ++i) {
            vertexIndexies[i] = readVariableSizeUnsignedData(pmxHeaderInfo_.vertexIndexSize);
        }
        TriangleSurface triangleSurface(vertexIndexies);
        model->pushBackSurface(triangleSurface);
    }
    return true;
}


bool PmxFileReader::readTextures(std::unique_ptr<PmxModel> &model) {
    // テクスチャ数
    int textureNum;
    fileStream_.read(reinterpret_cast<char *>(&textureNum), 4);

    bool needCodeTrans = false;
    if (pmxHeaderInfo_.encodeType == 0) {
        needCodeTrans = true;
    }

    for (int n = 0; n < textureNum; ++n) {
        // テクスチャファイル名取得
        int nameSize;
        fileStream_.read(reinterpret_cast<char *>(&nameSize), 4);
        string textureNameTstring = multiCharaCodeFileReader_->fread(fileStream_, nameSize, needCodeTrans, true);
        char textureNameChar[BUFSIZ];
        strcpy(textureNameChar, textureNameTstring.c_str());

        // テクスチャ画像取得
        cv::Mat textureImage = cv::imread(textureNameChar);
        cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);
        model->pushBackTexture(textureImage);
    }

    return true;
}

bool PmxFileReader::readMaterials(std::unique_ptr<PmxModel> &model) {
    // マテリアル数
    int materialNum;
    fileStream_.read(reinterpret_cast<char *>(&materialNum), 4);

    bool needCodeTrans = false;
    if (pmxHeaderInfo_.encodeType == 0) {
        needCodeTrans = true;
    }

    for (int n = 0; n < materialNum; ++n) {

        Material material;

        int tmpSize;
        float tmpFloat;
        unsigned char tmpByte;
        string tmpStr;

        // マテリアル名
        for (int i = 0; i < 2; ++i) {
            fileStream_.read(reinterpret_cast<char *>(&tmpSize), 4);
            tmpStr = multiCharaCodeFileReader_->fread(fileStream_, tmpSize, needCodeTrans, true);
        }

        // ディフューズ
        Eigen::Vector4f diffuse;
        fileStream_.read(reinterpret_cast<char *>(&diffuse), 16);
        material.setDiffuse(diffuse);

        // スペキュラー
        Eigen::Vector3f specular;
        fileStream_.read(reinterpret_cast<char *>(&specular), 12);
        material.setSpecular(specular);

        // スペキュラー係数
        float specularCoef;
        fileStream_.read(reinterpret_cast<char *>(&specularCoef), 4);
        material.setSpecularCoef(specularCoef);

        // アンビエント
        Eigen::Vector3f ambient;
        fileStream_.read(reinterpret_cast<char *>(&ambient), 12);
        material.setSpecular(ambient);

        // 描画フラグ
        fileStream_.read(reinterpret_cast<char *>(&tmpByte), 1);

        // エッジ情報
        for (int i = 0; i < 5; ++i) {
            fileStream_.read(reinterpret_cast<char *>(&tmpFloat), 4);
        }

        // 通常テクスチャインデックス
        unsigned char ordinaryTextureIndex = readVariableSizeSignedData(pmxHeaderInfo_.textureIndexSize);
        material.setOrdinaryTextureIndex(ordinaryTextureIndex);

        // 読み飛ばし
        for (int i = 0; i < 4; ++i) {
            fileStream_.read(reinterpret_cast<char *>(&tmpByte), 1);
        }

        // メモ
        fileStream_.read(reinterpret_cast<char *>(&tmpSize), 4);
        tmpStr = multiCharaCodeFileReader_->fread(fileStream_, tmpSize, false, false);

        // 面数
        int vertexNum;
        fileStream_.read(reinterpret_cast<char *>(&vertexNum), 4);
        if (vertexNum % 3 != 0) {
            std::cout << "vertexNum Error\n";
            return false;
        }
        material.setSurfaceNum(vertexNum / 3);

        model->pushBackMaterial(material);
    }

    return true;
}

bool PmxFileReader::readBones(std::unique_ptr<PmxModel> &model) {
    // ボーン数
    int boneNum;
    fileStream_.read(reinterpret_cast<char *>(&boneNum), 4);

    bool needCodeTrans = false;
    if (pmxHeaderInfo_.encodeType == 0) {
        needCodeTrans = true;
    }

    for (int n = 0; n < boneNum; ++n) {

        Bone bone;

        // ボーン名（日本語）
        int boneNameSize;
        fileStream_.read(reinterpret_cast<char *>(&boneNameSize), 4);
        bone.setBoneName(multiCharaCodeFileReader_->fread(fileStream_, boneNameSize, needCodeTrans, true));

        // ボーン名（英語）
        fileStream_.read(reinterpret_cast<char *>(&boneNameSize), 4);
        multiCharaCodeFileReader_->fread(fileStream_, boneNameSize, needCodeTrans, true);

        // 三次元座標
        Eigen::Vector3f position;
        fileStream_.read(reinterpret_cast<char *>(&position), 12);
        bone.setPosition(position);

        // 親ボーンインデックス
        bone.setParentBoneIndex(readVariableSizeSignedData(pmxHeaderInfo_.boneIndexSize));

        // 読み飛ばし
        double tmp;
        fileStream_.read(reinterpret_cast<char *>(&tmp), 4);

        // ボーンフラグ
        short boneFlag;
        fileStream_.read(reinterpret_cast<char *>(&boneFlag), 2);

        // ボーンフラグ分解
        short destinationMask = 0x0001;
        short IKMask = 0x0020;
        short rotationAddMask = 0x0100;
        short moveAddMask = 0x0200;
        short axisFixMask = 0x0400;
        short localAxisMask = 0x0800;
        short extParentChangeMask = 0x2000;
        bool destinationFlag = ((boneFlag & destinationMask) != 0);
        bool IKFlag = ((boneFlag & IKMask) != 0);
        bool rotationAddFlag = ((boneFlag & rotationAddMask) != 0);
        bool moveAddFlag = ((boneFlag & moveAddMask) != 0);
        bool axisFixFlag = ((boneFlag & axisFixMask) != 0);
        bool localAxisFlag = ((boneFlag & localAxisMask) != 0);
        bool extParentChageFlag = ((boneFlag & extParentChangeMask) != 0);
        bone.setDestinationFlag(destinationFlag);

        // ボーンフラグに沿って各種データ取得
        if (destinationFlag) {
            bone.setDestinationBoneIndex(readVariableSizeSignedData(pmxHeaderInfo_.boneIndexSize));
        } else {
            Eigen::Vector3f offset;
            fileStream_.read(reinterpret_cast<char *>(&offset), 12);
            bone.setOffset(offset);
        }

        if (rotationAddFlag || moveAddFlag) {
            fileStream_.read(reinterpret_cast<char *>(&tmp), 4 + pmxHeaderInfo_.boneIndexSize);
        }

        if (axisFixFlag) {
            Eigen::Vector3f tmp;
            fileStream_.read(reinterpret_cast<char *>(&tmp), 12);
        }

        if (localAxisFlag) {
            Eigen::Vector3f tmp;
            fileStream_.read(reinterpret_cast<char *>(&tmp), 12);
            fileStream_.read(reinterpret_cast<char *>(&tmp), 12);
        }

        if (extParentChageFlag) {
            fileStream_.read(reinterpret_cast<char *>(&tmp), 4);
        }

        if (IKFlag) {
            fileStream_.read(reinterpret_cast<char *>(&tmp), pmxHeaderInfo_.boneIndexSize);
            fileStream_.read(reinterpret_cast<char *>(&tmp), 8);

            int IKLinkNum;
            fileStream_.read(reinterpret_cast<char *>(&IKLinkNum), 4);
            for (int i = 0; i < IKLinkNum; ++i) {
                fileStream_.read(reinterpret_cast<char *>(&tmp), pmxHeaderInfo_.boneIndexSize);

                bool angleLimitFlag;
                fileStream_.read(reinterpret_cast<char *>(&angleLimitFlag), 1);
                if (angleLimitFlag) {
                    Eigen::Vector3f tmp;
                    fileStream_.read(reinterpret_cast<char *>(&tmp), 12);
                    fileStream_.read(reinterpret_cast<char *>(&tmp), 12);
                }
            }
        }

        model->pushBackBone(bone);
    }

    return true;
}


bool PmxFileReader::calcChildBoneIndices(std::unique_ptr<PmxModel> &model) {
    // 全ての親ボーンの探索
    int parentIndex = -1;
    for (int boneIndex = 0; boneIndex < model->getBoneNum(); ++boneIndex) {
        if (model->getBone(boneIndex).getParentBoneIndex() == parentIndex) {
            parentIndex = boneIndex;
            break;
        }
    }
    if (parentIndex == -1) {
        cout << "全ての親ボーンが見つかりません\n";
        return false;
    }

    // 子ボーンを登録していく
    vector<int> parentIndices;
    parentIndices.push_back(parentIndex);
    searchChildBone(model, parentIndices);

    return true;
}

void PmxFileReader::searchChildBone(std::unique_ptr<PmxModel> &model, const vector<int> &parentBoneIndices) {
    for (unsigned int parent = 0; parent < parentBoneIndices.size(); ++parent) {
        vector<int> childBoneIndices;
        for (int child = 0; child < model->getBoneNum(); ++child) {
            // もし、あるボーンにparentIndices[i]が親ボーンとして登録されていたら
            if (model->getBone(child).getParentBoneIndex() == parentBoneIndices[parent]) {
                // そのボーンインデックスを子ボーンインデックスとして親ボーンに登録
                model->getBone(parentBoneIndices[parent]).pushBackChildBoneIndex(child);
                // 登録した子ボーンは後で親ボーンとして参照するのでvectorに突っ込んでおく
                childBoneIndices.push_back(child);
            }
        }
        // 子ボーンを親ボーンとして再帰的に探索
        searchChildBone(model, childBoneIndices);
    }
}

int PmxFileReader::readVariableSizeUnsignedData(const int dataSize) {
    int data = 0;
    for (int b = 0; b < dataSize; ++b) {
        unsigned char tmp;
        fileStream_.read(reinterpret_cast<char *>(&tmp), 1);
        data += tmp * static_cast<int>(pow(256, b));
    }
    return data;
}

int PmxFileReader::readVariableSizeSignedData(const int dataSize) {
    int data = readVariableSizeUnsignedData(dataSize);

    int max = static_cast<int>(pow(256, dataSize));
    if (data >= max / 2) {
        data = -(max - data);
    }

    return data;
}

