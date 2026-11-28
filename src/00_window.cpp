// ============================================================================
// 练习 00：创建 OpenGL 窗口与 Context
// 完整源码位置：src/00_window.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 1 课
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 00_window -Run
// ============================================================================
// GLAD 声明 OpenGL 函数类型，并在 Context 创建后加载实际函数地址。
#include <glad/gl.h>
// GLFW 负责窗口、OpenGL Context 以及键盘鼠标事件。
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

// 每个附录都是一个独立程序，因此从 main() 开始完成初始化、绘制和释放。
int main()
{
    // 1. 初始化 GLFW；失败时不要继续调用 GLFW API。
    // 初始化 GLFW；失败时没有可用的窗口系统，立即返回。
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    // 2. 请求 OpenGL 3.3 Core Profile。
    // 这些 Hint 必须在创建窗口前设置，决定新 Context 请求的 OpenGL 版本和模式。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. 创建窗口和 OpenGL Context。
    // 创建窗口时同时创建与之关联的 OpenGL Context。
    GLFWwindow* window = glfwCreateWindow(640, 380, "QuickOpenGL - Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    // 4. 让这个 Context 成为当前线程的 Context。
    // 只有当前线程拥有有效 Context 后，glXXX 函数调用才有作用。
    glfwMakeContextCurrent(window);

    // 5. Context 创建后才能加载 OpenGL 函数指针。
    // GLFW 提供地址查询入口，GLAD 用它加载当前驱动支持的 OpenGL 函数。
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        // 退出时释放窗口和 Context；随后终止 GLFW 的全局资源。
    glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // 查询驱动报告的版本和渲染器，确认 Context 确实创建成功。
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

    // 6. 主循环：清屏、显示当前帧、处理输入事件。
    // 每次循环完成一帧：处理输入、更新状态、清除缓冲、绘制、交换前后缓冲。
    while (!glfwWindowShouldClose(window)) {
        // 使用经过的秒数驱动动画，避免把动画速度写死为某个帧数。
        const float time = static_cast<float>(glfwGetTime());

        // 三个颜色通道使用不同相位，形成缓慢变化的“五彩斑斓的黑”。
        const float red = 0.050f + 0.150f * (std::sin(time) + 1.0f) * 0.5f;
        const float green = 0.030f + 0.120f * (std::sin(time + 2.0f) + 1.0f) * 0.5f;
        const float blue = 0.080f + 0.180f * (std::sin(time + 4.0f) + 1.0f) * 0.5f;

        // glClearColor 只设置清屏状态，下一行 glClear 才真正写入颜色缓冲。
        glClearColor(red, green, blue, 1.0f);
        // 清除上一帧的颜色，避免新帧残留旧画面。
        glClear(GL_COLOR_BUFFER_BIT);

        // 双缓冲先绘制后台缓冲，再一次性显示，避免半帧画面闪烁。
        glfwSwapBuffers(window);
        // 取出操作系统事件，否则窗口无法响应移动、关闭和键盘鼠标输入。
        glfwPollEvents();
    }

    // 7. 退出时释放窗口和 GLFW 资源。
    // 退出时释放窗口和 Context；随后终止 GLFW 的全局资源。
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
