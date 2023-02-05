cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(PathTracing)

set(THIRD_PARTY ${CMAKE_SOURCE_DIR}/third_party)
include_directories(${THIRD_PARTY}/Includes)

set(OFFLINE_RENDERING_INC
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/BoundingBox.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/BvhTree.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/HitResult.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Material.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/MaterialCookTorrance.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/MaterialDefuse.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/MaterialFrostedGlass.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/MaterialSpecular.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/MaterialTransparent.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Mesh.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Randerer.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Scene.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Shape.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Sphere.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Triangle.h
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc/Utils.h
    )

set(OFFLINE_RENDERING_SRC
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/BoundingBox.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/BvhTree.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/main.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Material.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/MaterialCookTorrance.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/MaterialDefuse.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/MaterialFrostedGlass.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/MaterialSpecular.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/MaterialTransparent.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Randerer.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Scene.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Shape.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Sphere.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Triangle.cpp
    ${CMAKE_SOURCE_DIR}/offline_rendering/Src/Utils.cpp
    )

set(INCLUDE_PATH
    ${CMAKE_SOURCE_DIR}/offline_rendering/Inc
    )
include_directories(${INCLUDE_PATH})

add_executable(${PROJECT_NAME} ${OFFLINE_RENDERING_SRC} ${OFFLINE_RENDERING_INC})
target_link_libraries(${PROJECT_NAME} ${THIRD_PARTY}/libs/assimp/assimp-vc143-mt.lib)
#target_link_libraries(${PROJECT_NAME} -l${THIRD_PARTY}/Bin/assimp/assimp-vc143-mt.dll)


