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
#include "Model.h"

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

void processInput(GLFWwindow *window);

void renderScene();

glm::vec3 cameraPos(0, 3, 9);
glm::vec3 cameraFront(0, -0.3f, -1);
glm::vec3 cameraUp(0, 1, 0);
float vfov = 60;


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
    Shader program("shader.vert", "shader.frag");
    program.use();
    program.setInt("material.texture_diffuse1", 0);
    program.setInt("material.texture_specular1", 1);
    program.setInt("shadowMap", 2);

    // MODEL
    Model draenei("warcraft-draenei-fanart/untitled.obj");

    // SET UP VERTEX DATA
    float vertices[] = {
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
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    // BIND BUFFERS
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // POSITION
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    // NORMALS
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
    stbi_image_free(data);

    // LIGHT
    glm::vec3 lightPos(5, 5, 5);
    glm::vec3 lightFront(-1, -1, -1);
    glm::vec3 lightUp(0, 1, 0);
    program.setVec3("viewPosition", cameraPos);
    program.setVec3("light.position", lightPos);

    program.setVec3("light.ambient", 0.4, 0.4, 0.4);
    program.setVec3("light.diffuse", 0.9, 0.9, 0.9);
    program.setVec3("light.specular", 1, 1, 1);

    // MATERIAL
//    program.setInt("material.diffuse", 0);
//    program.setInt("material.specular", 1);
    program.setFloat("material.shininess", 4.0f);

    // SHADOW MAP
    int shadowSize = 1024;
    // SHADOW MAP TEXTURE
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // FRAMEBUFFER TEXTURE
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // WHILE LOOP
    glEnable(GL_DEPTH_TEST);
    int screenWidth, screenHeight;
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        processInput(window);

        // GLOBAL TRANSFORMS
        float angle = float(M_PI) * float(glfwGetTime() / 5.0f);
        glm::mat4 draeneiTransform = glm::mat4(1.0f);
        draeneiTransform = glm::rotate(draeneiTransform, angle, glm::vec3(0, 1, 0));
        draeneiTransform = glm::translate(draeneiTransform, glm::vec3(0, -2, 0));

        glm::mat4 groundTransform = glm::mat4(1.0f);
        groundTransform = glm::translate(groundTransform, glm::vec3(0, -2.5, 0));
        groundTransform = glm::scale(groundTransform, glm::vec3(20, 20, 20));
        groundTransform = glm::rotate(groundTransform, glm::radians(90.0f), glm::vec3(1, 0, 0));

        // RENDER DEPTH MAP
        glViewport(0, 0, shadowSize, shadowSize);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // DEPTH PROJECTION
        glm::mat4 lightView = glm::lookAt(lightPos, lightPos + glm::normalize(lightFront), lightUp);
        glm::mat4 lightProjection = glm::perspective(
                glm::radians(75.0f),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        program.setMat4("view", lightView);
        program.setMat4("projection", lightProjection);
        program.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // DRAENEI
        program.setMat4("model", draeneiTransform);
        draenei.Draw(program);
        // GROUND
        program.setMat4("model", groundTransform);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // SCENE RENDER
        glViewport(0, 0, screenWidth, screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // SCENE PROJECTION
        glm::mat4 viewTransform = glm::lookAt(cameraPos, cameraPos + glm::normalize(cameraFront), cameraUp);
        glm::mat4 projectionTransform = glm::perspective(
                glm::radians(vfov),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );
        program.setMat4("view", viewTransform);
        program.setMat4("projection", projectionTransform);
        // SET SHADOW MAP
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        // DRAENEI
        program.setMat4("model", draeneiTransform);
        draenei.Draw(program);
        // DRAW GROUND
        program.setMat4("model", groundTransform);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // FINISH
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
