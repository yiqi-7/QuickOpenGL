// ============================================================================
// 练习 03：给立方体添加纹理
// 对应手册：docs/动手学OpenGL_自学手册.md》第 4 课
//
// 本课目标
//   1. 扩展顶点布局：位置 xyz + 纹理坐标 uv
//   2. 编写 Vertex Shader，将 UV 传给 Fragment Shader
//   3. 编写 Fragment Shader，使用 sampler2D 采样纹理
//   4. 从 assets/checker.ppm 读取 ASCII PPM(P3) 像素
//   5. 创建 GL_TEXTURE_2D，设置 Wrap、Filter 并生成 Mipmap
//   6. 绘制带纹理的立方体
//
// 自己查文档时重点看
//   glTexImage2D / glTexParameteri / glGenerateMipmap
//   glActiveTexture / glBindTexture / glUniform1i
//   顶点 stride、属性 offset 与 UV 坐标的对应关系
//
// 验收
//   立方体六个面都能显示 checker.ppm；纹理不会因 stride 或 offset 错误而错位。
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    // TODO: 在 02_cube 的 MVP 基础上加入 UV 属性和 2D 纹理对象。
    // 建议先实现 loadPpmP3，再实现 createTexture，最后接入渲染循环。
    std::cout << "TODO: implement the textured cube example\n";
    return 0;
}
