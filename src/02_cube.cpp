// 本课目标：理解 3D 顶点、深度测试、MVP 矩阵。完整交互立方体见 04_interactive_cube.cpp。
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

static const char* VS=R"(#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP;
void main(){ gl_Position=uMVP*vec4(aPos,1.0); }
)";
static const char* FS=R"(#version 330 core
out vec4 FragColor;
void main(){ FragColor=vec4(0.25,0.65,0.92,1.0); }
)";
static GLuint shader(GLenum t,const char*s){GLuint x=glCreateShader(t);glShaderSource(x,1,&s,nullptr);glCompileShader(x);GLint ok;glGetShaderiv(x,GL_COMPILE_STATUS,&ok);if(!ok){char l[1024]{};glGetShaderInfoLog(x,1024,nullptr,l);std::cerr<<l<<'\n';}return x;}
int main(){
 if(!glfwInit())return 1; glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
 glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
 GLFWwindow*w=glfwCreateWindow(900,600,"QuickOpenGL - 3D Cube",nullptr,nullptr);if(!w){glfwTerminate();return 1;}glfwMakeContextCurrent(w);if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){glfwDestroyWindow(w);glfwTerminate();return 1;}
 const float v[]={
 -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 0.5f,0.5f,-0.5f, 0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,
 -0.5f,-0.5f,0.5f, 0.5f,-0.5f,0.5f, 0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,
 -0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,
 0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,
 -0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,
 -0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f};
 GLuint vao,vbo;glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);glBindVertexArray(vao);glBindBuffer(GL_ARRAY_BUFFER,vbo);glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);glEnableVertexAttribArray(0);
 GLuint a=shader(GL_VERTEX_SHADER,VS),b=shader(GL_FRAGMENT_SHADER,FS),p=glCreateProgram();glAttachShader(p,a);glAttachShader(p,b);glLinkProgram(p);glDeleteShader(a);glDeleteShader(b);
 glEnable(GL_DEPTH_TEST);while(!glfwWindowShouldClose(w)){int width,height;glfwGetFramebufferSize(w,&width,&height);if(height==0){glfwPollEvents();continue;}glViewport(0,0,width,height);glClearColor(.07f,.09f,.13f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 float t=(float)glfwGetTime();glm::mat4 model=glm::rotate(glm::mat4(1),t*.6f,glm::vec3(.5f,1,.2f));glm::mat4 view=glm::lookAt(glm::vec3(2,2,3),glm::vec3(0),glm::vec3(0,1,0));glm::mat4 proj=glm::perspective(glm::radians(45.0f),(float)width/(float)height,.1f,100.0f);glm::mat4 mvp=proj*view*model;
 glUseProgram(p);glUniformMatrix4fv(glGetUniformLocation(p,"uMVP"),1,GL_FALSE,glm::value_ptr(mvp));glBindVertexArray(vao);glDrawArrays(GL_TRIANGLES,0,36);glfwSwapBuffers(w);glfwPollEvents();}
 glDeleteProgram(p);glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);glfwDestroyWindow(w);glfwTerminate();return 0;
}
