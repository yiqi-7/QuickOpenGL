# 动手学 OpenGL：从窗口到交互式贴图立方体

本手册与 `OpenGL_Cpp_Quickstart_Guide.md` 配套。`src/` 目录提供按课程编号组织的练习壳文件，建议先读讲义对应章节，再按照文件头的目标、TODO 和验收标准逐步完成。原有完整示例保存在 Git 历史提交 `7a6d577`。

## 0. 项目路线

```text
00_window
  创建窗口、OpenGL Context、加载函数
       ↓
01_triangle
  顶点数据 -> VAO/VBO -> Shader -> Draw
       ↓
02_cube
  3D 顶点 -> MVP -> 深度测试 -> 立方体
       ↓
03_texture
  UV -> Texture -> PPM -> 纹理立方体
       ↓
04_interactive_cube
  Camera -> Mouse/Scroll -> Auto Rotate
       ↓
最终成果：可旋转、缩放、贴图的 3D 立方体
```

## 第 1 课：创建窗口与 OpenGL Context

**代码：** 本节的带注释窗口骨架
**目标：** 建立最小 OpenGL 程序。

关键步骤：

1. `glfwInit()` 初始化 GLFW。
2. `glfwWindowHint()` 请求 OpenGL 3.3 Core Profile。
3. `glfwCreateWindow()` 创建窗口。
4. `glfwMakeContextCurrent()` 将 Context 设为当前线程的 Context。
5. `gladLoadGL(glfwGetProcAddress)` 加载 OpenGL 函数。
6. `glClear()` 清屏，`glfwSwapBuffers()` 显示，`glfwPollEvents()` 处理事件。

最小窗口骨架如下，注释标出了初始化、创建 Context 和主循环的边界：

```cpp
#include <glad/gl.h>        // GLAD 声明和函数加载入口
#include <GLFW/glfw3.h>     // 窗口、Context 和输入事件

int main() {
    if (!glfwInit()) return 1; // 初始化 GLFW，失败时不要继续调用 GLFW API
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 请求 OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 请求 OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用现代 Core Profile

    GLFWwindow* window = glfwCreateWindow(900, 600, "OpenGL", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; } // 创建失败时释放 GLFW
    glfwMakeContextCurrent(window);             // 后续 GL 调用依赖这个当前 Context
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) return 1; // 加载函数指针

    while (!glfwWindowShouldClose(window)) {    // 每次循环渲染一帧
        glClearColor(0.08f, 0.10f, 0.14f, 1.0f); // 设置清屏颜色
        glClear(GL_COLOR_BUFFER_BIT);             // 清除颜色缓冲
        glfwSwapBuffers(window);                  // 显示后缓冲
        glfwPollEvents();                         // 处理窗口和输入事件
    }
    glfwDestroyWindow(window); // 销毁窗口资源
    glfwTerminate();            // 结束 GLFW
}
```

**观察：** 窗口标题和背景色；控制台输出的 OpenGL 版本。

**练习：** 修改窗口标题、尺寸和 `glClearColor()` 的颜色。

**验收：** 程序能够创建窗口，正常关闭，并打印 OpenGL 版本。

## 第 2 课：绘制第一个三角形

**代码：** 本节的带注释 VBO/VAO 与绘制片段
**目标：** 理解最小可编程渲染流程。

### 2.1 顶点数据

三个二维平面位置以三维向量形式提交：每个顶点包含 x、y、z。数据最初位于 CPU 内存，随后通过 `glBufferData()` 上传到 VBO。

### 2.2 顶点属性

`glVertexAttribPointer()` 告诉 OpenGL 如何解析缓冲区：

- 属性位置 0；
- 每个顶点 3 个 float；
- 不归一化；
- 每个顶点步长 3 个 float；
- 起始偏移为 0。

### 2.3 Shader

Vertex Shader 把位置写入 `gl_Position`；Fragment Shader 输出橙色。两个 Shader 编译并链接成 Program 后，通过 `glUseProgram()` 激活。

### 2.4 绘制

`glDrawArrays(GL_TRIANGLES, 0, 3)` 表示从第 0 个顶点开始，使用 3 个顶点绘制三角形。

**练习：** 修改三个顶点位置和片段颜色；尝试增加第四个顶点并绘制两个三角形。

下面展示顶点数据如何经过 VBO 上传、VAO 配置并发起绘制。Shader 需要先编译并链接为 `program`，这里用省略号表示该部分：

```cpp
const float vertices[] = { // 三个二维三角形顶点，每个位置用 xyz 表示
    -0.6f, -0.5f, 0.0f,
     0.6f, -0.5f, 0.0f,
     0.0f,  0.6f, 0.0f
};
GLuint vao = 0, vbo = 0;
glGenVertexArrays(1, &vao); // 创建顶点格式配置对象
glGenBuffers(1, &vbo);      // 创建顶点数据缓冲
glBindVertexArray(vao);      // 后续属性设置归这个 VAO 管理
glBindBuffer(GL_ARRAY_BUFFER, vbo); // 绑定要写入的顶点缓冲
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 上传顶点数据
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr); // 属性 0 每顶点 3 个 float
glEnableVertexAttribArray(0); // 启用顶点位置属性

// 绘制前还需 glUseProgram(program)，激活已链接的 Shader Program
glDrawArrays(GL_TRIANGLES, 0, 3); // 从顶点 0 取 3 个顶点组成一个三角形
```

**验收：** 能解释 VAO、VBO、Shader Program 和 `glDrawArrays()` 的关系。

## 第 3 课：绘制 3D 立方体

**代码：** 本节的带注释 MVP 与深度测试片段
**目标：** 理解三维顶点、MVP 与深度缓冲。

立方体有 6 个面，每个面由 2 个三角形组成，因此示例直接提交 36 个顶点。开启深度测试后，GPU 根据深度缓冲判断哪些片段应该被遮挡。

每帧构造三个矩阵：

- Model：示例中使用旋转矩阵。
- View：`glm::lookAt()` 构造观察矩阵。
- Projection：`glm::perspective()` 构造透视投影矩阵。

组合方式：`MVP = Projection * View * Model`。Vertex Shader 使用 MVP 将局部顶点变换到裁剪空间。

**练习：** 修改摄像机位置、投影视场角和旋转速度；注释 `glEnable(GL_DEPTH_TEST)` 观察遮挡异常。

下面展示每帧构造 MVP 并开启深度测试的核心代码。`program`、`vao` 和 `vertexCount` 由初始化阶段创建：

```cpp
glEnable(GL_DEPTH_TEST); // 启用深度比较，正确处理物体前后遮挡

const glm::mat4 model = glm::mat4(1.0f); // 模型矩阵：这里保持立方体在原点
const glm::mat4 view = glm::lookAt( // 观察矩阵：从摄像机位置看向目标点
    glm::vec3(2.0f, 2.0f, 3.0f), // 摄像机位置
    glm::vec3(0.0f),              // 观察目标
    glm::vec3(0.0f, 1.0f, 0.0f)  // 世界上方向
);
const glm::mat4 projection = glm::perspective( // 透视投影矩阵
    glm::radians(45.0f), aspect, 0.1f, 100.0f // 视场角、宽高比、近/远裁剪面
);
const glm::mat4 mvp = projection * view * model; // 列向量约定下从右向左变换

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 每帧同时清颜色与深度
// 将 mvp 上传给 Shader 的 uniform 后，绑定 VAO 并提交顶点绘制
```

**验收：** 能说明为什么 3D 场景需要深度测试，以及 MVP 三个矩阵的意义。

## 第 4 课：纹理映射

**代码：** 本节的带注释纹理与相机片段
**目标：** 理解顶点 UV、纹理对象和片段采样。

每个顶点由 5 个 float 组成：3 个位置分量和 2 个纹理坐标分量。VAO 分别配置 location 0 和 location 1。Vertex Shader 将 UV 传给 Fragment Shader，Fragment Shader 使用 `texture(uTexture, TexCoord)` 取样。

纹理上传流程：

1. 从 `assets/checker.ppm` 读取 P3 格式像素。
2. 创建并绑定 `GL_TEXTURE_2D`。
3. 设置 Wrap 和 Filter 参数。
4. 使用 `glTexImage2D()` 上传 RGB 数据。
5. 使用 `glGenerateMipmap()` 生成 Mipmap。
6. 绘制时激活纹理单元 0 并绑定纹理。

**练习：** 修改 PPM 颜色；尝试 `GL_REPEAT` 与 `GL_CLAMP_TO_EDGE`；改变 UV 范围。

片段着色器从纹理采样的最小示例：

```glsl
#version 330 core                         // 与 OpenGL 3.3 Core 对应
in vec2 TexCoord;                         // 接收顶点着色器插值后的 UV
out vec4 FragColor;                       // 输出当前片段颜色
uniform sampler2D uTexture;                // 由 CPU 绑定的二维纹理
void main() {
    FragColor = texture(uTexture, TexCoord); // 按 UV 从纹理读取颜色
}
```

上传纹理时，顶点的属性布局也要与 Shader 输入一致：

```cpp
constexpr GLsizei stride = 5 * sizeof(float); // 每顶点 xyzuv，共 5 个 float
// 位置从每个顶点开头读取 3 个 float
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
glEnableVertexAttribArray(0);
// UV 紧接在 xyz 之后，因此字节偏移是 3 个 float
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                      reinterpret_cast<void*>(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

**验收：** 六个立方体面都能显示纹理，纹理不会因为顶点属性步长错误而错位。

## 第 5 课：鼠标旋转与滚轮缩放

**代码：** 本节的带注释纹理与相机片段
**目标：** 把用户输入转换为摄像机参数。

### 5.1 鼠标旋转

左键按下时记录光标位置；移动时计算 dx/dy，更新 yaw/pitch。pitch 被限制在 [-89°, 89°]，避免视线方向与世界上方向向量接近平行。

### 5.2 滚轮缩放

滚轮改变摄像机到观察目标的距离。距离被限制在 [2, 9]，避免相机过近或过远。

### 5.3 摄像机位置

根据 yaw、pitch、distance 计算摄像机在球面轨道上的位置，再通过 `glm::lookAt()` 生成 View 矩阵。

### 5.4 其他按键

- `F`：切换自动旋转。
- `R`：重置观察角度和距离。
- `ESC`：退出。

**练习：** 调整鼠标灵敏度、缩放速度和距离范围；让自动旋转速度与帧率无关。

相机轨道位置可由 yaw、pitch 和距离计算。下面展示鼠标回调更新角度的简化代码：

```cpp
// cursorX/cursorY 是当前光标位置；lastX/lastY 保存上一次位置
const float dx = static_cast<float>(cursorX - lastX); // 水平位移
const float dy = static_cast<float>(cursorY - lastY); // 垂直位移
lastX = cursorX; // 更新基准点，下一次只计算增量
lastY = cursorY;
yaw += dx * sensitivity; // 水平拖动改变方位角
pitch -= dy * sensitivity; // 垂直拖动改变俯仰角
pitch = std::clamp(pitch, -89.0f, 89.0f); // 避免视线接近竖直时相机上方向退化

// 滚轮回调中限制相机距离，防止穿进模型或离得太远
distance = std::clamp(distance - static_cast<float>(yOffset) * zoomSpeed,
                      minDistance, maxDistance);
```

**验收：** 鼠标拖动能连续旋转立方体，滚轮能缩放，窗口改变大小后画面比例正常。

## 第 6 课：综合验收

逐项检查：

- [ ] 窗口正常创建，控制台能打印 OpenGL 版本。
- [ ] 立方体六个面均可见，前后遮挡正确。
- [ ] 纹理在六个面上正确显示。
- [ ] 鼠标左键拖动可以改变观察方向。
- [ ] 鼠标滚轮可以拉近和拉远。
- [ ] `F` 可以切换自动旋转。
- [ ] `R` 可以重置视角。
- [ ] `ESC` 可以正常退出。
- [ ] 调整窗口大小后 viewport 和投影宽高比正确。

## 常见错误排查

| 现象 | 常见原因 | 检查方向 |
|---|---|---|
| 窗口创建失败 | 驱动不支持所请求的 OpenGL 版本 | 检查 OpenGL 3.3 Core 支持 |
| GL 函数崩溃或为空 | GLAD 未成功加载，或 Context 未设为当前 | 检查 `glfwMakeContextCurrent` 和 `gladLoadGL` |
| Shader 编译失败 | GLSL 语法或版本不匹配 | 输出 Shader Info Log |
| 物体全黑/遮挡错误 | 深度测试未启用或深度缓冲未清除 | 检查 `GL_DEPTH_TEST` 和 `glClear` |
| 纹理错位 | 顶点 stride/offset/UV 不匹配 | 检查 5-float 顶点布局 |
| 纹理文件找不到 | 工作目录不在项目根目录 | 从项目根目录运行 |
| 窗口拉伸后模型变形 | viewport 或 projection aspect 未更新 | 使用 framebuffer 宽高比 |

## 进一步拓展

完成基础项目后，再按以下顺序扩展：

1. 每个立方体面使用不同纹理。
2. 增加模型平移、旋转和缩放键位。
3. 添加法线、方向光和 Phong 光照。
4. 使用 EBO 组织索引数据。
5. 使用 stb_image 读取 PNG/JPG。
6. 使用 Assimp 加载外部 3D 模型。
