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

glm::vec3 cameraPos(0, 0, 9);
glm::vec3 cameraFront(0, 0, -1);
glm::vec3 cameraUp(0, 1, 0);
float vfov = 45;


int main() {
    // SET UP GLFW
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr);
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
    program.setVec3("viewPosition", cameraPos);
    program.setVec3("light.position", 5, 5, 5);

    program.setVec3("light.ambient", 0.8, 0.8, 0.8);
    program.setVec3("light.diffuse", 0.5, 0.5, 0.5);
    program.setVec3("light.specular", 1, 1, 1);

    // MATERIAL
//    program.setInt("material.diffuse", 0);
//    program.setInt("material.specular", 1);
    program.setFloat("material.shininess", 4.0f);

    // FRAMEBUFFER
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // FRAMEBUFFER TEXTURE
    GLuint screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLuint screenTextureDepth;
    glGenTextures(1, &screenTextureDepth);
    glBindTexture(GL_TEXTURE_2D, screenTextureDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 512, 512, 0,  GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ATTACH TEXTURE
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, screenTextureDepth, 0);



    // WHILE LOOP
    glEnable(GL_DEPTH_TEST);
    int screenWidth, screenHeight;
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        processInput(window);

//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
//        model = glm::translate(model, glm::vec3(0, 0, -0.6f));
//        model = glm::scale(model, glm::vec3(scale));
//        model = glm::rotate(model, angle / 10.3f, glm::vec3(1, 0, 0));

        // DRAENEI RENDER
                glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.8f, 0.8f, 0.8f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //DRAENEI LIGHT
        program.setVec3("light.specular", 1, 1, 1);
        // DRAW DRAENEI
        glm::vec3 dCameraPos(0, 0, 9);
        glm::vec3 dCameraFront(0, 0, -1);
        glm::vec3 dCameraUp(0, 1, 0);
        glm::mat4 dViewTransform = glm::lookAt(dCameraPos, dCameraPos + dCameraFront, cameraUp);
        program.setMat4("view", dViewTransform);
        glm::mat4 dProjectionTransform = glm::perspective(
                glm::radians(vfov),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );
        program.setMat4("projection", dProjectionTransform);
        float angle = float(M_PI) * float(glfwGetTime() / 5.0f);
        glm::mat4 modelTransform = glm::mat4(1.0f);
        modelTransform = glm::rotate(modelTransform, angle, glm::vec3(0, 1, 0));
        modelTransform = glm::translate(modelTransform, glm::vec3(0, -2, 0));
        program.setMat4("model", modelTransform);
        draenei.Draw(program);

        // SCREEN RENDER
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //SCREEN LIGHT
        program.setVec3("light.specular", 0, 0, 0);
        // TRANSFORM SCREEN
        glm::mat4 viewTransform = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        program.setMat4("view", viewTransform);
        glm::mat4 projectionTransform = glm::perspective(
                glm::radians(vfov),
                float(screenWidth) / float(screenHeight),
                0.1f,
                100.f
        );
        program.setMat4("projection", projectionTransform);
        glm::mat4 screenTransform = glm::mat4(1.0f);
        float screenAngle = glm::cos(float(glfwGetTime())) * float(M_PI) / 3.0f;
        screenTransform = glm::rotate(screenTransform, screenAngle, glm::vec3(0, 1, 0));
        screenTransform = glm::scale(screenTransform, glm::vec3(6, 6, 6));
        program.setMat4("model", screenTransform);
        // BIND SCREEN BUFFERS AND TEXTURE
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        // DRAW SCREEN
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
