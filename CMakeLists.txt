cmake_minimum_required(VERSION 3.5)
project(bane)

add_library(${PROJECT_NAME} 
  src/bane.cpp
  src/controllers/icontroller.cpp
  src/controllers/spectatorController.cpp
  src/components/camera.cpp
  src/components/bobject.cpp
  src/components/staticmesh.cpp
  src/utility/shader.cpp
  src/primitives/primitives.cpp
  src/primitives/mesh.cpp
  src/primitives/model.cpp
  src/primitives/animatedModel.cpp
  src/glad.c
  src/stb_image.c
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_link_directories(${PROJECT_NAME}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/build
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  glfw3
  assimp-vc143-mt
)

