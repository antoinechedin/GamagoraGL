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
#include "Particle.h"

static void error_callback(int /*error*/, const char *description) {
    std::cout << description << std::endl;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void APIENTRY
opengl_error_callback(GLenum
                      source,
                      GLenum type,
                      GLuint
                      id,
                      GLenum severity,
                      GLsizei
                      length,
                      const GLchar *message,
                      const void *userParam
) {
    std::cout << "ERROR:OPEN_GL:" << message <<
              std::endl;
}

std::vector<Particle> genRandomParticles(
        const int &num,
        const glm::vec3 &min,
        const glm::vec3 &max,
        std::default_random_engine &rand
);

void processInput(GLFWwindow *window);

glm::vec3 cameraPos(0, 0, 10);
glm::vec3 cameraFront(0, 0, -1);
glm::vec3 cameraUp(0, 1, 0);
float vfov = 60;
std::default_random_engine randomEngine;

int main() {
    // SET UP GLFW
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1024, 1024, "OpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "ERROR:GLFW:CREATE_WINDOW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    // SET UP GLAD
    if (!gladLoadGL()) {
        std::cout << "ERROR:GLAD:INITIALISATION" << std::endl;
        return EXIT_FAILURE;
    }
    glDebugMessageCallback(opengl_error_callback, nullptr);

    // SET UP SHADER PROGRAM
    Shader particleShader("particleShader.vert", "particleShader.frag");
    particleShader.use();

    // GENERATE PARTICLES
    int numParticles = 1000;
    std::vector<Particle> particles = genRandomParticles(
            numParticles,
            glm::vec3(-5, -5, -5),
            glm::vec3(5, 5, 5),
            randomEngine
    );
    GLuint particlesVAO, particleVBO;
    glGenVertexArrays(1, &particlesVAO);
    glGenBuffers(1, &particleVBO);
    // BIND BUFFERS
    glBindVertexArray(particlesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
    // ATTRIB POINTERS
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) sizeof(glm::vec3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) (sizeof(glm::vec3) * 2));
    glEnableVertexAttribArray(2);
    // POINT SIZE
    glPointSize(10.0f);

    // FLOOR
    /*float vertices[] = {
             -0.5f, -0.5f, 0, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
             0.5f, 0.5f, 0, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
             -0.5f, 0.5f, 0, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, 0, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
     };
     unsigned int indices[] = {
             0, 1, 3,
             1, 2, 3
     };
     unsigned int floorVBO, floorVAO, floorEBO;
     glGenVertexArrays(1, &floorVAO);
     glGenBuffers(1, &floorVBO);
     glGenBuffers(1, &floorEBO);
     glBindVertexArray(floorVAO);
     // BIND BUFFERS
     glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
     // ATTRIB POINTERS
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) sizeof(glm::vec3));
     glEnableVertexAttribArray(1);
     // TEXTURES
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (sizeof(glm::vec3) * 2));
     glEnableVertexAttribArray(2);
     //LOAD TEXTURE
     GLuint diffuseMap;
     glGenTextures(1, &diffuseMap);
     glBindTexture(GL_TEXTURE_2D, diffuseMap);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     int width, height, nrChannels;
     stbi_set_flip_vertically_on_load(true);
     unsigned char *data = stbi_load("container2.png", &width, &height, &nrChannels, 0);
     if (data) {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
         glGenerateMipmap(GL_TEXTURE_2D);
     } else {
         std::cout << "ERROR:TEXTURE:FAILED_TO_LOAD" << std::endl;
     }
     stbi_image_free(data);*/

    // WHILE LOOP
    glEnable(GL_DEPTH_TEST);
    int screenWidth, screenHeight;
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        processInput(window);

        // SCENE SETUP
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // CAMERA
        glm::mat4 viewTransform = glm::lookAt(cameraPos, cameraPos + glm::normalize(cameraFront), cameraUp);
        glm::mat4 projectionTransform = glm::perspective(
                glm::radians(vfov),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );
        particleShader.setMat4("view", viewTransform);
        particleShader.setMat4("projection", projectionTransform);

        // PARTICLES EMITTER
        glm::mat4 emitterRoot = glm::mat4(1.0f);
        particleShader.setMat4("model", emitterRoot);
        // DRAW
        glBindVertexArray(particlesVAO);
        glDrawArrays(GL_POINTS, 0, numParticles);

        // FINISH
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

std::vector<Particle> genRandomParticles(
        const int &num,
        const glm::vec3 &min,
        const glm::vec3 &max,
        std::default_random_engine &rand
) {
    std::vector<Particle> res;
    std::uniform_real_distribution xDist(min.x, max.x);
    std::uniform_real_distribution yDist(min.y, max.y);
    std::uniform_real_distribution zDist(min.z, max.z);
    std::uniform_real_distribution color(0.0f, 1.0f);
    for (int i = 0; i < num; ++i) {
        res.emplace_back(
                glm::vec3(xDist(rand), yDist(rand), zDist(rand)),
                glm::vec3(),
                glm::vec3(color(rand), color(rand), color(rand))
        );
    }
    return res;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 0.07f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
