// ============================================================================
// 练习 00：创建 OpenGL 窗口与 Context
// 对应手册：docs/动手学OpenGL_自学手册.md》第 1 课
//
// 本课目标
//   1. 初始化 GLFW，并请求 OpenGL 3.3 Core Profile
//   2. 创建一个 900x600 的窗口
//   3. 让窗口 Context 成为当前线程的 Context
//   4. 使用 GLAD 加载 OpenGL 函数指针
//   5. 在循环中清屏、交换缓冲并处理窗口事件
//   6. 退出时销毁窗口并调用 glfwTerminate()
//
// 自己查文档时重点看
//   glfwInit / glfwWindowHint / glfwCreateWindow
//   glfwMakeContextCurrent / gladLoadGL
//   glfwWindowShouldClose / glfwSwapBuffers / glfwPollEvents
//
// 验收
//   窗口能够创建并正常关闭；控制台打印 OpenGL 版本；背景色可见。
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
    if (!glfwInit()) return 1; // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 请求OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 请求OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用现代profile

    GLFWwindow* window = glfwCreateWindow(640, 380, "Hello World", nullptr, nullptr);
    if (!window) { // 释放GLFW
        glfwTerminate(); 
        return 1;
    }

    glfwMakeContextCurrent(window); // 后续GL效用依赖但概念这个Context
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) return 1; // 加载函数指针
    
    while (!glfwWindowShouldClose(window)) { // 循环渲染一帧
        const float time = static_cast<float>(glfwGetTime());
        
        const float red =
    0.050f + 0.150f * (std::sin(time) + 1.0f) * 0.5f;

    const float green =
    0.030f + 0.120f * (std::sin(time + 2.0f) + 1.0f) * 0.5f;

    const float blue =
    0.080f + 0.180f * (std::sin(time + 4.0f) + 1.0f) * 0.5f;

        glClearColor(red, green, blue, 1.0f); // 设置清屏颜色 
        glClear(GL_COLOR_BUFFER_BIT);           // 设置清屏缓冲
        glfwSwapBuffers(window);                // 显示后缓冲
        glfwPollEvents();                       // 处理窗口和输入事件
    
    }
    glfwDestroyWindow(window); // 销毁窗口资源
    glfwTerminate(); // 结束GLFW
    

    return 0;
}
