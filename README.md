# QuickOpenGL：从零实现可交互的 3D 贴图立方体

这是一个与 QuickOpenCV 配套思路一致的 C++ OpenGL 学习项目。`src/` 按课程编号提供带注释的完整可运行示例：每个文件都包含本课目标、关键 API、完整实现和运行命令。你可以先直接运行，再逐段阅读和修改。

历史初始示例保存在 Git 提交 `7a6d577` 中，需要时可以通过 `git show` 或从该提交恢复。

## 1. 环境要求

- C++17 编译器：MSVC 2019+、GCC 9+ 或 Clang 10+
- CMake 3.20+
- Python 3（GLAD 2 在构建阶段生成 OpenGL 加载代码）
- 支持 OpenGL 3.3 Core Profile 的显卡驱动
- VS Code（可选，推荐配合 CMake Tools）
- 首次 CMake 配置需要网络，用于 FetchContent 下载 GLFW、GLM、GLAD；GLAD 生成源码时还需要 Python 3。下载完成后可复用 `build/_deps/`

依赖分工：

| 依赖 | 作用 |
|---|---|
| GLFW 3.4 | 创建窗口、OpenGL Context、键盘鼠标输入 |
| GLAD 2 | 加载 OpenGL 函数指针 |
| GLM 1.0.1 | 向量、矩阵、旋转、视图和投影变换 |
| OpenGL 3.3 | GPU 图形渲染 API，由显卡驱动提供 |

项目不依赖 OpenCV，也不需要额外图片解码库。纹理资源使用简单的 ASCII PPM（P3）格式。

## 2. 项目结构

```text
QuickOpenGL/
├── CMakeLists.txt
├── README.md
├── build.ps1
├── build.sh
├── docs/
│   ├── OpenGL_Cpp_Quickstart_Guide.md  # 入门讲义
│   └── 动手学OpenGL_自学手册.md          # 完整代码片段与逐段解读
├── src/                              # 按课程编号组织的完整可运行示例
│   ├── 00_window.cpp                 # 完整：窗口、Context、GLAD
│   ├── 01_triangle.cpp               # 完整：VAO、VBO、Shader、三角形
│   ├── 02_cube.cpp                   # 完整：3D 顶点、MVP、深度测试
│   ├── 03_texture.cpp                # 完整：UV、PPM 纹理、纹理采样
│   └── 04_interactive_cube.cpp       # 完整：鼠标、滚轮、轨道摄像机
├── include/                          # 可放置项目头文件
├── assets/
│   └── checker.ppm                   # 示例纹理资源
└── output/                           # 可选运行结果
```

每个 `src/*.cpp` 都是独立可执行目标。`CMakeLists.txt` 自动扫描源文件；新增、删除或重命名示例文件后重新配置即可。文档中的代码块用于解释关键步骤，实际完整代码直接放在对应的 `src/*.cpp` 文件中。

## 3. 构建与运行

### Linux

在项目根目录执行以下命令：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/<your_target>
```

只编译指定目标：

```bash
cmake --build build --target <your_target> -j
```

### Windows（PowerShell）

在项目根目录执行以下命令：

```powershell
.\build.ps1
.\build.ps1 <your_target> -Run
```

也可以使用 Visual Studio 生成器：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target <your_target>
.\build\Release\<your_target>.exe
```

### macOS

需要安装 Xcode Command Line Tools 和 CMake。先在 `src/` 中创建 `.cpp` 文件，然后执行：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/<your_target>
```

如果系统无法创建 OpenGL 3.3 Core Context，先检查系统版本、显卡和驱动支持情况。

## 4. 开始编写

`src/` 已经准备好 00~04 五个完整示例文件。建议按编号运行和阅读，每个文件都可以单独编译和运行：

1. 运行并阅读 `00_window.cpp`，确认窗口、Context 和 GLAD 初始化正常。
2. 运行并阅读 `01_triangle.cpp`，理解 VAO、VBO、Shader 和第一次绘制。
3. 运行并阅读 `02_cube.cpp`，理解 3D 坐标、MVP 和深度测试。
4. 运行并阅读 `03_texture.cpp`，理解 UV 和 `assets/checker.ppm` 纹理。
5. 运行并阅读 `04_interactive_cube.cpp`，理解鼠标、滚轮和轨道摄像机。

CMake 会自动扫描 `src/*.cpp`，重新配置后每个文件都会生成同名可执行目标。每个示例文件顶部都写有本课目标、重点 API、完整运行命令和验收标准。

历史版本中的交互式立方体操作如下，恢复旧提交后可以继续使用：

| 操作 | 功能 |
|---|---|
| 鼠标左键按住并拖动 | 旋转观察立方体 |
| 鼠标滚轮 | 拉近 / 拉远摄像机，实现缩放 |
| `F` | 开启 / 关闭自动旋转 |
| `R` | 恢复初始视角和距离 |
| `ESC` | 退出程序 |

## 5. 学习顺序

1. 阅读 `docs/OpenGL_Cpp_Quickstart_Guide.md`，建立 OpenGL 渲染管线的整体认识。
2. 阅读 `docs/动手学OpenGL_自学手册.md` 中对应课程的代码解释。
3. 按 `src/00_window.cpp` 到 `src/04_interactive_cube.cpp` 的顺序运行完整源码。
4. 每完成一课就单独编译、运行并对照文件头的验收标准。
5. 先阅读对应 `src/*.cpp` 的完整实现，再结合手册中的片段理解每一步；后续可以复制该文件继续做自己的改动。

## 6. 常见问题

- **CMake 下载依赖失败：** 检查网络或代理；也可预先克隆 GLFW、GLM、GLAD 并改为本地 `add_subdirectory`。
- **GLAD 生成失败：** 检查 Python 3 是否安装，以及 CMake 是否能找到 Python 解释器。
- **找不到 OpenGL：** 更新显卡驱动；Linux 检查 OpenGL/Mesa 开发包；Windows 确认驱动正常。
- **窗口创建失败：** 当前环境可能不支持 OpenGL 3.3 Core，或处于无图形界面的远程环境。
- **纹理文件加载失败：** 从项目根目录启动程序，确保 `assets/checker.ppm` 存在。程序使用相对路径。
- **画面黑屏：** 查看终端 Shader 编译/链接日志，确认显卡驱动支持 GLSL 330。
- **VS Code 无法编译：** VS Code 只是编辑器，需要独立安装 C++ 编译器和 CMake。

## 7. 项目边界

本项目重点是掌握现代 OpenGL 的最小完整渲染流程，不是游戏引擎。暂不涉及模型导入、复杂光照、阴影、PBR、骨骼动画和场景管理。掌握当前项目后，可以继续扩展这些内容。
