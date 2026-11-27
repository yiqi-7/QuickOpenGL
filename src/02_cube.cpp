// ============================================================================
// 练习 02：使用 MVP 矩阵绘制 3D 立方体
// 对应手册：docs/动手学OpenGL_自学手册.md》第 3 课
//
// 本课目标
//   1. 为立方体准备 36 个顶点（6 个面，每面 2 个三角形）
//   2. 使用 GLM 构造 Model、View、Projection 矩阵
//   3. 将 MVP 矩阵传给 Vertex Shader 的 uniform
//   4. 开启深度测试，并在每帧清除颜色和深度缓冲
//   5. 让立方体随时间旋转
//
// 自己查文档时重点看
//   glm::rotate / glm::lookAt / glm::perspective
//   glUniformMatrix4fv / glm::value_ptr
//   GL_DEPTH_TEST / GL_COLOR_BUFFER_BIT / GL_DEPTH_BUFFER_BIT
//
// 验收
//   立方体前后遮挡正确；窗口尺寸变化时投影宽高比保持正确；模型持续旋转。
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

int main()
{
    // TODO: 复用前两课的窗口、Shader 和 VBO 代码，加入 3D 顶点与 MVP 变换。
    // 注意：GLM 使用弧度；透视投影的 aspect 应取 framebufferWidth / framebufferHeight。
    std::cout << "TODO: implement the 3D cube example\n";
    return 0;
}
