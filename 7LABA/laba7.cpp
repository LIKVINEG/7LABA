#define GLEW_DLL
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 12.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f, lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float gorizontRot = 0.0f;
float vertikalRot = 0.0f;
float manipulatorPos = 0.0f;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float rotSpeed = 50.0f * deltaTime;
    float moveSpeed = 2.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  gorizontRot += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) gorizontRot -= rotSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    vertikalRot += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  vertikalRot -= rotSpeed;
    if (vertikalRot > 28.0f) vertikalRot = 28.0f;
    if (vertikalRot < -31.0f) vertikalRot = -31.0f;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) manipulatorPos += moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) manipulatorPos -= moveSpeed;
    if (manipulatorPos > 1.9f) manipulatorPos = 1.9f;
    if (manipulatorPos < 0) manipulatorPos = 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float xoff = (float)xpos - lastX; float yoff = lastY - (float)ypos;
    lastX = (float)xpos; lastY = (float)ypos;
    yaw += xoff * 0.1f; pitch += yoff * 0.1f;
    if (pitch > 89.0f) pitch = 89.0f; if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(f);
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");
    Model platforma("platforma.obj"), gorizont("gorizont.obj"), vertikal("vertikal.obj"), manipulator("manipulator.obj");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        ourShader.setVec3("viewPos", cameraPos);
        ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        platforma.Draw(ourShader);

        model = glm::rotate(model, glm::radians(gorizontRot), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        gorizont.Draw(ourShader);

        glm::vec3 pivotV = glm::vec3(0.164644f, 1.24697f, -0.015617f);

        model = glm::translate(model, pivotV);
        model = glm::rotate(model, glm::radians(vertikalRot), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -pivotV);

        ourShader.setMat4("model", model);
        vertikal.Draw(ourShader);

        glm::mat4 manipM = model;
        manipM = glm::translate(manipM, glm::vec3(manipulatorPos, 0.0f, 0.0f));

        ourShader.setMat4("model", manipM);
        manipulator.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}