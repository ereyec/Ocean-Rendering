#pragma once


#ifndef CAMERA_H
#define CAMERA_H

//Camera
class Camera
{
public:
    //How to use: Create a camera object before the game loop, in the game loop call update.
    //To set the view matrix, simply access the object's members.

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    float lastX = 600, lastY = 450; //400, 300? instead of 600,450
    float yaw = -90.0f;
    float pitch = 0.0f;
    float cameraSpeed = 8.f;

    double mouseXcurrent = 0.0;
    double mouseYcurrent = 0.0;

    float xoffset = 0.0f;
    float yoffset = 0.0f;

    struct CameraState {
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        float yaw;
        float pitch;
    };
    CameraState savedCameraState;

    GLFWwindow* window;

    Camera() {}
    Camera(GLFWwindow* _window) : window(_window) {}

    void update(float deltaTime)
    {

        glfwGetCursorPos(window, &mouseXcurrent, &mouseYcurrent);
        //KEYS        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            this->cameraPos += cameraSpeed * this->cameraFront * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            this->cameraPos -= cameraSpeed * this->cameraFront * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            this->cameraPos -= glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * cameraSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            this->cameraPos += glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * cameraSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            this->cameraPos += cameraSpeed * this->cameraUp * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            this->cameraPos -= cameraSpeed * this->cameraUp * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            yaw -= 100 * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            yaw += 100 * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            pitch += 100 * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            pitch -= 100 * deltaTime;

        //MOUSE        
        xoffset = mouseXcurrent - this->lastX;
        yoffset = this->lastY - mouseYcurrent;
        this->lastX = mouseXcurrent;
        this->lastY = mouseYcurrent;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        this->yaw += xoffset;
        this->pitch += yoffset;

        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        direction.y = sin(glm::radians(this->pitch));
        direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        this->cameraFront = glm::normalize(direction);

        //std::cout << "Pitch: " << pitch << std::endl;

    }

    void saveCameraState()
    {
        savedCameraState.position = this->cameraPos;
        savedCameraState.front = this->cameraFront;
        savedCameraState.up = this->cameraUp;
        savedCameraState.yaw = this->yaw;
        savedCameraState.pitch = this->pitch;
    }

    void restoreCameraState()
    {
        this->cameraPos = savedCameraState.position;
        this->cameraFront = savedCameraState.front;
        this->cameraUp = savedCameraState.up;
        this->yaw = savedCameraState.yaw;
        this->pitch = savedCameraState.pitch;
    }

};

#endif