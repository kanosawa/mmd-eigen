#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GL/glut.h>
#include "BoneStream.h"
#include "VertexStream.h"
#include "PmxFileReader.h"
#include "VmdFileReader.h"

using namespace std;

unique_ptr<mmd::PmxModel> model;
std::unique_ptr<mmd::VmdDataStream> vmdDataStream;

mmd::BoneStream boneStream;
mmd::VertexStream vertexStream;
map<int, vector<mmd::Vertex>>::const_iterator vertexItr;
map<int, vector<mmd::Bone>>::const_iterator boneItr;


const int textureNum = 17;
GLuint texname[textureNum];
GLuint vbo;

int width;
int height;
int mouse_l = 0;
int mouse_m = 0;
int mouse_r = 0;
int mpos[2];
double trans[3] = {0.0, 0.0, 0.0};
double theta[3] = {0.0, 0.0, 0.0};
double angle[3] = {0.0, 0.0, 0.0};

int superParentIndex;
vector<vector<mmd::Motion>> boneMotions; // ボーンごとのモーション
vector<vector<mmd::Motion>::iterator> boneMotionsItrs;
int currentFrame = 0;
vector<mmd::Bone> currentBones;
vector<mmd::Vertex> currentVertex;
int beforeTime;

void display(void) {
    glViewport(0, 0, width, height);
    glLoadIdentity();
    gluPerspective(15.0, (double) width / (double) height, 1.0, 1000.0);
    gluLookAt(-0.0, 70.0, -100.0, -8.0, 10.0, 0.0, 0.0, 1.0, 0.0);

    glRotated(angle[0], 1.0, 0.0, 0.0);
    glRotated(angle[1], 0.0, 1.0, 0.0);
    glRotated(angle[2], 0.0, 0.0, 1.0);

    //vector<mmd::Bone> bones = model->getBones();
    //vector<mmd::Vertex> vertices = model->getVertices();
    //vector<mmd::Bone> bones = boneItr->second;
    //vector<mmd::Vertex> vertices = vertexItr->second;

    vector<mmd::Bone> bones = currentBones;
    vector<mmd::Vertex> vertices = currentVertex;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // �|���S���̕`��
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    int surfaceNo = 0;
    for (int i = 0; i < model->getMaterialNum(); ++i) {
        int ordinaryTextureIndex = model->getMaterial(i).getOrdinaryTextureIndex();
        glBindTexture(GL_TEXTURE_2D, texname[ordinaryTextureIndex]);

        glBegin(GL_TRIANGLES);
        int surfaceNum = model->getMaterial(i).getSurfaceNum();
        for (int s = 0; s < surfaceNum; ++s) {
            mmd::TriangleSurface triangleSurface = model->getSurface(surfaceNo);
            Eigen::Vector3i vertexIndexies = triangleSurface.getVertexIndexies();
            for (int j = 0; j < 3; ++j) {
                mmd::Vertex vertex = vertices[vertexIndexies[j]];
                glTexCoord2f(vertex.getUv().x(), vertex.getUv().y());
                glVertex3f(vertex.getPosition().x(), vertex.getPosition().y(), vertex.getPosition().z());
            }
            ++surfaceNo;
        }
        glEnd();
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);


	// �{�[���_�̕\��
	glPointSize(2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	int boneNum = model->getBoneNum();
	for (int b = 0; b < boneNum; ++b) {
        Eigen::Vector3f pos = bones[b].getTemporalPosition();
		glVertex3f(pos.x(), pos.y(), pos.z());
	}
	glEnd();


	// �{�[���̕\��
    glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2);
	glBegin(GL_LINES);
	for (int b = 0; b < boneNum; ++b) {
		mmd::Bone bone = bones[b];

		// �n�_
        Eigen::Vector3f pos = bone.getTemporalPosition();
		glVertex3f(pos.x(), pos.y(), pos.z());

		// �I�_
		Eigen::Vector3f destination;
		if (bone.getDestinationFlag()) {
			int destinationBoneIndex = bone.getDestinationBoneIndex();
			if (destinationBoneIndex < 0) {
				destination = pos;
			}
			else {
                destination = bones[destinationBoneIndex].getTemporalPosition();
			}
		}
		else {
			destination = pos + bone.getOffset();
		}

		glVertex3f(destination.x(), destination.y(), destination.z());
	}
	glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                mpos[0] = x;
                mpos[1] = y;
                mouse_l = 1;
            }
            if (state == GLUT_UP) {
                mouse_l = 0;
            }
            break;
        default:
            break;
    }
}

void motion(int x, int y) {
    if (mouse_l == 1) {
        theta[0] = (double) (y - mpos[1]) / 5.0;
        theta[1] = (double) (x - mpos[0]) / 5.0;
    }
    if (mouse_l == 1 || mouse_m == 1 || mouse_r == 1) {
        mpos[0] = x;
        mpos[1] = y;
        angle[0] += theta[0];
        angle[1] += theta[1];
        glutPostRedisplay();
    }
}

void resize(int w, int h) {
    width = w;
    height = h;
}

void moveChildBones(vector<mmd::Bone> &bones, const int parentBoneIndex,
                                   const vector<mmd::Motion> &frameMotions) {

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

void update(vector<mmd::Bone>& bones)
{
    cout << currentFrame << endl;

    vector<mmd::Motion> frameMotions(bones.size());
    for (unsigned int i = 0; i < frameMotions.size(); ++i) {
        if (boneMotionsItrs[i]->getFrameNo() == currentFrame) {
            frameMotions[i] = *boneMotionsItrs[i];
        } else if (distance(boneMotionsItrs[i], boneMotions[i].begin()) != boneMotions[i].size() - 1 && (boneMotionsItrs[i]+1)->getFrameNo() == currentFrame) {
            frameMotions[i] = *(++(boneMotionsItrs[i]));
        } else if (boneMotionsItrs[i]->getFrameNo() < currentFrame && distance(boneMotionsItrs[i], boneMotions[i].begin()) != boneMotions[i].size() - 1) {
            float ratio = float(currentFrame - boneMotionsItrs[i]->getFrameNo()) /
                          ((boneMotionsItrs[i]+1)->getFrameNo() - boneMotionsItrs[i]->getFrameNo());
            Eigen::Vector3f shift = boneMotionsItrs[i]->getShift() * (1.0f - ratio) + (boneMotionsItrs[i]+1)->getShift() * ratio;
            Eigen::Quaternionf quaternion = boneMotionsItrs[i]->getQuaternion().slerp(
                    ratio, (boneMotionsItrs[i]+1)->getQuaternion());
            frameMotions[i] = mmd::Motion(i, currentFrame, shift, quaternion);
        } else if (boneMotionsItrs[i]->getFrameNo() < currentFrame && distance(boneMotionsItrs[i], boneMotions[i].begin()) == boneMotions[i].size() - 1) {
            frameMotions[i] = *boneMotionsItrs[i];
        } else {
            cout << "error\n";
        }
    }

    bones[superParentIndex].setTemporalPosition(
            bones[superParentIndex].getInitialPosition() + frameMotions[superParentIndex].getShift());
    bones[superParentIndex].setTemporalQuaternion(frameMotions[superParentIndex].getQuaternion());

    moveChildBones(bones, superParentIndex, frameMotions);

    vector<mmd::Vertex> initialVertices = model->getVertices();
    for (unsigned int i = 0; i < initialVertices.size(); ++i) {
        vector<int> refBoneIndices = initialVertices[i].getRefBoneIndices();
        vector<float> refBoneWeightList = initialVertices[i].getRefBoneWeightList();
        Eigen::Vector3f pos(0, 0, 0);
        for (unsigned int b = 0; b < refBoneIndices.size(); ++b) {

            int boneIndex = refBoneIndices[b];

            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (bones[boneIndex].getTemporalQuaternion() *
                    (initialVertices[i].getPosition() - bones[boneIndex].getInitialPosition()) +
                    bones[boneIndex].getTemporalPosition()) *
                   refBoneWeightList[b];
        }
        // refBoneIndices.size()で割る必要があるのでは？
        currentVertex[i].setPosition(pos);
    }
}

void Idle() {
    int GLtimenow = glutGet(GLUT_ELAPSED_TIME);
    if (GLtimenow - beforeTime > 33)
    {
        update(currentBones);
        glutPostRedisplay();
        ++currentFrame;
        beforeTime = GLtimenow;
    }
}

int searchSuperParentBone(const vector<mmd::Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};

int main(int argc, char *argv[]) {
    // PMXファイルの入力
    mmd::PmxFileReader pmxFileReader("maya.pmx");
    model = pmxFileReader.readFile();

    // VMDファイルの入力
    vector<mmd::Motion> motions;
    mmd::VmdFileReader vmdFileReader("maya.vmd", model->getBones());
    vmdFileReader.readFile(motions);

    // MotionをBoneごとに分ける
    boneMotions.resize(model->getBones().size());
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

    // vector<Motion>::iteratorを先頭にセット
    boneMotionsItrs.resize(boneMotions.size());
    for (unsigned int i = 0; i < boneMotions.size(); ++i) {
        boneMotionsItrs[i] = boneMotions[i].begin();
    }

    // 全ての親ボーンの探索
    superParentIndex = searchSuperParentBone(model->getBones());
    if (superParentIndex == -1) {
        cout << "全ての親ボーンが見つかりません\n";
        return false;
    }

    currentBones = model->getBones();
    //currentVertex.resize(model->getVertices().size());
    currentVertex = model->getVertices();

    //vmdDataStream->calcStream(boneStream, vertexStream, model->getBones(), model->getVertices(), motions);

    // GLUTの準備
    glutInit(&argc, NULL);
    glutInitWindowSize(960, 640);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Maya");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(resize);
    glutIdleFunc(Idle);
    glClearColor(0.0, 0.0, 1.0, 0.0);

    // GLEWの準備
    glewInit();

    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    // glTextureの準備
    glGenTextures(textureNum, texname);
    for (int i = 0; i < model->getTextureNum(); ++i) {
        glBindTexture(GL_TEXTURE_2D, texname[i]);
        cv::Mat texture = model->getTexture(i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols, texture.rows, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &texture.data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    //boneItr = boneStream.getBoneListMap().begin();
    //vertexItr = vertexStream.getVertexListMap().begin();

    beforeTime = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();

    return 0;
}

/*
#include "stdafx.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "PmxFileReader.h"
#include "VmdFileReader.h"
#include "BoneIndexCalculator.h"
#include "VertexStream.h"

using namespace std;

std::unique_ptr<pmx::PmxModel> model;
std::unique_ptr<pmx::VmdDataStream> vmdDataStream;
pmx::BoneStream boneStream;
pmx::VertexStream vertexStream;
map<int, vector<pmx::Vertex>>::const_iterator vertexItr;
map<int, vector<pmx::Bone>>::const_iterator boneItr;

vector<pmx::Bone> initialBones;

int frameNo = 0;

bool firstFrame = true;

const int textureNum = 17;
GLuint texname[textureNum];
GLuint vbo;

int mouse_l = 0;
int mouse_m = 0;
int mouse_r = 0;
int mpos[2];
double trans[3] = { 0.0, 0.0, 0.0 };
double theta[3] = { 0.0, 0.0, 0.0 };
double angle[3] = { 0.0, 0.0, 0.0 };

int width;
int height;

void display(void)
{
	glViewport(0, 0, width, height);
	glLoadIdentity();
	gluPerspective(15.0, (double)width / (double)height, 1.0, 1000.0);
	//glTranslated(0.0, 0.0, -200.0);
	gluLookAt(-0.0, 70.0, -100.0, -8.0, 10.0, 0.0, 0.0, 1.0, 0.0);

	glRotated(angle[0], 1.0, 0.0, 0.0);
	glRotated(angle[1], 0.0, 1.0, 0.0);
	glRotated(angle[2], 0.0, 0.0, 1.0);

	// ���t���[���̃{�[���ƒ��_���擾
	vector<pmx::Bone> bones = boneItr->second;
	vector<pmx::Vertex> vertices = vertexItr->second;
	//vector<pmx::Bone> bones = initialBones;
	//vector<pmx::Vertex> vertices = model->getVertices();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	// �|���S���̕`��
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	int surfaceNo = 0;
	for (int i = 0; i < model->getMaterialNum(); ++i) {
		int ordinaryTextureIndex = model->getMaterial(i).getOrdinaryTextureIndex();
		glBindTexture(GL_TEXTURE_2D, texname[ordinaryTextureIndex]);

		glBegin(GL_TRIANGLES);
		int surfaceNum = model->getMaterial(i).getSurfaceNum();
		for (int s = 0; s < surfaceNum; ++s) {
			pmx::TriangleSurface triangleSurface = model->getSurface(surfaceNo);
			Eigen::Vector3i vertexIndexies = triangleSurface.getVertexIndexies();
			for (int j = 0; j < 3; ++j) {
				pmx::Vertex vertex = vertices[vertexIndexies[j]];
				glTexCoord2f(vertex.getUv().x(), vertex.getUv().y());
				glVertex3f(vertex.getPosition().x(), vertex.getPosition().y(), vertex.getPosition().z());
			}
			++surfaceNo;
		}
		glEnd();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	
	
	// �{�[���_�̕\��
	glPointSize(2);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	int boneNum = model->getBoneNum();
	for (int b = 0; b < boneNum; ++b) {
		Eigen::Vector3f pos = bones[b].getPosition();
		glVertex3f(pos.x(), pos.y(), pos.z());
	}
	glEnd();

	
	// �{�[���̕\��
	glLineWidth(2);
	glBegin(GL_LINES);	
	for (int b = 0; b < boneNum; ++b) {
		pmx::Bone bone = bones[b];

		// �n�_
		Eigen::Vector3f pos = bone.getPosition();
		glVertex3f(pos.x(), pos.y(), pos.z());

		// �I�_
		Eigen::Vector3f destination;
		if (bone.getDestinationFlag()) {
			int destinationBoneIndex = bone.getDestinationBoneIndex();
			if (destinationBoneIndex < 0) {
				destination = pos;
			}
			else {
				destination = bones[destinationBoneIndex].getPosition();
			}
		}
		else {
			destination = pos + bone.getOffset();
		}

		glVertex3f(destination.x(), destination.y(), destination.z());
	}
	glEnd();
	

	glColor3f(1.0f, 1.0f, 1.0f);
	glutSwapBuffers();

	boneItr++;
	vertexItr++;
	if (++boneItr == boneStream.getBoneListMap().end()) boneItr = boneStream.getBoneListMap().begin();
	if (++vertexItr == vertexStream.getVertexListMap().end()) vertexItr = vertexStream.getVertexListMap().begin();
}

void mouse(int button, int state, int x, int y)
{
	switch (button){
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN){
			mpos[0] = x;
			mpos[1] = y;
			mouse_l = 1;
		}
		if (state == GLUT_UP){
			mouse_l = 0;
		}
		break;
	default:
		break;
	}
}

void motion(int x, int y)
{
	if (mouse_l == 1){
		theta[0] = (double)(y - mpos[1]) / 5.0;
		theta[1] = (double)(x - mpos[0]) / 5.0;
	}
	if (mouse_l == 1 || mouse_m == 1 || mouse_r == 1){
		mpos[0] = x;
		mpos[1] = y;
		angle[0] += theta[0];
		angle[1] += theta[1];
		glutPostRedisplay();
	}
}

void resize(int w, int h)
{
	width = w;
	height = h;
}


void Idle()
{
	//glutPostRedisplay();
}


int _tmain(int argc, _TCHAR* argv[])
{
	// GLUT������
	glutInit(&argc, NULL);
	glutInitWindowSize(960, 640);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Maya");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(resize);
	glutIdleFunc(Idle);
	glClearColor(0.0, 0.0, 1.0, 0.0);

	// GLEW������
	glewInit();

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	// �t�@�C����
	pmx::tstring pmxFileName = _T("maya.pmx");
	pmx::tstring vmdFileName = _T("maya.vmd");

	// PMX�t�@�C���̓���
	pmx::PmxFileReader pmxFileReader(pmxFileName);
	model = pmxFileReader.readFile();

	// VMD�t�@�C���̓���
	pmx::VmdFileReader vmdFileReader(vmdFileName, model->getBones());
	vmdDataStream = vmdFileReader.readFile();

	// ���U�t���[���̃{�[���X�g���[�����Z�o
	//vector<pmx::Bone> initialBones = model->getBones();
	initialBones = model->getBones();
	vmdDataStream->calcStream(boneStream, vertexStream, model->getBones(), model->getVertices());

	glGenTextures(textureNum, texname);
	int textureNum = model->getTextureNum();
	cout << textureNum << endl;

	for (int i = 0; i < model->getTextureNum(); ++i) {
		glBindTexture(GL_TEXTURE_2D, texname[i]);
		cv::Mat texture = model->getTexture(i);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols, texture.rows, 0,
			GL_RGB, GL_UNSIGNED_BYTE, &texture.data[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	cout << "hoge\n" << endl;

	boneItr = boneStream.getBoneListMap().begin();
	vertexItr = vertexStream.getVertexListMap().begin();

	// �\��
	glutMainLoop();

	return 0;
}
*/
