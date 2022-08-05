#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h" // our custom shader class
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) // function that adjusts the opengl viewport whenever the user adjusts the window size
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) // do all input processing in one place
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // close the window when escape key is pressed down
       glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell glfw that we're using opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // tell glfw we're using the core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on mac
  

    GLFWwindow* window = glfwCreateWindow(800, 600, "Check Out This Window", NULL, NULL); // creates a window with glfw, stores it as GLFWwindow object
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // need to update the current context to the window we just created

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // tell GLAD to do some magic?
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600); // tell opengl the window size is the same as the glfw window

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // tell glfw to use the function we defined as the callback


    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");

    // set up vertex data and buffers and configure vertex attribute

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first triangle
        1, 2, 3 // second triangle
    };

    unsigned int VBO, VAO, EBO; // create Vertex Buffer Object, Vertex Array Object, and Element Buffer Object
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // copy vertex array into vertex buffer for OpenGl to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy data from vertices to buffer

    // copy element index array into an element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture position attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glEnableVertexAttribArray(0);

    // draw as wireframes rather than filled
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpeg", &width, &height, &nrChannels, 0); 

    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    while(!glfwWindowShouldClose(window)) // main render loop, terminates when glfw gets a close signal
    {
        processInput(window); // call the process input function every frame
        
        // render functions
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // activate the shader
        ourShader.use();

        glm::mat4 trans = glm::mat4(1.0f); // create identity matrix for transformations
        trans = glm::translate(trans,glm::vec3(0.5f,-0.5f,0.0f));
        trans = glm::rotate(trans,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));

        unsigned int transformLoc = glGetUniformLocation(ourShader.ID,"transform");
        glUniformMatrix4fv(transformLoc,1,GL_FALSE,glm::value_ptr(trans));

        // render the triangles
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swap the buffers and poll IO event
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    // de-allocate all resources after they're done
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate(); // clean up allocated glfw resources
    return 0;
}