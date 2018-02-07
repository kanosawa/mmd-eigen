cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O0 -g")

include_directories(/usr/include/eigen3)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

add_executable(mmd-eigen src/Main.cpp src/PmxFileReader.cpp src/VmdFileReader.cpp
        src/MultiCharaCodeFileReader.cpp src/Vertex.cpp src/PmxModel.cpp
        src/Material.cpp src/Bone.cpp src/Motion.cpp src/Renderer.cpp
        src/MotionStream.cpp)

target_link_libraries(mmd-eigen ${OpenCV_LIBS} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLEW_LIBRARY})
