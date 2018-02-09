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
        : width_(960)
        , height_(640)
        , mouse_l_(0)
        , mouse_m_(0)
        , mouse_r_(0)
        , theta_({0, 0, 0})
        , angle_({0, 0, 0}) {

    int argc = 1;
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
}


Renderer::~Renderer() {
}


Renderer& Renderer::getInstance() {
    static Renderer renderer;
    return renderer;
}


void Renderer::setParam(const PmxModel& model, const vector<MotionStream>& motionStreams) {

    updater_.setParam(model, motionStreams);

    // glTexture
    glGenTextures(model.getTextureNum(), texname);
    for (int i = 0; i < model.getTextureNum(); ++i) {
        glBindTexture(GL_TEXTURE_2D, texname[i]);
        cv::Mat texture = model.getTexture(i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols, texture.rows, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &texture.data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}


void Renderer::start() {
    glutMainLoop();
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
    for (int i = 0; i < updater_.getModel().getMaterialNum(); ++i) {
        int ordinaryTextureIndex = updater_.getModel().getMaterial(i).getOrdinaryTextureIndex();
        glBindTexture(GL_TEXTURE_2D, texname[ordinaryTextureIndex]);

        glBegin(GL_TRIANGLES);
        int surfaceNum = updater_.getModel().getMaterial(i).getSurfaceNum();
        for (int s = 0; s < surfaceNum; ++s) {
            mmd::Surface surface = updater_.getModel().getSurface(surfaceNo);
            Eigen::Vector3i vertexIndexies = surface.getVertexIndexies();
            for (int j = 0; j < 3; ++j) {
                mmd::Vertex vertex = updater_.getModel().getVertices()[vertexIndexies[j]];
                glTexCoord2f(vertex.getUv().x(), vertex.getUv().y());
                glVertex3f(vertex.getTemporalPosition().x(), vertex.getTemporalPosition().y(), vertex.getTemporalPosition().z());
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
    int boneNum = updater_.getModel().getBoneNum();
    for (int b = 0; b < boneNum; ++b) {
        Eigen::Vector3f pos = updater_.getModel().getBones()[b].getTemporalPosition();
        glVertex3f(pos.x(), pos.y(), pos.z());
    }
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2);
    glBegin(GL_LINES);
    for (int b = 0; b < boneNum; ++b) {
        mmd::Bone bone = updater_.getModel().getBones()[b];

        Eigen::Vector3f pos = bone.getTemporalPosition();
        glVertex3f(pos.x(), pos.y(), pos.z());

        Eigen::Vector3f destination;
        if (bone.getDestinationFlag()) {
            int destinationBoneIndex = bone.getDestinationBoneIndex();
            if (destinationBoneIndex < 0) {
                destination = pos;
            }
            else {
                destination = updater_.getModel().getBones()[destinationBoneIndex].getTemporalPosition();
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
        updater_.update();
        glutPostRedisplay();
        time_base_ = time_now;
    }
}
