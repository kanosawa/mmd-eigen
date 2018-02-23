cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O3 -g")

include_directories(/usr/include/eigen3 include)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(mmd-eigen src/Main.cpp src/PmxModelFileReader.cpp src/VmdFileReader.cpp
        src/FileReader.cpp src/Vertex.cpp src/PmxModel.cpp src/Material.cpp src/Bone.cpp
        src/Motion.cpp src/Renderer.cpp src/MotionStream.cpp src/ModelUpdater.cpp src/Surface.cpp
        src/MotionStreamGenerator.cpp src/PmdModelFileReader.cpp src/ModelFileReader.cpp
        src/ModelFileReaderFactory.cpp)

target_link_libraries(mmd-eigen ${OpenCV_LIBS} ${OPENGL_LIBRARY} ${GLUT_LIBRARY})
