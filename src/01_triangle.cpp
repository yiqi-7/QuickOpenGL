#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

static const char* vertexShader = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";
static const char* fragmentShader = R"(#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(0.95, 0.48, 0.20, 1.0); }
)";

static GLuint compile(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024]{};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error: " << log << '\n';
    }
    return shader;
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* w = glfwCreateWindow(900, 600, "QuickOpenGL - Triangle", nullptr, nullptr);
    if (!w) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(w);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) { glfwDestroyWindow(w); glfwTerminate(); return 1; }

    const float vertices[] = {-0.6f,-0.5f,0.0f, 0.6f,-0.5f,0.0f, 0.0f,0.6f,0.0f};
    GLuint vao=0, vbo=0;
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    GLuint vs=compile(GL_VERTEX_SHADER,vertexShader), fs=compile(GL_FRAGMENT_SHADER,fragmentShader);
    GLuint program=glCreateProgram(); glAttachShader(program,vs); glAttachShader(program,fs); glLinkProgram(program);
    GLint linked=GL_FALSE; glGetProgramiv(program,GL_LINK_STATUS,&linked);
    if (!linked) { char log[1024]{}; glGetProgramInfoLog(program,sizeof(log),nullptr,log); std::cerr<<log<<'\n'; }
    glDeleteShader(vs); glDeleteShader(fs);

    while (!glfwWindowShouldClose(w)) {
        glClearColor(0.08f,0.10f,0.14f,1.0f); glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program); glBindVertexArray(vao); glDrawArrays(GL_TRIANGLES,0,3);
        glfwSwapBuffers(w); glfwPollEvents();
    }
    glDeleteProgram(program); glDeleteBuffers(1,&vbo); glDeleteVertexArrays(1,&vao);
    glfwDestroyWindow(w); glfwTerminate(); return 0;
}
