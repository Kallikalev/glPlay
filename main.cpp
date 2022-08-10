#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h" // our custom shader class
#include "Camera.h" // our custom camera class
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <math.h>

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

glm::vec3 lightPos(1.2f,1.0f,2.0f);

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell glfw that we're using opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // tell glfw we're using the core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on mac
  

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Check Out This Window", NULL, NULL); // creates a window with glfw, stores it as GLFWwindow object
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

    glViewport(0, 0, 800, 600); // tell opengl the window size is the same as the glfw window



    // build and compile our shader program
    Shader lightingShader("shaders/shader.vs", "shaders/shader.fs");
    Shader lightCubeShader("shaders/light_cube.vs","shaders/light_cube.fs");

    // set up vertex data and buffers and configure vertex attribute

    float vertices[] = { // the 36 vertices for the 12 triangles for the 6 faces of a cube, along with their normals
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
    // unsigned int indices[] = {
    //     0, 1, 3, // front face
    //     2, 1, 3
    // };

    unsigned int VBO, cubeVAO/*, EBO*/; // create Vertex Buffer Object, Vertex Array Object, and Element Buffer Object
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);

    // copy vertex array into vertex buffer for OpenGl to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy data from vertices to buffer

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // normal attribute
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glEnableVertexAttribArray(0);


    // create the light with the same vertices as the cube object
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // we only need to bind the the existing VBO, it already contians cube data
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6  * sizeof(float), (void*)0); // we don't want to use the normals, but update the stride to match
    glEnableVertexAttribArray(0);

    // draw as wireframes rather than filled
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // tell openGl to do depth testing
    glEnable(GL_DEPTH_TEST);  


    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpeg", &width, &height, &nrChannels, 0); 

    stbi_image_free(data);

    // set up camera


    while(!glfwWindowShouldClose(window)) // main render loop, terminates when glfw gets a close signal
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        processInput(window); // call the process input function every frame
        
        // render functions
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate the shader
        lightingShader.use();

        lightingShader.setVec3("objectColor",glm::vec3(1.0f,0.5,0.31f));
        lightingShader.setVec3("lightColor",glm::vec3(1.0f,1.0f,1.0f));
        lightingShader.setVec3("lightPos",lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // camera/view transformation
        glm::mat4 viewMat = camera.GetViewMatrix();

        lightingShader.setMat4("view",viewMat);

        // create projection matrix
        glm::mat4 projectionMat;
        projectionMat = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); // 45 degree field of view, 800x600 aspect ratio, 0.1 close field, 100 far field

        lightingShader.setMat4("projection",projectionMat);

        // multiple cubes, all in a bit different spot
        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f), 
            glm::vec3( 2.0f,  5.0f, -15.0f), 
            glm::vec3(-1.5f, -2.2f, -2.5f),  
            glm::vec3(-3.8f, -2.0f, -12.3f),  
            glm::vec3( 2.4f, -0.4f, -3.5f),  
            glm::vec3(-1.7f,  3.0f, -7.5f),  
            glm::vec3( 1.3f, -2.0f, -2.5f),  
            glm::vec3( 1.5f,  2.0f, -2.5f), 
            glm::vec3( 1.5f,  0.2f, -1.5f), 
            glm::vec3(-1.3f,  1.0f, -1.5f)  
        };

        // render the cube, repeat 10 times with each
        glBindVertexArray(cubeVAO);

        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++) {
            // model transformation matrix
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat,cubePositions[i]);
            float angle = 20.0f * i;
            modelMat = glm::rotate(modelMat,glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            if (i % 3 == 0) {
                modelMat = glm::rotate(modelMat,(float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
            }

            lightingShader.setMat4("model",modelMat);


            glDrawArrays(GL_TRIANGLES,0,36);
        }
        // draw the cube objects
        glDrawArrays(GL_TRIANGLES,0,36);



        // draw the light source marker
        lightCubeShader.use();
        lightCubeShader.setVec3("lightColor",glm::vec3(1.0f,1.0f,1.0f)); // make the light cube have the color of the light

        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, lightPos);
        modelMat = glm::scale(modelMat,glm::vec3(0.2f));
        lightCubeShader.setMat4("view",viewMat);
        lightCubeShader.setMat4("projection",projectionMat);
        lightCubeShader.setMat4("model",modelMat);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES,0,36);


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
    glDeleteVertexArrays(1, &lightCubeVAO);
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