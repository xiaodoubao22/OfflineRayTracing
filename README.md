# LearnRendering

## 简介

一个在跑在CPU上的光线追踪离线绘制引擎，主要作为学习用途，目前正在开发中

## 效果图

### MicroFacet模型实现透明玻璃、磨砂玻璃、磨砂金属材质

![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/res1.png)

### 相同模型采用不同的环境光HDR贴图（spp=1024）

![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/res2.png)

### 在Cook-Torrance材质模型中使用Kulla-Conty近似能量守恒

注：第一排未补偿，第二排使用Kulla-Conty近似补偿，粗糙度从左至右分别为0.4、0.6、0.8、1.0

![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/knob.png)

### 白炉测试结果，第一行使用Kulla-Conty补偿，第二排未补偿

![image](https://raw.githubusercontent.com/xiaodoubao22/LearnRendering/main/figure/white_furnace.png)

## 模块分解

### CPU离线渲染器

多线程渲染器，实现蒙特卡洛光线追踪，模拟直接光照、间接光照、阴影、辉映、焦散等效果。

### 几何模块

构建BVH树、采用SAH加速，实现对三角形、Mesh网格、球体的射线求交算法。

### 材质系统

- 基础材质：<br/>

- [x] 漫反射
- [x] 完全镜面反射
- [x] 光滑玻璃

- 微表面模型：<br/>

- [x] GGX-BRDF(光滑/磨砂金属)
- [x] GGX-BSDF(磨砂玻璃)

- 弹射光线重要性采样 <br/>


### 调试工具

指定像素光线传播路径可视化、边界包围盒可视化

## 环境配置

本项目可用MinGW或MSVC编译，用到了三方库：ASSIMP、GLM、stb、Qt

### Qt

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
