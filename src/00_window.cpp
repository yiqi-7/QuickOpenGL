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
    // TODO: 按上面的 1~6 步实现最小 OpenGL 窗口。
    // 卡住时先阅读 docs/动手学OpenGL_自学手册.md》的窗口骨架。
    std::cout << "TODO: implement the OpenGL window example\n";
    return 0;
}
