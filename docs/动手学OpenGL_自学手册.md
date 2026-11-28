# 动手学 OpenGL：从窗口到交互式贴图立方体

本手册与 `OpenGL_Cpp_Quickstart_Guide.md` 配套。`src/` 中保留按课程编号组织的练习框架，你需要把本手册对应课程的代码写入指定文件。每一课末尾都给出完整参考代码，卡住时可以整段对照，也可以只对照当前函数。

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

### 本课代码写在哪里

本课只修改：`src/01_triangle.cpp`。

请按下面顺序动手：

1. 打开 `src/01_triangle.cpp`，保留文件头注释和 `main()`。
2. 先把第 1 课的窗口初始化代码写入 `main()`，确认窗口可以创建。
3. 按“Shader 编译函数 → Shader Program 链接函数 → 顶点数组 → VAO/VBO → 主循环绘制”的顺序写代码。
4. 本课完整参考代码位于本文末尾的“附录 A”，复制时目标路径就是 `src/01_triangle.cpp`。
5. 写完后使用下面的命令编译和运行；如果 PowerShell 拦截脚本，保留 `ExecutionPolicy Bypass` 参数。

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 01_triangle -Run
```

不要把三角形代码写回 `src/00_window.cpp`；第 1 课负责窗口，第 2 课从 `src/01_triangle.cpp` 开始形成第一个可绘制目标。

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

### 本课代码写在哪里

本课只修改：`src/02_cube.cpp`。

`src/02_cube.cpp` 是本课唯一需要修改的文件。请按下面方向写：

1. 先把 `src/01_triangle.cpp` 中的窗口、Shader、VAO/VBO 结构复制到 `src/02_cube.cpp`。
2. 将三角形顶点数组替换为 36 个立方体顶点，加入 `glEnable(GL_DEPTH_TEST)`。
3. 在 Shader 中增加 `uniform mat4 uMVP`，在每帧用 GLM 构造 Model、View、Projection，并用 `glUniformMatrix4fv()` 上传。
4. 窗口大小回调写在 `src/02_cube.cpp`，每帧根据 framebuffer 宽高更新 viewport 和 Projection 的 aspect。
5. 本课完整参考代码位于本文末尾的“附录 B”，目标路径是 `src/02_cube.cpp`。

完成后使用目标 `02_cube` 验证本课：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 02_cube -Run
```

本课的 Shader 仍然可以写成 `src/02_cube.cpp` 中的字符串；如果后续想拆出 `.glsl` 文件，再统一放到 `assets/shaders/`，不要混入 `build/` 目录。

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

### 本课代码写在哪里

本课只修改：`src/03_texture.cpp`。

`src/03_texture.cpp` 是本课唯一需要修改的文件。请按下面方向写：

1. 从 `src/02_cube.cpp` 复制窗口、MVP、深度测试和立方体绘制结构。
2. 将每个顶点从 `xyz` 扩展为 `xyzuv`，在 VAO 中配置 location 0 和 location 1。
3. 在 `src/03_texture.cpp` 中实现 `loadPpmP3()`，读取 `assets/checker.ppm` 的 P3 像素。
4. 创建纹理对象，设置 Wrap、Filter、`glTexImage2D()` 和 `glGenerateMipmap()`。
5. 在 Fragment Shader 中使用 `sampler2D`，绘制前绑定纹理单元 0。
6. 本课完整参考代码位于本文末尾的“附录 C”，目标路径是 `src/03_texture.cpp`；程序必须从项目根目录启动。

使用目标 `03_texture` 编译和运行，确认静态纹理立方体显示正确，再进入下一课：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 03_texture -Run
```

不要把纹理代码直接修改到 `src/02_cube.cpp`；这样可以保留一个没有纹理的 MVP 对照版本。

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

### 本课代码写在哪里

本课只修改：`src/04_interactive_cube.cpp`。

`src/04_interactive_cube.cpp` 是本课唯一需要修改的文件。请按下面方向写：

1. 从 `src/03_texture.cpp` 复制纹理立方体结构，先让静态纹理立方体正常显示。
2. 在文件顶部定义 `yaw`、`pitch`、`distance`、鼠标拖动状态和上一次光标位置。
3. 在 `main()` 前实现 `framebufferSizeCallback`、`mouseButtonCallback`、`cursorPositionCallback`、`scrollCallback` 和 `processInput`。
4. 注册 GLFW 回调，在主循环中计算 `deltaTime`，再根据 yaw、pitch、distance 计算轨道相机位置。
5. 把 F、R、ESC 的处理集中写入 `processInput()`。
6. 本课完整参考代码位于本文末尾的“附录 D”，目标路径是 `src/04_interactive_cube.cpp`。

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 04_interactive_cube -Run
```

本课完成后，`src/04_interactive_cube.cpp` 才是最终综合程序；前面的 `00_window.cpp`、`01_triangle.cpp`、`02_cube.cpp` 和 `03_texture.cpp` 保留为独立的学习对照版本。

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

### 本课代码写在哪里

第 6 课不再新建源码文件，继续验收和完善完整程序：`src/04_interactive_cube.cpp`。

请在项目根目录执行下面的命令：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 04_interactive_cube -Run
```

如果某项验收失败，回到 `src/04_interactive_cube.cpp` 修改对应区域：窗口或 GLAD 问题看初始化部分，Shader 或纹理问题看资源创建部分，旋转和缩放问题看回调与相机计算部分，窗口变形问题看 framebuffer 回调和 Projection 更新部分。完成后再重复编译运行，不要修改 `build/Release/` 中生成的 `.exe`。

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

---

# 附录：完整参考代码

下面的代码是每一课的完整参考实现。练习时先打开对应的 `src/*.cpp` 壳文件，按照正文中的顺序自己写；只有编译错误或逻辑问题无法解决时，再对照对应附录。每段代码都必须放回标题中写明的路径。

## 附录 0：`src/00_window.cpp`

将下面完整代码写入 `src/00_window.cpp`，然后使用文件头的命令编译运行。

```cpp
// ============================================================================
// 练习 00：创建 OpenGL 窗口与 Context
// 完整源码位置：src/00_window.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 1 课
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 00_window -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

int main()
{
    // 1. 初始化 GLFW；失败时不要继续调用 GLFW API。
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    // 2. 请求 OpenGL 3.3 Core Profile。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. 创建窗口和 OpenGL Context。
    GLFWwindow* window = glfwCreateWindow(640, 380, "QuickOpenGL - Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    // 4. 让这个 Context 成为当前线程的 Context。
    glfwMakeContextCurrent(window);

    // 5. Context 创建后才能加载 OpenGL 函数指针。
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

    // 6. 主循环：清屏、显示当前帧、处理输入事件。
    while (!glfwWindowShouldClose(window)) {
        const float time = static_cast<float>(glfwGetTime());

        // 三个颜色通道使用不同相位，形成缓慢变化的“五彩斑斓的黑”。
        const float red = 0.050f + 0.150f * (std::sin(time) + 1.0f) * 0.5f;
        const float green = 0.030f + 0.120f * (std::sin(time + 2.0f) + 1.0f) * 0.5f;
        const float blue = 0.080f + 0.180f * (std::sin(time + 4.0f) + 1.0f) * 0.5f;

        glClearColor(red, green, blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. 退出时释放窗口和 GLFW 资源。
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

## 附录 A：`src/01_triangle.cpp`

将下面完整代码写入 `src/01_triangle.cpp`，然后使用文件头的命令编译运行。

```cpp
// ============================================================================
// 练习 01：使用 VAO、VBO 和 Shader 绘制三角形
// 完整源码位置：src/01_triangle.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 2 课
//
// 本课从 00_window.cpp 复制窗口初始化，然后在本文件中加入：
//   顶点数据 -> VBO -> VAO -> Vertex Shader/Fragment Shader -> 绘制
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 01_triangle -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace {

GLuint compileShader(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader program linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

} // namespace

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "QuickOpenGL - Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // 顶点着色器：把二维三角形顶点直接输出到裁剪空间。
    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    // 片段着色器：为三角形输出橙色。
    constexpr const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.35, 0.08, 1.0);
        }
    )";

    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // 每个顶点包含 xyz 三个 float。
    constexpr float vertices[] = {
        -0.6f, -0.5f, 0.0f,
         0.6f, -0.5f, 0.0f,
         0.0f,  0.6f, 0.0f
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // VAO 记录“如何解释 VBO 中的数据”。
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.012f, 0.006f, 0.025f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

## 附录 B：`src/02_cube.cpp`

将下面完整代码写入 `src/02_cube.cpp`，然后使用文件头的命令编译运行。

```cpp
// ============================================================================
// 练习 02：使用 MVP 矩阵绘制 3D 立方体
// 完整源码位置：src/02_cube.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 3 课
//
// 本课从 src/01_triangle.cpp 继续：
//   三角形顶点 -> 立方体顶点 -> Model/View/Projection -> 深度测试
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 02_cube -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <string>

namespace {

GLuint compileShader(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader program linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

} // namespace

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "QuickOpenGL - Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 uMVP;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(0.18, 0.62, 0.92, 1.0);
        }
    )";

    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // 立方体的 6 个面，每个面由 2 个三角形组成，共 36 个位置顶点。
    constexpr float vertices[] = {
        // front
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // back
         0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        // left
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // right
         0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
        // top
        -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,
        // bottom
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    const GLint mvpLocation = glGetUniformLocation(program, "uMVP");

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        const int width = std::max(1, framebufferWidth);
        const int height = std::max(1, framebufferHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        const float aspect = static_cast<float>(framebufferWidth) /
                             static_cast<float>(std::max(1, framebufferHeight));
        const float time = static_cast<float>(glfwGetTime());
        const glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
        const glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 1.8f, 3.2f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glViewport(0, 0, width, height);
        glClearColor(0.008f, 0.012f, 0.025f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

## 附录 C：`src/03_texture.cpp`

将下面完整代码写入 `src/03_texture.cpp`，然后使用文件头的命令编译运行。

```cpp
// ============================================================================
// 练习 03：使用 PPM 纹理绘制立方体
// 完整源码位置：src/03_texture.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 4 课
//
// 本课从 src/02_cube.cpp 继续：
//   xyz -> xyzuv -> 读取 assets/checker.ppm -> GL_TEXTURE_2D -> 采样
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 03_texture -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Image {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

bool loadPpmP3(const std::string& path, Image& image)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open texture: " << path << '\n';
        return false;
    }

    std::string magic;
    int maxValue = 0;
    if (!(file >> magic >> image.width >> image.height >> maxValue) ||
        magic != "P3" || image.width <= 0 || image.height <= 0 || maxValue <= 0) {
        std::cerr << "Invalid P3 PPM header: " << path << '\n';
        return false;
    }

    image.pixels.resize(static_cast<std::size_t>(image.width) *
                        static_cast<std::size_t>(image.height) * 3);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        int value = 0;
        if (!(file >> value)) {
            std::cerr << "Not enough pixels in texture: " << path << '\n';
            return false;
        }
        value = std::clamp(value, 0, maxValue);
        image.pixels[i] = static_cast<unsigned char>(255 * value / maxValue);
    }
    return true;
}

GLuint compileShader(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader program linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void addVertex(std::vector<float>& vertices, const glm::vec3& position, const glm::vec2& uv)
{
    vertices.insert(vertices.end(), {
        position.x, position.y, position.z, uv.x, uv.y
    });
}

void addFace(std::vector<float>& vertices,
             const glm::vec3& a,
             const glm::vec3& b,
             const glm::vec3& c,
             const glm::vec3& d)
{
    addVertex(vertices, a, {0.0f, 0.0f});
    addVertex(vertices, b, {1.0f, 0.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, d, {0.0f, 1.0f});
    addVertex(vertices, a, {0.0f, 0.0f});
}

std::vector<float> makeTexturedCube()
{
    std::vector<float> vertices;
    vertices.reserve(36 * 5);

    // 每个面按照逆时针方向添加 6 个顶点。
    addFace(vertices, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
                     { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}); // front
    addFace(vertices, { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                     {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}); // back
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f},
                     {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}); // left
    addFace(vertices, { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f}); // right
    addFace(vertices, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
                     { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}); // top
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}); // bottom
    return vertices;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

} // namespace

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "QuickOpenGL - Textured Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 uMVP;
        out vec2 TexCoord;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D uTexture;
        out vec4 FragColor;
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";

    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const std::vector<float> vertices = makeTexturedCube();
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    constexpr GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    Image image;
    if (!loadPpmP3("assets/checker.ppm", image)) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    const GLint mvpLocation = glGetUniformLocation(program, "uMVP");
    const GLint textureLocation = glGetUniformLocation(program, "uTexture");

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const float aspect = static_cast<float>(framebufferWidth) /
                             static_cast<float>(std::max(1, framebufferHeight));
        const float time = static_cast<float>(glfwGetTime());
        const glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
        const glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 1.8f, 3.2f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.008f, 0.012f, 0.025f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLocation, 0);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

## 附录 D：`src/04_interactive_cube.cpp`

将下面完整代码写入 `src/04_interactive_cube.cpp`，然后使用文件头的命令编译运行。

```cpp
// ============================================================================
// 练习 04：交互式纹理立方体（综合完整示例）
// 完整源码位置：src/04_interactive_cube.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 5、6 课
//
// 本课从 src/03_texture.cpp 继续加入：
//   鼠标左键旋转、滚轮缩放、F 自动旋转、R 重置视角、ESC 退出
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 04_interactive_cube -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Image {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

struct CameraState {
    float yaw = 35.0f;
    float pitch = 20.0f;
    float distance = 3.5f;
    bool dragging = false;
    bool autoRotate = false;
    bool fWasPressed = false;
    bool rWasPressed = false;
    double lastCursorX = 0.0;
    double lastCursorY = 0.0;
};

bool loadPpmP3(const std::string& path, Image& image)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open texture: " << path << '\n';
        return false;
    }

    std::string magic;
    int maxValue = 0;
    if (!(file >> magic >> image.width >> image.height >> maxValue) ||
        magic != "P3" || image.width <= 0 || image.height <= 0 || maxValue <= 0) {
        std::cerr << "Invalid P3 PPM header: " << path << '\n';
        return false;
    }

    image.pixels.resize(static_cast<std::size_t>(image.width) *
                        static_cast<std::size_t>(image.height) * 3);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        int value = 0;
        if (!(file >> value)) {
            std::cerr << "Not enough pixels in texture: " << path << '\n';
            return false;
        }
        value = std::clamp(value, 0, maxValue);
        image.pixels[i] = static_cast<unsigned char>(255 * value / maxValue);
    }
    return true;
}

GLuint compileShader(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader program linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void addVertex(std::vector<float>& vertices, const glm::vec3& position, const glm::vec2& uv)
{
    vertices.insert(vertices.end(), {
        position.x, position.y, position.z, uv.x, uv.y
    });
}

void addFace(std::vector<float>& vertices,
             const glm::vec3& a,
             const glm::vec3& b,
             const glm::vec3& c,
             const glm::vec3& d)
{
    addVertex(vertices, a, {0.0f, 0.0f});
    addVertex(vertices, b, {1.0f, 0.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, d, {0.0f, 1.0f});
    addVertex(vertices, a, {0.0f, 0.0f});
}

std::vector<float> makeTexturedCube()
{
    std::vector<float> vertices;
    vertices.reserve(36 * 5);
    addFace(vertices, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
                     { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f});
    addFace(vertices, { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                     {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f});
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f},
                     {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f});
    addFace(vertices, { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f});
    addFace(vertices, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
                     { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f});
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f});
    return vertices;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        state->dragging = true;
        glfwGetCursorPos(window, &state->lastCursorX, &state->lastCursorY);
    } else if (action == GLFW_RELEASE) {
        state->dragging = false;
    }
}

void cursorPositionCallback(GLFWwindow* window, double cursorX, double cursorY)
{
    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    if (!state->dragging) {
        state->lastCursorX = cursorX;
        state->lastCursorY = cursorY;
        return;
    }

    const double dx = cursorX - state->lastCursorX;
    const double dy = cursorY - state->lastCursorY;
    state->lastCursorX = cursorX;
    state->lastCursorY = cursorY;

    constexpr float sensitivity = 0.25f;
    state->yaw += static_cast<float>(dx) * sensitivity;
    state->pitch -= static_cast<float>(dy) * sensitivity;
    state->pitch = std::clamp(state->pitch, -89.0f, 89.0f);
}

void scrollCallback(GLFWwindow* window, double, double yOffset)
{
    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    state->distance -= static_cast<float>(yOffset) * 0.35f;
    state->distance = std::clamp(state->distance, 2.0f, 9.0f);
}

void processInput(GLFWwindow* window, CameraState& state, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !state.fWasPressed) {
        state.autoRotate = !state.autoRotate;
    }
    state.fWasPressed = fPressed;

    const bool rPressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    if (rPressed && !state.rWasPressed) {
        state.yaw = 35.0f;
        state.pitch = 20.0f;
        state.distance = 3.5f;
    }
    state.rWasPressed = rPressed;

    if (state.autoRotate) {
        state.yaw += 25.0f * deltaTime;
    }
}

} // namespace

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 600, "QuickOpenGL - Interactive Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    CameraState camera;
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 uMVP;
        out vec2 TexCoord;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D uTexture;
        out vec4 FragColor;
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";

    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const std::vector<float> vertices = makeTexturedCube();
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    constexpr GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    Image image;
    if (!loadPpmP3("assets/checker.ppm", image)) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    const GLint mvpLocation = glGetUniformLocation(program, "uMVP");
    const GLint textureLocation = glGetUniformLocation(program, "uTexture");
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;
        processInput(window, camera, std::min(deltaTime, 0.1f));

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const int safeWidth = std::max(1, framebufferWidth);
        const int safeHeight = std::max(1, framebufferHeight);
        const float aspect = static_cast<float>(safeWidth) / static_cast<float>(safeHeight);

        const float yaw = glm::radians(camera.yaw);
        const float pitch = glm::radians(camera.pitch);
        const glm::vec3 cameraPosition(
            camera.distance * std::cos(pitch) * std::sin(yaw),
            camera.distance * std::sin(pitch),
            camera.distance * std::cos(pitch) * std::cos(yaw));

        const glm::mat4 model = glm::mat4(1.0f);
        const glm::mat4 view = glm::lookAt(
            cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glViewport(0, 0, safeWidth, safeHeight);
        glClearColor(0.008f, 0.012f, 0.025f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLocation, 0);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```
