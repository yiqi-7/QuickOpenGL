// ============================================================================
// 练习 03：使用 PPM 纹理绘制立方体
// 完整源码位置：src/03_texture.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 4 课
//
// 本课从 src/02_cube.cpp 继续：
//   xyz -> xyzuv -> 读取 assets/checker.ppm -> GL_TEXTURE_2D -> 采样
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 03_texture -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Image {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

bool loadPpmP3(const std::string& path, Image& image)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open texture: " << path << '\n';
        return false;
    }

    std::string magic;
    int maxValue = 0;
    if (!(file >> magic >> image.width >> image.height >> maxValue) ||
        magic != "P3" || image.width <= 0 || image.height <= 0 || maxValue <= 0) {
        std::cerr << "Invalid P3 PPM header: " << path << '\n';
        return false;
    }

    image.pixels.resize(static_cast<std::size_t>(image.width) *
                        static_cast<std::size_t>(image.height) * 3);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        int value = 0;
        if (!(file >> value)) {
            std::cerr << "Not enough pixels in texture: " << path << '\n';
            return false;
        }
        value = std::clamp(value, 0, maxValue);
        image.pixels[i] = static_cast<unsigned char>(255 * value / maxValue);
    }
    return true;
}

GLuint compileShader(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader program linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void addVertex(std::vector<float>& vertices, const glm::vec3& position, const glm::vec2& uv)
{
    vertices.insert(vertices.end(), {
        position.x, position.y, position.z, uv.x, uv.y
    });
}

void addFace(std::vector<float>& vertices,
             const glm::vec3& a,
             const glm::vec3& b,
             const glm::vec3& c,
             const glm::vec3& d)
{
    addVertex(vertices, a, {0.0f, 0.0f});
    addVertex(vertices, b, {1.0f, 0.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, c, {1.0f, 1.0f});
    addVertex(vertices, d, {0.0f, 1.0f});
    addVertex(vertices, a, {0.0f, 0.0f});
}

std::vector<float> makeTexturedCube()
{
    std::vector<float> vertices;
    vertices.reserve(36 * 5);

    // 每个面按照逆时针方向添加 6 个顶点。
    addFace(vertices, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
                     { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}); // front
    addFace(vertices, { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                     {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}); // back
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f},
                     {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}); // left
    addFace(vertices, { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f}); // right
    addFace(vertices, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
                     { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}); // top
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}); // bottom
    return vertices;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

} // namespace

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "QuickOpenGL - Textured Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    constexpr const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 uMVP;
        out vec2 TexCoord;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D uTexture;
        out vec4 FragColor;
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";

    const GLuint program = createProgram(vertexSource, fragmentSource);
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const std::vector<float> vertices = makeTexturedCube();
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    constexpr GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    Image image;
    if (!loadPpmP3("assets/checker.ppm", image)) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    const GLint mvpLocation = glGetUniformLocation(program, "uMVP");
    const GLint textureLocation = glGetUniformLocation(program, "uTexture");

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const float aspect = static_cast<float>(framebufferWidth) /
                             static_cast<float>(std::max(1, framebufferHeight));
        const float time = static_cast<float>(glfwGetTime());
        const glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
        const glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 1.8f, 3.2f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.008f, 0.012f, 0.025f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLocation, 0);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
