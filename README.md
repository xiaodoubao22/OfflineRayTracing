# LearnRendering

## 简介

本项目为光线追踪渲染引擎，主要作为学习用途，目前正在开发中

## 效果图

### MicroFacet模型实现毛玻璃材质
![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/res1.png)

### 环境光IBL
![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/res2.png)

## 已实现特性

### 渲染算法及效果

蒙特卡洛光线追踪，实现直接光照、间接光照、阴影、辉映、焦散等效果。

### 几何图元

三角形、Mesh、球体

### 材质

漫反射、完全镜面反射、光滑玻璃、金属微表面模型(BRDF)、毛玻璃微表面模型(BTDF)

### 加速方法

BVH树、SAH加速、多线程加速

### 调试工具

指定像素光线传播路径可视化、边界包围盒可视化

## 环境配置

本项目需要用到三方库ASSIMP、GLM、SVPNG

### ASSIMP

- 下载assimp-5.2.5源码用CMake构建(64位)并用VS2022编译

- 得到assimp-vc143-mt.lib放入目录"LearnRendering/third_party/Libs/assimp/"

- 得到assimp-vc143-mt.dll放入目录"LearnRendering/third_party/Bin/assimp/"

- 在ASSIMP的"assimp-5.2.5/include"目录下找到assimp文件夹放入目录"LearnRendering/third_party/Includes"

- 在ASSIMP编译好后的"assimp-5.2.5/build/include/assimp"目录下找到config.h放入目录"LearnRendering/third_party/Includes/assimp"
  
  ### GLM

### SVPNG
