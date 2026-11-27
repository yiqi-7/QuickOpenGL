# QuickOpenGL：从零实现可交互的 3D 贴图立方体

这是一个与 QuickOpenCV 配套思路一致的 C++ OpenGL 学习项目。当前工作区已清空 `src/` 示例源码，方便从零开始编写自己的 OpenGL 程序；学习讲义、构建配置和纹理资源仍然保留。

历史示例保存在 Git 提交 `7a6d577` 中，需要时可以通过 `git show` 或从该提交恢复。

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
│   └── 动手学OpenGL_自学手册.md          # 分阶段学习与代码解读
├── src/                              # 从这里开始编写新的 .cpp 文件
│   └── .gitkeep
├── include/                          # 可放置项目头文件
├── assets/
│   └── checker.ppm                   # 示例纹理资源
└── output/                           # 可选运行结果
```

每个 `src/*.cpp` 都是独立可执行目标。`CMakeLists.txt` 自动扫描源文件；新增示例后重新配置即可。

## 3. 构建与运行

### Linux

先在 `src/` 中创建一个或多个 `.cpp` 文件，再执行：

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

先在 `src/` 中创建一个或多个 `.cpp` 文件，再执行：

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

当前 `src/` 目录为空。建议先创建 `src/00_window.cpp`，完成窗口和 Context 初始化，再逐步加入三角形、MVP、纹理和交互。CMake 会自动扫描 `src/*.cpp`，重新配置后每个文件都会生成同名可执行目标。

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
2. 阅读《动手学 OpenGL 自学手册》中的带注释代码片段。
3. 在 `src/00_window.cpp` 中完成窗口、Context 和 GLAD 初始化。
4. 新建三角形示例，理解顶点数据、VBO、VAO 和 Shader。
5. 新建立方体示例，理解三维坐标、深度测试和 MVP。
6. 使用 `assets/checker.ppm` 加入纹理，再实现鼠标回调和摄像机轨道控制。

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
