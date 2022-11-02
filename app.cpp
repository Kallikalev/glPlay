#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h" // our custom shader class
#include "Camera.h" // our custom camera class
// #include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdio.h>
#include <iostream>
#include <math.h>

struct part {
    glm::vec3 dimensions; // length, width, height of rectangular prism
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotation;
    glm::vec3 color; // 0-1 rgb
};

// declare these at the top so that code below can use them, but we can implement at bottom of code
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float mouseLastX = SCR_WIDTH / 2.0f;
float mouseLastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    // initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell glfw that we're using opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // tell glfw we're using the core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on mac

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create the GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hexapod Simulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // need to update the current context to the window we just created
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // tell glfw to use the function we defined as the callback
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // tell glfw to make the cursor captured and invisible


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // tell GLAD to do some magic?
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader lightingShader("shaders/shader.vs", "shaders/shader.fs");

    // set up vertex data and buffers and configure vertex attribute

    float vertices[] = { // vertices for 36 points of 12 triangles of 6 faces of cube and normals
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    std::vector<part> parts = {
        // body
        {
            glm::vec3(3.0f,0.5f,2.0f), // dimensions
            glm::vec3(0.0f,0.0f,0.0f), // position
            glm::vec3(1.0f,0.0f,0.0f), // rotation axis
            0.0f, // amount of rotation (in degrees)
            glm::vec3(0.0f,1.0f,1.0f) // color
        },

        // leg 1
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(1.25f,0.0f,2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        },

        // leg 2
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(0.0f,0.0f,2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        },

        // leg 3
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(-1.25f,0.0f,2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        },

        // leg 4
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(1.25f,0.0f,-2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        },

        // leg 5
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(0.0f,0.0f,-2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        },

        // leg 6
        {
            glm::vec3(2.0f,0.5f,0.5f),
            glm::vec3(-1.25f,0.0f,-2.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::pi<float>()/2,
            glm::vec3(1.0f,0.2f,0.2f)
        }
    };



    unsigned int VBO, cubeVAO; // create Vertex Buffer Object, Vertex Array Object
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    // copy vertex array into vertex buffer for OpenGl to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy data from vertices to buffer

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // normal attribute
    glEnableVertexAttribArray(1);
 

    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6  * sizeof(float), (void*)0); // we don't want to use the normals, but update the stride to match
    glEnableVertexAttribArray(0);

    // draw as wireframes rather than filled
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // shader config
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setFloat("material.shininess", 32.0f);


    while (!glfwWindowShouldClose(window)) { // main render loop, terminates when glfw gets a close signal
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        processInput(window); // call the process input function every frame
        
        // render functions
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();

        glm::vec3 lightColor = glm::vec3(1.0f,1.0f,1.0f);
        glm::vec3 ambientColor = lightColor * glm::vec3(0.2f); 
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); 


        lightingShader.setVec3("dirLight.ambient",ambientColor);
        lightingShader.setVec3("dirLight.diffuse",diffuseColor);
        lightingShader.setVec3("dirLight.specular",lightColor);
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f); 

        // view/projection transformations
        lightingShader.setVec3("viewPos", camera.Position);
        glm::mat4 projectionMat = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); // 45 degree field of view, 800x600 aspect ratio, 0.1 close field, 100 far field
        glm::mat4 viewMat = camera.GetViewMatrix();
        lightingShader.setMat4("projection",projectionMat);
        lightingShader.setMat4("view",viewMat);

        glBindVertexArray(cubeVAO);

        // render each part of the robot
        for (int i = 0; i < parts.size(); i++) {
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat,parts[i].position);
            modelMat = glm::rotate(modelMat,parts[i].rotation,parts[i].rotationAxis);
            modelMat = glm::scale(modelMat,parts[i].dimensions);

            lightingShader.setMat4("model",modelMat);

            lightingShader.setVec3("color",parts[i].color); // make the light cube have the color of the light

            glDrawArrays(GL_TRIANGLES,0,36);
        }

        // swap the buffers and poll IO event
        glfwSwapBuffers(window);

        #ifdef __APPLE__ // on mac there is a bug where things don't render right until the window moves. this fixes
            static bool macMoved = false;

            if(!macMoved) {
                int x, y;
                glfwGetWindowPos(window, &x, &y);
                glfwSetWindowPos(window, ++x, y);
                macMoved = true;
            }
        #endif

        glfwPollEvents();    
    }

    // de-allocate all resources after they're done
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate(); // clean up allocated glfw resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) // function that adjusts the opengl viewport whenever the user adjusts the window size
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) // do all keyboard input processing in one place
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) { // do mouse input processing for camera motion
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (firstMouse) {
        mouseLastX = xPos;
        mouseLastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - mouseLastX;
    float yOffset = mouseLastY - yPos; // reversed since
    mouseLastX = xPos;
    mouseLastY = yPos;

    camera.ProcessMouseMovement(xOffset,yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}