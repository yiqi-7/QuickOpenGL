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
#include <string>
#include <cmath>


namespace {
    // 将一段GLSL源码编译成一个Shader对象, type区分是顶点还是片段
    GLuint compileShader(GLenum type, const char* source)
    {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader); 

        GLint success = GL_FALSE;
        // 读取编译状态 
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<std::size_t>(logLength), '\0');

            // log包含错误原因
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());
            std::cerr << "Shader compilation failed: \n" << log << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    // 将两个Shader编译成 一次绘制使用的Program 
    GLuint createProgram(const char* vertexSouce, const char* fragmentSource)
    {
        const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSouce);
        const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (vertexShader == 0 || fragmentShader == 0) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return 0;
        }

        const GLuint program = glCreateProgram();
        // 先附加顶点和片段，再链接器输入输出接口
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        // 链接阶段检测两个Shader的接口是否匹配
        glLinkProgram(program);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<std::size_t>(logLength), '\0');
            glGetProgramInfoLog(program, logLength, nullptr, log.data());

            std::cerr << "Shader program linking failed:\n" << log << std::endl;
            glDeleteProgram(program);
            
            // 链接完成不在需要临时shader对象
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return 0;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return program;
    }

    // framebuffer 
    void framebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        // 把OpenGL的标准化设备坐标映射到当前的framebuffer
        glViewport(0, 0, width, height);
    }
} // namespace



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
    GLFWwindow* window = glfwCreateWindow(800, 600, "QuickOpenGL - triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    // 4. 让这个 Context 成为当前线程的 Context。
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // 5. Context 创建后才能加载 OpenGL 函数指针。
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }


    // 顶点着色器: 把二维三角形顶点输出到裁剪空间
    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    // 片段着色器: 为三角形输出橙色
    constexpr const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.35, 0.08, 1.0);
        }
    )";

    // 两个Shader都编译成功后才能让program进行绘制 
    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        // 释放窗口和Context
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

   
    const float vertices[] = { // 三个二维三角形顶点，每个位置用 xyz 表示
        -0.6f, -0.5f, 0.0f,
        0.6f, -0.5f, 0.0f,
        0.0f,  0.6f, 0.0f
    };
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao); // 创建顶点格式配置对象
    glGenBuffers(1, &vbo);      // 创建顶点数据缓冲

    glBindVertexArray(vao);      // 后续属性设置归这个 VAO 管理
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // 绑定要写入的顶点缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 上传顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr); // 属性 0 每顶点 3 个 float
    glEnableVertexAttribArray(0); // 启用顶点位置属性
    glBindVertexArray(0); //  解绑 VAO，避免后续代码误修改它

    // 6. 主循环：清屏、显示当前帧、处理输入事件。
    while (!glfwWindowShouldClose(window)) {
        const float time = static_cast<float>(glfwGetTime());

        // 三个颜色通道使用不同相位，形成缓慢变化的“五彩斑斓的黑”。
        const float red = 0.050f + 0.150f * (std::sin(time) + 1.0f) * 0.5f;
        const float green = 0.030f + 0.120f * (std::sin(time + 2.0f) + 1.0f) * 0.5f;
        const float blue = 0.080f + 0.180f * (std::sin(time + 4.0f) + 1.0f) * 0.5f;

        glClearColor(red, green, blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用当前program
        glUseProgram(program);
        // 后续顶点属性记录到当前VAO
        glBindVertexArray(vao); 
        // 从顶点0开始取三个顶点，组成一个三角形
        glDrawArrays(GL_TRIANGLES, 0, 3); 

        // 双缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 删除GPU资源
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);

    // 7. 退出时释放窗口和 GLFW 资源。
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
