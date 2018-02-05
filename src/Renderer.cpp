#include <GL/glew.h>
#include <GL/glut.h>
#include "Renderer.h"

using namespace mmd;

GLuint texname[17];


void display_glut() {
    Renderer::getInstance().display();
}


void mouse_glut(int button, int state, int x, int y) {
    Renderer::getInstance().mouse(button, state, x, y);
}


void motion_glut(int x, int y) {
    Renderer::getInstance().motion(x, y);
}


void resize_glut(int w, int h) {
    Renderer::getInstance().resize(w, h);
}


void idle_glut() {
    Renderer::getInstance().idle();
}


Renderer::Renderer()
        : currentFrameNo_(0)
        , width_(960)
        , height_(640)
        , mouse_l_(0)
        , mouse_m_(0)
        , mouse_r_(0)
        , theta_({0, 0, 0})
        , angle_({0, 0, 0}) {

    // GLUT
    int argc;
    glutInit(&argc, NULL);
    glutInitWindowSize(width_, height_);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("");
    glutDisplayFunc(display_glut);
    glutMouseFunc(mouse_glut);
    glutMotionFunc(motion_glut);
    glutReshapeFunc(resize_glut);
    glutIdleFunc(idle_glut);
    glClearColor(0.0, 0.0, 1.0, 0.0);

    // GLEW
    glewInit();
}


Renderer::~Renderer() {
}


Renderer& Renderer::getInstance() {
    static Renderer renderer;
    return renderer;
}


void Renderer::setParam(const PmxModel& model, const vector<Motion>& motions,
              const vector<vector<mmd::Motion>>& boneMotions) {
    model_ = model;
    motions_ = motions;
    boneMotions_ = boneMotions;

    // glTexture
    glGenTextures(model_.getTextureNum(), texname);
    for (int i = 0; i < model_.getTextureNum(); ++i) {
        glBindTexture(GL_TEXTURE_2D, texname[i]);
        cv::Mat texture = model_.getTexture(i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols, texture.rows, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &texture.data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    bones_ = model_.getBones();
    superParentIndex_ = searchSuperParentBone(bones_);
    vertices_ = model_.getVertices();

    // vector<Motion>::iteratorを先頭にセット
    boneMotionsItrs_.resize(boneMotions_.size());
    for (unsigned int i = 0; i < boneMotions_.size(); ++i) {
        boneMotionsItrs_[i] = boneMotions_[i].begin();
    }
}


void Renderer::start() {
    glutMainLoop();
}


void Renderer::update()
{
    cout << currentFrameNo_ << endl;

    vector<mmd::Motion> frameMotions(bones_.size());
    for (unsigned int i = 0; i < frameMotions.size(); ++i) {
        if (boneMotionsItrs_[i]->getFrameNo() == currentFrameNo_) {
            frameMotions[i] = *boneMotionsItrs_[i];
        } else if (distance(boneMotionsItrs_[i], boneMotions_[i].begin()) != boneMotions_[i].size() - 1 && (boneMotionsItrs_[i]+1)->getFrameNo() == currentFrameNo_) {
            frameMotions[i] = *(++(boneMotionsItrs_[i]));
        } else if (boneMotionsItrs_[i]->getFrameNo() < currentFrameNo_ && distance(boneMotionsItrs_[i], boneMotions_[i].begin()) != boneMotions_[i].size() - 1) {
            float ratio = float(currentFrameNo_ - boneMotionsItrs_[i]->getFrameNo()) /
                          ((boneMotionsItrs_[i]+1)->getFrameNo() - boneMotionsItrs_[i]->getFrameNo());
            Eigen::Vector3f shift = boneMotionsItrs_[i]->getShift() * (1.0f - ratio) + (boneMotionsItrs_[i]+1)->getShift() * ratio;
            Eigen::Quaternionf quaternion = boneMotionsItrs_[i]->getQuaternion().slerp(
                    ratio, (boneMotionsItrs_[i]+1)->getQuaternion());
            frameMotions[i] = mmd::Motion(i, currentFrameNo_, shift, quaternion);
        } else if (boneMotionsItrs_[i]->getFrameNo() < currentFrameNo_ && distance(boneMotionsItrs_[i], boneMotions_[i].begin()) == boneMotions_[i].size() - 1) {
            frameMotions[i] = *boneMotionsItrs_[i];
        } else {
            cout << "error\n";
        }
    }

    bones_[superParentIndex_].setTemporalPosition(
            bones_[superParentIndex_].getInitialPosition() + frameMotions[superParentIndex_].getShift());
    bones_[superParentIndex_].setTemporalQuaternion(frameMotions[superParentIndex_].getQuaternion());

    moveChildBones(bones_, superParentIndex_, frameMotions);

    vector<mmd::Vertex> initialVertices = model_.getVertices();
    for (unsigned int i = 0; i < initialVertices.size(); ++i) {
        vector<int> refBoneIndices = initialVertices[i].getRefBoneIndices();
        vector<float> refBoneWeightList = initialVertices[i].getRefBoneWeightList();
        Eigen::Vector3f pos(0, 0, 0);
        for (unsigned int b = 0; b < refBoneIndices.size(); ++b) {

            int boneIndex = refBoneIndices[b];

            // 移動後頂点の位置 = 移動後ボーンの回転 * (移動前頂点の位置 - 移動前ボーンの位置) + 移動後ボーンの位置
            pos += (bones_[boneIndex].getTemporalQuaternion() *
                    (initialVertices[i].getPosition() - bones_[boneIndex].getInitialPosition()) +
                    bones_[boneIndex].getTemporalPosition()) *
                   refBoneWeightList[b];
        }
        // refBoneIndices.size()で割る必要があるのでは？
        vertices_[i].setPosition(pos);
    }
}


void Renderer::display() {

    glViewport(0, 0, width_, height_);
    glLoadIdentity();
    gluPerspective(15.0, (double) width_ / (double) height_, 1.0, 1000.0);
    gluLookAt(-0.0, 70.0, -100.0, -8.0, 10.0, 0.0, 0.0, 1.0, 0.0);

    glRotated(angle_[0], 1.0, 0.0, 0.0);
    glRotated(angle_[1], 0.0, 1.0, 0.0);
    glRotated(angle_[2], 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    int surfaceNo = 0;
    for (int i = 0; i < model_.getMaterialNum(); ++i) {
        int ordinaryTextureIndex = model_.getMaterial(i).getOrdinaryTextureIndex();
        glBindTexture(GL_TEXTURE_2D, texname[ordinaryTextureIndex]);

        glBegin(GL_TRIANGLES);
        int surfaceNum = model_.getMaterial(i).getSurfaceNum();
        for (int s = 0; s < surfaceNum; ++s) {
            mmd::TriangleSurface triangleSurface = model_.getSurface(surfaceNo);
            Eigen::Vector3i vertexIndexies = triangleSurface.getVertexIndexies();
            for (int j = 0; j < 3; ++j) {
                mmd::Vertex vertex = vertices_[vertexIndexies[j]];
                glTexCoord2f(vertex.getUv().x(), vertex.getUv().y());
                glVertex3f(vertex.getPosition().x(), vertex.getPosition().y(), vertex.getPosition().z());
            }
            ++surfaceNo;
        }
        glEnd();
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glPointSize(2);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    int boneNum = model_.getBoneNum();
    for (int b = 0; b < boneNum; ++b) {
        Eigen::Vector3f pos = bones_[b].getTemporalPosition();
        glVertex3f(pos.x(), pos.y(), pos.z());
    }
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2);
    glBegin(GL_LINES);
    for (int b = 0; b < boneNum; ++b) {
        mmd::Bone bone = bones_[b];

        Eigen::Vector3f pos = bone.getTemporalPosition();
        glVertex3f(pos.x(), pos.y(), pos.z());

        Eigen::Vector3f destination;
        if (bone.getDestinationFlag()) {
            int destinationBoneIndex = bone.getDestinationBoneIndex();
            if (destinationBoneIndex < 0) {
                destination = pos;
            }
            else {
                destination = bones_[destinationBoneIndex].getTemporalPosition();
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


void Renderer::mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                mouse_pos_[0] = x;
                mouse_pos_[1] = y;
                mouse_l_ = 1;
            }
            if (state == GLUT_UP) {
                mouse_l_ = 0;
            }
            break;
        default:
            break;
    }
}


void Renderer::motion(int x, int y) {
    if (mouse_l_ == 1) {
        theta_[0] = (double) (y - mouse_pos_[1]) / 5.0;
        theta_[1] = (double) (x - mouse_pos_[0]) / 5.0;
    }
    if (mouse_l_ == 1 || mouse_m_ == 1 || mouse_r_ == 1) {
        mouse_pos_[0] = x;
        mouse_pos_[1] = y;
        angle_[0] += theta_[0];
        angle_[1] += theta_[1];
        glutPostRedisplay();
    }
}


void Renderer::resize(int w, int h) {
    width_ = w;
    height_ = h;
}


void Renderer::idle() {
    int time_now = glutGet(GLUT_ELAPSED_TIME);
    if (time_now - time_base_ > 33)
    {
        update();
        glutPostRedisplay();
        ++currentFrameNo_;
        time_base_ = time_now;
    }
}


int Renderer::searchSuperParentBone(const vector<mmd::Bone> &bones) {
    for (unsigned int boneIndex = 0; boneIndex < bones.size(); ++boneIndex) {
        if (bones[boneIndex].getParentBoneIndex() == -1) {
            return boneIndex;
        }
    }
    return -1;
};


void Renderer::moveChildBones(vector<mmd::Bone> &bones, const int parentBoneIndex,
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


