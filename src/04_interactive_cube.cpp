// ============================================================================
// 练习 04：交互式纹理立方体（综合完整示例）
// 完整源码位置：src/04_interactive_cube.cpp
// 对应手册：docs/动手学OpenGL_自学手册.md》第 5、6 课
//
// 本课从 src/03_texture.cpp 继续加入：
//   鼠标左键旋转、滚轮缩放、F 自动旋转、R 重置视角、ESC 退出
//
// 运行：powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 04_interactive_cube -Run
// ============================================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
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

struct CameraState {
    float yaw = 35.0f;
    float pitch = 20.0f;
    float distance = 3.5f;
    bool dragging = false;
    bool autoRotate = false;
    bool fWasPressed = false;
    bool rWasPressed = false;
    double lastCursorX = 0.0;
    double lastCursorY = 0.0;
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
    addFace(vertices, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
                     { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f});
    addFace(vertices, { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                     {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f});
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f},
                     {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f});
    addFace(vertices, { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f});
    addFace(vertices, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
                     { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f});
    addFace(vertices, {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
                     { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f});
    return vertices;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        state->dragging = true;
        glfwGetCursorPos(window, &state->lastCursorX, &state->lastCursorY);
    } else if (action == GLFW_RELEASE) {
        state->dragging = false;
    }
}

void cursorPositionCallback(GLFWwindow* window, double cursorX, double cursorY)
{
    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    if (!state->dragging) {
        state->lastCursorX = cursorX;
        state->lastCursorY = cursorY;
        return;
    }

    const double dx = cursorX - state->lastCursorX;
    const double dy = cursorY - state->lastCursorY;
    state->lastCursorX = cursorX;
    state->lastCursorY = cursorY;

    constexpr float sensitivity = 0.25f;
    state->yaw += static_cast<float>(dx) * sensitivity;
    state->pitch -= static_cast<float>(dy) * sensitivity;
    state->pitch = std::clamp(state->pitch, -89.0f, 89.0f);
}

void scrollCallback(GLFWwindow* window, double, double yOffset)
{
    auto* state = static_cast<CameraState*>(glfwGetWindowUserPointer(window));
    state->distance -= static_cast<float>(yOffset) * 0.35f;
    state->distance = std::clamp(state->distance, 2.0f, 9.0f);
}

void processInput(GLFWwindow* window, CameraState& state, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !state.fWasPressed) {
        state.autoRotate = !state.autoRotate;
    }
    state.fWasPressed = fPressed;

    const bool rPressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    if (rPressed && !state.rWasPressed) {
        state.yaw = 35.0f;
        state.pitch = 20.0f;
        state.distance = 3.5f;
    }
    state.rWasPressed = rPressed;

    if (state.autoRotate) {
        state.yaw += 25.0f * deltaTime;
    }
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

    GLFWwindow* window = glfwCreateWindow(900, 600, "QuickOpenGL - Interactive Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    CameraState camera;
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

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
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;
        processInput(window, camera, std::min(deltaTime, 0.1f));

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const int safeWidth = std::max(1, framebufferWidth);
        const int safeHeight = std::max(1, framebufferHeight);
        const float aspect = static_cast<float>(safeWidth) / static_cast<float>(safeHeight);

        const float yaw = glm::radians(camera.yaw);
        const float pitch = glm::radians(camera.pitch);
        const glm::vec3 cameraPosition(
            camera.distance * std::cos(pitch) * std::sin(yaw),
            camera.distance * std::sin(pitch),
            camera.distance * std::cos(pitch) * std::cos(yaw));

        const glm::mat4 model = glm::mat4(1.0f);
        const glm::mat4 view = glm::lookAt(
            cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glViewport(0, 0, safeWidth, safeHeight);
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
