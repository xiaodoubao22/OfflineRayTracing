cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(PathTracing)

# 三方库包含路径
message(CMAKE_C_COMPILER_ID)
set(THIRD_PARTY ${CMAKE_SOURCE_DIR}/../third_party)
if(MINGW)
    set(ASSIMP_DIR ${THIRD_PARTY}/assimp-5.1.6-mingw)
    set(GLM_DIR ${THIRD_PARTY}/glm)
elseif(MSVC)
    set(ASSIMP_DIR ${THIRD_PARTY}/assimp-5.1.6-msvc)
    set(GLM_DIR ${THIRD_PARTY}/glm)
endif()
include_directories(${ASSIMP_DIR}/include)
include_directories(${GLM_DIR})
include_directories(${THIRD_PARTY})

# 设置cpu_engin头文件路径
set(CPU_ENGIN_INC_DIR ${CMAKE_SOURCE_DIR}/cpu_engin/Inc)
set(CPU_ENGIN_INC_FILES
    ${CPU_ENGIN_INC_DIR}/BoundingBox.h
    ${CPU_ENGIN_INC_DIR}/BvhTree.h
    ${CPU_ENGIN_INC_DIR}/HitResult.h
    ${CPU_ENGIN_INC_DIR}/LdsGenerator.h
    ${CPU_ENGIN_INC_DIR}/Material.h
    ${CPU_ENGIN_INC_DIR}/MaterialCookTorrance.h
    ${CPU_ENGIN_INC_DIR}/MaterialDefuse.h
    ${CPU_ENGIN_INC_DIR}/MaterialFrostedGlass.h
    ${CPU_ENGIN_INC_DIR}/MaterialSpecular.h
    ${CPU_ENGIN_INC_DIR}/MaterialTransparent.h
    ${CPU_ENGIN_INC_DIR}/Mesh.h
    ${CPU_ENGIN_INC_DIR}/Randerer.h
    ${CPU_ENGIN_INC_DIR}/Scene.h
    ${CPU_ENGIN_INC_DIR}/Shape.h
    ${CPU_ENGIN_INC_DIR}/Sphere.h
    ${CPU_ENGIN_INC_DIR}/TexureSampler.h
    ${CPU_ENGIN_INC_DIR}/Triangle.h
    ${CPU_ENGIN_INC_DIR}/Utils.h
    )

# 设置cpu_engin源文件路径
set(CPU_ENGIN_SRC_DIR ${CMAKE_SOURCE_DIR}/cpu_engin/Src)
set(CPU_ENGIN_SRC_FILES
    ${CPU_ENGIN_SRC_DIR}/BoundingBox.cpp
    ${CPU_ENGIN_SRC_DIR}/BvhTree.cpp
    ${CPU_ENGIN_SRC_DIR}/LdsGenerator.cpp
    ${CPU_ENGIN_SRC_DIR}/Material.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialCookTorrance.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialDefuse.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialFrostedGlass.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialSpecular.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialTransparent.cpp
    ${CPU_ENGIN_SRC_DIR}/Mesh.cpp
    ${CPU_ENGIN_SRC_DIR}/Randerer.cpp
    ${CPU_ENGIN_SRC_DIR}/Scene.cpp
    ${CPU_ENGIN_SRC_DIR}/Shape.cpp
    ${CPU_ENGIN_SRC_DIR}/Sphere.cpp
    ${CPU_ENGIN_SRC_DIR}/TexureSampler.cpp
    ${CPU_ENGIN_SRC_DIR}/Triangle.cpp
    ${CPU_ENGIN_SRC_DIR}/Utils.cpp
    )

# 包含目录
include_directories(${CPU_ENGIN_INC_DIR})

# 生成exe
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${CPU_ENGIN_INC_FILES} 
    ${CPU_ENGIN_SRC_FILES}
    )

if(MINGW)
    # 链接assimp静态库
    target_link_libraries(${PROJECT_NAME} ${ASSIMP_DIR}/lib/libassimp.dll.a)
    target_link_libraries(${PROJECT_NAME} ${ASSIMP_DIR}/bin/libassimp-5.dll)
elseif(MSVC)
    # 链接assimp静态库
    target_link_libraries(${PROJECT_NAME} ${ASSIMP_DIR}/lib/assimp-vc143-mt.lib)
    target_link_libraries(${PROJECT_NAME} -l${ASSIMP_DIR}/bin/assimp-vc143-mt.dll)
    # 设置启动项目
    set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()