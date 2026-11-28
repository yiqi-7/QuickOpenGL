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
