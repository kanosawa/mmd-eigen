#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GL/glut.h>
#include "BoneStream.h"
#include "VertexStream.h"
#include "PmxFileReader.h"
#include "VmdFileReader.h"
#include "VmdDataStream.h"

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
    vector<mmd::Bone> bones = boneItr->second;
    vector<mmd::Vertex> vertices = vertexItr->second;

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

void Idle() {
    if (++boneItr == boneStream.getBoneListMap().end()) boneItr = boneStream.getBoneListMap().begin();
    if (++vertexItr == vertexStream.getVertexListMap().end()) vertexItr = vertexStream.getVertexListMap().begin();
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
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

    // PMXファイルの入力
    mmd::PmxFileReader pmxFileReader("maya.pmx");
    model = pmxFileReader.readFile();

    // VMDファイルの入力
    mmd::VmdFileReader vmdFileReader("maya.vmd", model->getBones());
    vmdDataStream = vmdFileReader.readFile();

    vmdDataStream->calcStream(boneStream, vertexStream, model->getBones(), model->getVertices());

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

    boneItr = boneStream.getBoneListMap().begin();
    vertexItr = vertexStream.getVertexListMap().begin();

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
