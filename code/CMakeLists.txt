cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# [Qt]设置moc rcc uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 项目
project(PathTracing)
message(STATUS ${CMAKE_C_COMPILER_ID})

# 设置cmake前缀路径
set(CMAKE_PREFIX_PATH 
    ${Qt5_PREFIX_PATH}    # Qt5前缀路径由外部指定
)

# [third_party]设置三方库包含路径
set(THIRD_PARTY ${CMAKE_SOURCE_DIR}/../third_party)
if(MINGW)
    set(ASSIMP_DIR ${THIRD_PARTY}/assimp-5.1.6-mingw)
    set(GLM_DIR ${THIRD_PARTY}/glm)
elseif(MSVC)
    set(ASSIMP_DIR ${THIRD_PARTY}/assimp-5.1.6-msvc)
    set(GLM_DIR ${THIRD_PARTY}/glm)
endif()

# [cpu_engin]设置头文件路径
set(CPU_ENGIN_INC_DIR ${CMAKE_SOURCE_DIR}/cpu_engin/Inc)
set(CPU_ENGIN_INC_FILES
    ${CPU_ENGIN_INC_DIR}/BoundingBox.h
    ${CPU_ENGIN_INC_DIR}/BvhTree.h
    ${CPU_ENGIN_INC_DIR}/Camera.h
    ${CPU_ENGIN_INC_DIR}/HitResult.h
    ${CPU_ENGIN_INC_DIR}/LdsGenerator.h
    ${CPU_ENGIN_INC_DIR}/Material.h
    ${CPU_ENGIN_INC_DIR}/MaterialCookTorrance.h
    ${CPU_ENGIN_INC_DIR}/MaterialDefuse.h
    ${CPU_ENGIN_INC_DIR}/MaterialFrostedGlass.h
    ${CPU_ENGIN_INC_DIR}/MaterialSpecular.h
    ${CPU_ENGIN_INC_DIR}/MaterialSet.h
    ${CPU_ENGIN_INC_DIR}/MaterialTransparent.h
    ${CPU_ENGIN_INC_DIR}/Mesh.h
    ${CPU_ENGIN_INC_DIR}/Renderer.h
    ${CPU_ENGIN_INC_DIR}/Scene.h
    ${CPU_ENGIN_INC_DIR}/Shape.h
    ${CPU_ENGIN_INC_DIR}/ShapeSet.h
    ${CPU_ENGIN_INC_DIR}/Sphere.h
    ${CPU_ENGIN_INC_DIR}/TexureSampler.h
    ${CPU_ENGIN_INC_DIR}/Triangle.h
    ${CPU_ENGIN_INC_DIR}/Utils.h
    )
# [cpu_engin]设置源文件路径
set(CPU_ENGIN_SRC_DIR ${CMAKE_SOURCE_DIR}/cpu_engin/Src)
set(CPU_ENGIN_SRC_FILES
    ${CPU_ENGIN_SRC_DIR}/BoundingBox.cpp
    ${CPU_ENGIN_SRC_DIR}/BvhTree.cpp
    ${CPU_ENGIN_SRC_DIR}/Camera.cpp
    ${CPU_ENGIN_SRC_DIR}/LdsGenerator.cpp
    ${CPU_ENGIN_SRC_DIR}/Material.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialCookTorrance.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialDefuse.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialFrostedGlass.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialSpecular.cpp
    ${CPU_ENGIN_SRC_DIR}/MaterialTransparent.cpp
    ${CPU_ENGIN_SRC_DIR}/Mesh.cpp
    ${CPU_ENGIN_SRC_DIR}/Renderer.cpp
    ${CPU_ENGIN_SRC_DIR}/Scene.cpp
    ${CPU_ENGIN_SRC_DIR}/Shape.cpp
    ${CPU_ENGIN_SRC_DIR}/Sphere.cpp
    ${CPU_ENGIN_SRC_DIR}/TexureSampler.cpp
    ${CPU_ENGIN_SRC_DIR}/Triangle.cpp
    ${CPU_ENGIN_SRC_DIR}/Utils.cpp
    )

# [hmi]设置头文件路径
set(HMI_INC_DIR ${CMAKE_SOURCE_DIR}/hmi/Inc)
set(HMI_INC_FILES
    ${HMI_INC_DIR}/SceneReader.h
)
# [hmi]设置源文件路径
set(HMI_SRC_DIR ${CMAKE_SOURCE_DIR}/hmi/Src)
set(HMI_SRC_FILES
    ${HMI_SRC_DIR}/SceneReader.cpp
)

# [third_party][hmi][cpu_engin]包含目录
include_directories(${THIRD_PARTY} ${ASSIMP_DIR}/include ${GLM_DIR})
include_directories(${CPU_ENGIN_INC_DIR} ${HMI_INC_DIR})

# 生成exe
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${CPU_ENGIN_INC_FILES} 
    ${CPU_ENGIN_SRC_FILES}
    ${HMI_INC_FILES}
    ${HMI_SRC_FILES}
    )
# [vs]设置启动项目
if(MSVC)
    set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

if(MINGW)
    # [third_party]链接assimp静态库
    target_link_libraries(${PROJECT_NAME} 
        ${ASSIMP_DIR}/lib/libassimp.dll.a
        ${ASSIMP_DIR}/bin/libassimp-5.dll
        )
elseif(MSVC)
    # [third_party]链接assimp静态库
    target_link_libraries(${PROJECT_NAME} 
        ${ASSIMP_DIR}/lib/assimp-vc143-mt.lib
        -l${ASSIMP_DIR}/bin/assimp-vc143-mt.dll
        )
endif()

# [Qt]查找Qt库
find_package(Qt5 COMPONENTS widgets REQUIRED)
find_package(Qt5 COMPONENTS xml REQUIRED)
find_package(Qt5 COMPONENTS core REQUIRED)
# [Qt]链接Qt库
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Xml Qt5::Core)
