// ============================================================================
// 练习 01：使用 VAO、VBO 和 Shader 绘制三角形
// 对应手册：docs/动手学OpenGL_自学手册.md》第 2 课
//
// 本课目标
//   1. 准备三个顶点的位置数据
//   2. 创建 VAO 和 VBO，把顶点数据上传到 GPU
//   3. 配置 location=0 的顶点位置属性
//   4. 编写并编译 Vertex Shader / Fragment Shader
//   5. 链接 Shader Program，检查编译和链接日志
//   6. 使用 glDrawArrays(GL_TRIANGLES, 0, 3) 绘制
//
// 自己查文档时重点看
//   glGenVertexArrays / glGenBuffers / glBufferData
//   glVertexAttribPointer / glEnableVertexAttribArray
//   glCreateShader / glShaderSource / glCompileShader
//   glCreateProgram / glAttachShader / glLinkProgram / glUseProgram
//
// 验收
//   窗口中显示三角形；修改顶点位置或片段颜色后画面会变化。
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
    // TODO: 先复制 00_window 的窗口初始化，再完成 VAO/VBO/Shader 绘制流程。
    // 建议把 Shader 编译和错误日志封装成独立函数，便于后续课程复用。
    std::cout << "TODO: implement the triangle example\n";
    return 0;
}
