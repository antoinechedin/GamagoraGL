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
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <tinyply.h>
#include "stl.h"
#include "Shader.h"

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
    Shader program ("shader.vert", "shader.frag");
    program.use();

    //Load texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(
            "container.jpg",
            &width,
            &height,
            &nrChannels,
            0
    );
    if (data) {
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    float vertices[] = {
            // positions          // colors           // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    // Buffers
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
    );

    // Bindings
    const auto posIndex = glGetAttribLocation(program.ID, "position");
    const auto normalLoc = glGetAttribLocation(program.ID, "normal");
    const auto texCoordLoc = glGetAttribLocation(program.ID, "texCoords");
    GLint modelLoc = glGetUniformLocation(program.ID, "model");
    GLint viewLoc = glGetUniformLocation(program.ID, "view");
    GLint projectionLoc = glGetUniformLocation(program.ID, "projection");

    glVertexAttribPointer(
            posIndex,
            3,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(float),
            nullptr
    );
    glEnableVertexAttribArray(posIndex);
    glVertexAttribPointer(
            normalLoc,
            3,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(float),
            (void *) sizeof(glm::vec3)
    );
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(
            texCoordLoc,
            2,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(float),
            (void *) (sizeof(glm::vec3) * 2)
    );
    glEnableVertexAttribArray(texCoordLoc);

    glEnable(GL_DEPTH_TEST);

    int screenWidth, screenHeight;
    glm::vec3 camPos(0, 0, -2);
    float vfov = 45;
    int time = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

        float scale = 1;
//        float scale = 1 + 0.001f * float(sin(time / M_PI * 0.2f));
        float angle = float(2 * M_PI / 300 * time);

        glm::mat4 view = glm::lookAt(
                camPos,
                glm::vec3(0),
                glm::vec3(0, 1, 0)
        );
        glm::mat4 projection = glm::perspective(
                glm::radians(vfov),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
//        model = glm::translate(model, glm::vec3(0, 0, -0.6f));
//        model = glm::scale(model, glm::vec3(scale));
        //model = glm::rotate(model, angle / 10.3f, glm::vec3(1, 0, 0));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

//        glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        time++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
