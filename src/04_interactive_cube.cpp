// QuickOpenGL final project: mouse-drag rotation + wheel zoom + 2D texture on all six faces.
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
constexpr int kWidth = 1000;
constexpr int kHeight = 700;
float gYaw = -35.0f;
float gPitch = 22.0f;
float gDistance = 4.0f;
float gLastX = 0.0f;
float gLastY = 0.0f;
bool gDragging = false;
bool gAutoRotate = false;

const char* kVertexShader = R"GLSL(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 uMVP;
void main() {
    TexCoord = aTexCoord;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)GLSL";

const char* kFragmentShader = R"GLSL(#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
void main() {
    FragColor = texture(uTexture, TexCoord);
}
)GLSL";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[2048]{};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    if (vs == 0 || fs == 0) {
        if (vs != 0) glDeleteShader(vs);
        if (fs != 0) glDeleteShader(fs);
        return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[2048]{};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader linking failed:\n" << log << '\n';
        glDeleteProgram(program);
        program = 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

// 读取本项目使用的 ASCII PPM(P3)，避免额外引入图片解码库。
bool loadPpmP3(const std::string& path, int& width, int& height,
               std::vector<unsigned char>& pixels) {
    std::ifstream file(path);
    if (!file) return false;
    std::string magic;
    file >> magic;
    if (magic != "P3") return false;
    file >> width >> height;
    int maxValue = 0;
    file >> maxValue;
    if (!file || width <= 0 || height <= 0 || maxValue <= 0) return false;
    pixels.resize(static_cast<size_t>(width) * height * 3);
    for (size_t i = 0; i < pixels.size(); ++i) {
        int value = 0;
        file >> value;
        if (!file) return false;
        value = std::clamp(value, 0, maxValue);
        pixels[i] = static_cast<unsigned char>(value * 255 / maxValue);
    }
    return true;
}

GLuint createTexture(const std::string& path) {
    int width = 0, height = 0;
    std::vector<unsigned char> pixels;
    if (!loadPpmP3(path, width, height, pixels)) {
        std::cerr << "Could not load texture: " << path << '\n';
        return 0;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action == GLFW_PRESS) {
        gDragging = true;
        double cursorX = 0.0;
        double cursorY = 0.0;
        glfwGetCursorPos(window, &cursorX, &cursorY);
        gLastX = static_cast<float>(cursorX);
        gLastY = static_cast<float>(cursorY);
    } else if (action == GLFW_RELEASE) {
        gDragging = false;
    }
}

void cursorPositionCallback(GLFWwindow*, double x, double y) {
    if (!gDragging) return;
    const float dx = static_cast<float>(x) - gLastX;
    const float dy = static_cast<float>(y) - gLastY;
    gLastX = static_cast<float>(x);
    gLastY = static_cast<float>(y);
    gYaw += dx * 0.35f;
    gPitch = std::clamp(gPitch - dy * 0.35f, -89.0f, 89.0f);
}

void scrollCallback(GLFWwindow*, double, double yOffset) {
    gDistance = std::clamp(gDistance - static_cast<float>(yOffset) * 0.35f, 2.0f, 9.0f);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        gYaw = -35.0f;
        gPitch = 22.0f;
        gDistance = 4.0f;
    }
    static bool fWasDown = false;
    const bool fIsDown = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fIsDown && !fWasDown) gAutoRotate = !gAutoRotate;
    fWasDown = fIsDown;
}

// 每个顶点布局：位置 xyz + 纹理坐标 uv。每个面单独提供 6 个顶点。
const float kCubeVertices[] = {
    // back face
    -0.5f,-0.5f,-0.5f, 0,0,   0.5f, 0.5f,-0.5f, 1,1,   0.5f,-0.5f,-0.5f, 1,0,
     0.5f, 0.5f,-0.5f, 1,1,  -0.5f,-0.5f,-0.5f, 0,0,  -0.5f, 0.5f,-0.5f, 0,1,
    // front face
    -0.5f,-0.5f, 0.5f, 0,0,   0.5f,-0.5f, 0.5f, 1,0,   0.5f, 0.5f, 0.5f, 1,1,
     0.5f, 0.5f, 0.5f, 1,1,  -0.5f, 0.5f, 0.5f, 0,1,  -0.5f,-0.5f, 0.5f, 0,0,
    // left face
    -0.5f, 0.5f, 0.5f, 1,0,  -0.5f, 0.5f,-0.5f, 1,1,  -0.5f,-0.5f,-0.5f, 0,1,
    -0.5f,-0.5f,-0.5f, 0,1,  -0.5f,-0.5f, 0.5f, 0,0,  -0.5f, 0.5f, 0.5f, 1,0,
    // right face
     0.5f, 0.5f, 0.5f, 1,0,   0.5f,-0.5f,-0.5f, 0,1,   0.5f, 0.5f,-0.5f, 1,1,
     0.5f,-0.5f,-0.5f, 0,1,   0.5f, 0.5f, 0.5f, 1,0,   0.5f,-0.5f, 0.5f, 0,0,
    // bottom face
    -0.5f,-0.5f,-0.5f, 0,1,   0.5f,-0.5f,-0.5f, 1,1,   0.5f,-0.5f, 0.5f, 1,0,
     0.5f,-0.5f, 0.5f, 1,0,  -0.5f,-0.5f, 0.5f, 0,0,  -0.5f,-0.5f,-0.5f, 0,1,
    // top face
    -0.5f, 0.5f,-0.5f, 0,1,   0.5f, 0.5f, 0.5f, 1,0,   0.5f, 0.5f,-0.5f, 1,1,
     0.5f, 0.5f, 0.5f, 1,0,  -0.5f, 0.5f,-0.5f, 0,1,  -0.5f, 0.5f, 0.5f, 0,0
};
} // namespace

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight,
        "QuickOpenGL | Interactive Textured Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    std::cout << "OpenGL version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << "\n"
              << "Controls: drag = rotate | wheel = zoom | F = auto rotate | R = reset | ESC = quit\n";

    GLuint program = createProgram();
    if (program == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
    constexpr GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const GLuint texture = createTexture("assets/checker.ppm");
    if (texture == 0) {
        glDeleteBuffers(1, &vbo); glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program); glfwDestroyWindow(window); glfwTerminate();
        return 1;
    }
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 0);
    glEnable(GL_DEPTH_TEST);

    float lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        const float currentFrame = static_cast<float>(glfwGetTime());
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (gAutoRotate && !gDragging) gYaw += 30.0f * deltaTime;

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;
        glViewport(0, 0, width, height);
        glClearColor(0.055f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::vec3 cameraPos(
            gDistance * std::cos(glm::radians(gPitch)) * std::sin(glm::radians(gYaw)),
            gDistance * std::sin(glm::radians(gPitch)),
            gDistance * std::cos(glm::radians(gPitch)) * std::cos(glm::radians(gYaw)));
        const glm::mat4 model = glm::mat4(1.0f);
        const glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "uMVP"), 1, GL_FALSE,
                           glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
