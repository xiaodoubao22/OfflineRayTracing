# LearnRendering

## 简介

本项目为光线追踪渲染引擎，主要作为学习用途，目前正在开发中

## 效果图

### MicroFacet模型实现透明玻璃、磨砂玻璃、磨砂金属材质

![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/res1.png)

### 相同模型采用不同的环境光HDR贴图（spp=1024）

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

本项目可用MinGW或MSVC编译，用到了三方库：ASSIMP、GLM、stb、Qt、Vulkan

### Qt、Vulkan

正常从官网下载安装即可，建议Qt版本为5.14.2，建议安装Qt同时把MinGW选上

### 其他三方库

将其他三方库按要求放入third_party文件夹即可
ASSIMP建议用5.1.6

- third_party/assimp-5.1.6-mingw/include ASSIMP的头文件目录

- third_party/assimp-5.1.6-mingw/lib ASSIMP的静态库目录放于此目录

- third_party/assimp-5.1.6-mingw/bin ASSIMP的动态库目录放于此目录

- third_party/glm GLM的目录

- third_party/stb stb的目录

### 编译配置

scrips文件夹中为构建和编译脚本

- env_comfig.bat 环境配置文件，按照其中注释的要求修改目录即可
- build_debug_mingw.bat 用MinGW构建并编译Debug版本
- build_release_mingw.bat 用MinGW构建并编译Release版本
- build_sln.bat 用MSVC 17构建VS工程
- 其余脚本正在开发中，敬请期待
