#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

#define TINYPLY_IMPLEMENTATION

#include <tinyply.h>
#include "stl.h"

static void error_callback(int /*error*/, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/* PARTICULES */
struct Particule {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 speed;
};

std::vector<Particule> MakeParticules(const int n) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution01(0, 1);
    std::uniform_real_distribution<float> distributionWorld(-1, 1);

    std::vector<Particule> p;
    p.reserve(n);

    for (int i = 0; i < n; i++) {
        p.push_back(Particule{
                {
                        distributionWorld(generator),
                             distributionWorld(generator),
                                  distributionWorld(generator)
                },
                {
                        distribution01(generator),
                             distribution01(generator),
                                  distribution01(generator)
                },
                {       0.f, 0.f, 0.f}
        });
    }

    return p;
}

GLuint MakeShader(GLuint t, const std::string &path) {
    std::cout << path << std::endl;
    std::ifstream file(path.c_str(), std::ios::in);
    std::ostringstream contents;
    contents << file.rdbuf();
    file.close();

    const auto content = contents.str();
    std::cout << content << std::endl;

    const auto s = glCreateShader(t);

    GLint sizes[] = {(GLint) content.size()};
    const auto data = content.data();

    glShaderSource(s, 1, &data, sizes);
    glCompileShader(s);

    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        GLsizei l;
        glGetShaderInfoLog(s, 512, &l, infoLog);

        std::cout << infoLog << std::endl;
    }

    return s;
}

GLuint AttachAndLink(const std::vector<GLuint> &shaders) {
    const auto prg = glCreateProgram();
    for (const auto s : shaders) {
        glAttachShader(prg, s);
    }

    glLinkProgram(prg);

    GLint success;
    glGetProgramiv(prg, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        GLsizei l;
        glGetProgramInfoLog(prg, 512, &l, infoLog);

        std::cout << infoLog << std::endl;
    }

    return prg;
}

void APIENTRY opengl_error_callback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar *message,
                                    const void *userParam) {
    std::cout << message << std::endl;
}

int main() {
    GLFWwindow *window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(512, 512, "Simple example", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity

    if (!gladLoadGL()) {
        std::cerr << "Something went wrong!" << std::endl;
        exit(-1);
    }

    // Callbacks
    glDebugMessageCallback(opengl_error_callback, nullptr);

    const std::vector<Triangle> triangles = ReadStl("n64.stl");

    // Shader
    const auto vertex = MakeShader(GL_VERTEX_SHADER, "shader.vert");
    const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "shader.frag");

    const auto program = AttachAndLink({vertex, fragment});

    glUseProgram(program);


    // Buffers
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
            GL_ARRAY_BUFFER,
            triangles.size() * sizeof(Triangle),
            triangles.data(),
            GL_STATIC_DRAW
    );

    // Bindings
    const auto posIndex = glGetAttribLocation(program, "position");
    const auto normalLoc = glGetAttribLocation(program, "normal");
    GLint modelLoc = glGetUniformLocation(program, "model");
    GLint viewLoc = glGetUniformLocation(program, "view");
    GLint projectionLoc = glGetUniformLocation(program, "projection");

    glVertexAttribPointer(
            posIndex,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3) * 2,
            nullptr
    );
    glEnableVertexAttribArray(posIndex);

    glVertexAttribPointer(
            normalLoc,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3) * 2,
            (void *) sizeof(glm::vec3)
    );
    glEnableVertexAttribArray(normalLoc);

    glEnable(GL_DEPTH_TEST);

    int width, height;
    glm::vec3 camPos(0, -20, 10);
    float vfov = 45;

    int time = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);

        float scale = 1;
//        float scale = 1 + 0.001f * float(sin(time / M_PI * 0.2f));
        float angle = float(2 * M_PI / 300 * time);

        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0), glm::vec3(0,0,1));
        glm::mat4 projection = glm::perspective(glm::radians(vfov), float(width) / float(height), 0.1f, 100.f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(0, 0, -0.6f));
        model = glm::scale(model, glm::vec3(scale));
        //model = glm::rotate(model, angle / 10.3f, glm::vec3(1, 0, 0));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
        time++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
