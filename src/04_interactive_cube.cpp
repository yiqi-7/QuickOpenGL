// ============================================================================
// 练习 04：交互式纹理立方体（综合练习）
// 对应手册：docs/动手学OpenGL_自学手册.md》第 5 课
//
// 必须实现
//   1. 左键拖动：更新轨道摄像机 yaw / pitch
//   2. 滚轮：更新摄像机 distance，并限制最小/最大距离
//   3. F 键：切换自动旋转；R 键：恢复初始视角；ESC：退出
//   4. 窗口大小变化时更新 viewport 和投影宽高比
//   5. 使用 deltaTime 让自动旋转速度与帧率无关
//   6. 保留 Shader、纹理、深度测试和资源释放的错误检查
//
// 推荐拆分的函数
//   compileShader / createProgram / loadPpmP3 / createTexture
//   framebufferSizeCallback / mouseButtonCallback
//   cursorPositionCallback / scrollCallback / processInput
//
// 验收
//   鼠标旋转、滚轮缩放、自动旋转和视角重置都能工作；关闭窗口后无资源泄漏。
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>

int main()
{
    // TODO: 综合 00~03 课，完成交互式纹理立方体。
    // 先让静态纹理立方体运行，再逐项加入回调和键盘控制。
    std::cout << "TODO: implement the interactive cube example\n";
    return 0;
}
