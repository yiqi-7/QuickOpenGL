# OpenGL C++ 快速入门讲义

**目标读者：** 具备基本 C++ 语法，希望快速理解实时 3D 渲染并完成一个可交互小项目的学习者。  
**建议课时：** 6～8 小时。  
**开发环境：** VS Code、C++17、CMake 3.20+、Python 3、OpenGL 3.3 Core Profile。  
**配套代码：** `src/00_window.cpp`、`src/01_triangle.cpp`、`src/02_cube.cpp`、`src/04_interactive_cube.cpp`。

> 学习目标不是记住所有 OpenGL API，而是理解一条最小渲染链路：创建上下文、提交顶点数据、配置顶点属性、编写 Shader、执行绘制、处理输入并更新相机。

---

## 学习地图

| 章节 | 对应代码 | 学习结果 |
|---|---|---|
| 1. OpenGL 定位 | 全部 | 理解 OpenGL、GPU、窗口系统的关系 |
| 2. 渲染管线 | `01_triangle.cpp` | 理解顶点着色器到片段着色器 |
| 3. VAO/VBO | `01_triangle.cpp` | 把顶点数据交给 GPU |
| 4. 3D 与深度测试 | `02_cube.cpp` | 绘制立方体并正确处理前后遮挡 |
| 5. MVP 与摄像机 | `02_cube.cpp` | 理解模型、观察、投影变换 |
| 6. 纹理映射 | `04_interactive_cube.cpp` | 把二维图像映射到立方体表面 |
| 7. 交互控制 | `04_interactive_cube.cpp` | 鼠标拖动旋转、滚轮缩放 |
| 8. 综合项目 | `04_interactive_cube.cpp` | 完成最终交互式 3D 立方体 |

# 第一章 OpenGL 是什么

## 1.1 定义

OpenGL 是一套跨平台的图形渲染 API 规范，规定应用程序如何向图形系统提交顶点、纹理、状态和绘制命令。具体函数由显卡驱动提供实现，GPU 执行大量并行图形计算。

OpenGL 不是编程语言，也不是完整游戏引擎。C++ 是编写应用程序的语言；GLFW 负责窗口和输入；GLAD 负责加载 OpenGL 函数（GLAD 2 的生成步骤需要 Python 3）；GLM 提供数学运算；OpenGL 负责渲染接口。

## 1.2 OpenCV 与 OpenGL 的区别

| 技术 | 主要任务 | 例子 |
|---|---|---|
| OpenCV | 图像处理和计算机视觉 | 滤波、边缘检测、目标识别 |
| OpenGL | 图形渲染 | 显示纹理、绘制 3D 模型、相机变换 |
| GLFW | 窗口、输入和 OpenGL Context | 创建窗口、读取鼠标滚轮 |
| GLAD | OpenGL 函数加载 | 加载 `glCreateShader` 等函数入口 |
| GLM | 图形学数学库 | 向量、矩阵、透视投影 |

二者可以结合：OpenCV 读取或处理图像，OpenGL 将图像作为纹理显示在 3D 场景中。

## 1.3 为什么需要 GPU 渲染

CPU 适合组织流程、处理复杂控制逻辑；GPU 擅长对大量顶点和像素执行相似操作。图形渲染把顶点变换、三角形光栅化、像素着色等任务交给 GPU 并行完成。

# 第二章 现代 OpenGL 渲染管线

本项目使用 **OpenGL 3.3 Core Profile**，即现代可编程管线。它与早期固定管线不同：顶点如何变换、片段如何着色，主要由开发者编写 Shader 指定。

```text
CPU 提交顶点/纹理/状态
          ↓
      Vertex Shader       顶点坐标变换
          ↓
      图元装配             顶点组成三角形
          ↓
       光栅化              三角形覆盖哪些像素
          ↓
     Fragment Shader      计算片段颜色
          ↓
 深度/模板/混合等测试
          ↓
      Framebuffer         输出到窗口
```

### 2.1 Vertex Shader

顶点着色器按顶点执行，通常负责把模型局部坐标变换到裁剪空间，也可以传递纹理坐标、法线等属性。

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
```

`layout(location=0)` 表示顶点属性位置 0；`aPos` 是输入顶点位置；`gl_Position` 是必须写入的裁剪空间位置。

### 2.2 Fragment Shader

片段着色器通常为光栅化产生的每个片段计算颜色。

```glsl
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.4, 0.2, 1.0);
}
```

本项目最终版本会从纹理中采样颜色，而不是输出固定颜色。

### 2.3 Shader Program

Vertex Shader 和 Fragment Shader 分别编译后，需要链接成一个 Shader Program。绘制前通过 `glUseProgram(program)` 激活程序。

编译成功不代表链接一定成功。工程代码应分别检查 `GL_COMPILE_STATUS` 和 `GL_LINK_STATUS`，并打印日志定位错误。

# 第三章 顶点数据：VAO、VBO、EBO

## 3.1 VBO：Vertex Buffer Object

VBO 是 GPU 侧缓冲对象，用来存储顶点属性数据。典型流程：

1. `glGenBuffers()` 创建缓冲对象。
2. `glBindBuffer(GL_ARRAY_BUFFER, vbo)` 绑定。
3. `glBufferData()` 将 CPU 数据上传到 GPU。

```cpp
GLuint vbo = 0;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

`GL_STATIC_DRAW` 表示数据通常由 CPU 设置、GPU 多次读取；它是使用意图提示，不是强制存储策略。

## 3.2 VAO：Vertex Array Object

VAO 保存顶点属性如何从当前缓冲数据中读取的配置，例如属性位置、分量数量、数据类型、步长和偏移量。

```cpp
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                      3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

可以把 VBO 理解为“数据本身”，VAO 理解为“如何解释这些数据”。绘制前绑定对应 VAO 即可恢复其顶点属性配置。

## 3.3 EBO：Element Buffer Object

EBO 保存顶点索引，用于复用顶点，减少重复数据。通过 `glDrawElements()` 按索引绘制。本项目为了让每个立方体面使用独立纹理坐标，使用 36 个三角形顶点并调用 `glDrawArrays()`，所以没有强制使用 EBO。

## 3.4 顶点属性布局

最终立方体每个顶点包含 5 个 float：

```text
[x, y, z, u, v]
 └位置┘ └纹理坐标┘
```

因此步长是 `5 * sizeof(float)`；位置属性从偏移 0 开始，纹理坐标从偏移 `3 * sizeof(float)` 开始。

# 第四章 从 2D 到 3D：坐标与深度测试

## 4.1 坐标空间

常见坐标空间转换链路：

```text
模型局部空间 -> 世界空间 -> 观察空间 -> 裁剪空间 -> 屏幕空间
```

- **局部/模型空间：** 顶点相对于模型自身原点的位置。
- **世界空间：** 模型放入场景后的坐标。
- **观察空间：** 从摄像机视角表达场景。
- **裁剪空间：** 投影变换后的齐次坐标，后续进行裁剪和透视除法。
- **屏幕空间：** 映射到窗口像素坐标。

## 4.2 MVP 矩阵

MVP 是 Model、View、Projection 三个矩阵的组合：


a) Model：模型如何在世界中平移、旋转、缩放。  
 b) View：摄像机位于哪里、朝向哪里。  
 c) Projection：如何把 3D 场景投影到 2D 屏幕。

常用列向量约定下：

\[
P_{clip}=P\,V\,M\,P_{local}
\]

代码中写作 `projection * view * model * position`。矩阵乘法顺序不能随意交换。

## 4.3 透视投影

透视投影模拟近大远小。GLM 中常用：

```cpp
glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
```

参数依次是垂直视场角、宽高比、近裁剪面和远裁剪面。宽高比应根据 framebuffer 实际尺寸计算，窗口尺寸变化时需要更新 viewport。

## 4.4 深度测试

3D 物体的三角形可能互相遮挡。只按绘制顺序覆盖颜色是不够的，需要深度缓冲记录像素深度。

```cpp
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

若忘记清除深度缓冲，上一帧的深度信息可能影响下一帧。创建窗口时 GLFW 默认通常会提供深度缓冲，但应确保渲染循环正确清除它。

# 第五章 纹理映射

## 5.1 纹理是什么

纹理（Texture）是 GPU 侧图像数据。模型顶点携带纹理坐标 UV，片段着色器根据 UV 从纹理中采样颜色。

纹理坐标通常用二维坐标表示：左下附近为 (0,0)，右上附近为 (1,1)（实际图像数据的行方向还受解码器和上传方式影响）。

## 5.2 纹理坐标

一个矩形面通常由两个三角形组成。四个角可以使用：

| 顶点位置 | UV |
|---|---|
| 左下 | (0,0) |
| 右下 | (1,0) |
| 右上 | (1,1) |
| 左上 | (0,1) |

每个面要提供正确的顶点顺序和 UV。立方体有六个面，最终示例为每个面分别提供两个三角形。

## 5.3 纹理对象与采样

基本流程：

1. 读取图片像素。
2. `glGenTextures()` 创建纹理对象。
3. 绑定 `GL_TEXTURE_2D`。
4. 设置环绕和过滤参数。
5. `glTexImage2D()` 上传像素。
6. `glGenerateMipmap()` 生成多级渐远纹理。
7. 绘制时绑定纹理单元，Shader 通过 sampler 采样。

常见参数：

- `GL_TEXTURE_WRAP_S/T`：UV 超出 [0,1] 时如何处理，如重复或边缘夹取。
- `GL_TEXTURE_MIN_FILTER`：纹理缩小时的采样方式。
- `GL_TEXTURE_MAG_FILTER`：纹理放大时的采样方式。
- Mipmap：为不同缩小尺度预先生成不同分辨率层级，减少远处纹理闪烁。

本项目使用 PPM P3 纹理文件和自定义轻量读取函数，目的是避免初学阶段引入额外图片库。实际项目可以使用 stb_image 等图片加载库。

# 第六章 摄像机与鼠标交互

## 6.1 轨道摄像机

最终项目没有移动立方体本身，而是让摄像机围绕原点运动。用三个参数控制摄像机：

- yaw：水平角度。
- pitch：垂直角度。
- distance：摄像机到目标点的距离。

通过球坐标转换为摄像机位置：

\[
\begin{aligned}
x &= r\cos(\phi)\sin(\theta)\\
y &= r\sin(\phi)\\
z &= r\cos(\phi)\cos(\theta)
\end{aligned}
\]

其中 r 是距离，θ 是 yaw，φ 是 pitch。pitch 需要限制在接近 ±90° 的范围内，避免观察方向与上方向向量接近平行导致视图矩阵不稳定。

## 6.2 鼠标拖动旋转

GLFW 鼠标回调提供鼠标按键状态和光标位置。左键按下时记录起始位置；拖动过程中计算 `dx`、`dy`，分别更新 yaw 和 pitch。

```text
鼠标按下 -> 记录光标位置
鼠标移动 -> 计算位移 dx/dy
          -> 更新 yaw/pitch
          -> 下一帧重建 View 矩阵
鼠标松开 -> 停止更新
```

## 6.3 滚轮缩放

滚轮回调获得纵向滚动量，更新 distance。必须对距离做范围限制，避免摄像机进入模型内部或退得过远。

本项目的缩放是摄像机轨道半径变化，不是改变模型顶点坐标的 Scale 操作。两者视觉上都可能让物体变大或变小，但语义不同。

# 第七章 主循环与实时渲染

典型实时渲染循环：

```cpp
while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateMatrices();
    drawScene();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

- `glfwPollEvents()`：处理键盘、鼠标、窗口事件。
- `glfwSwapBuffers()`：交换前后缓冲，把已完成的一帧显示出来。
- `glClear()`：清理当前帧的颜色和深度缓冲。

OpenGL Context 通常要求在创建它的线程中调用 OpenGL 函数。不要在没有正确 Context 的线程里直接调用 GL API。

# 第八章 学习验收与拓展

完成项目后，应能解释并修改以下内容：

1. GLFW、GLAD、GLM、OpenGL 分别解决什么问题。
2. Vertex Shader 与 Fragment Shader 的职责。
3. VAO 与 VBO 的区别，顶点属性布局如何解释内存。
4. 深度测试解决什么问题，为什么每帧要清除深度缓冲。
5. Model、View、Projection 矩阵分别表示什么。
6. UV 如何把图片映射到立方体表面。
7. 鼠标位移如何转换成 yaw/pitch，滚轮如何改变摄像机距离。

建议拓展顺序：

- 更换 `assets/checker.ppm`，观察纹理映射变化。
- 给六个面使用不同纹理或不同 UV 区域。
- 增加按键控制模型平移和缩放。
- 增加简单方向光、法线和 Phong 光照。
- 使用 EBO 复用顶点，比较顶点组织方式。
- 引入 stb_image 加载 PNG/JPG。
- 使用 Assimp 导入 OBJ/FBX 等模型。
