#include <iostream>

// Linmath
#include "deps/linmath.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar *vertexShaderSource = "#version 330 core\n"
                                   "uniform mat4 mvp;\n"
                                   "in vec3 position;\n"
                                   "in vec3 color_in;\n"
                                   "out vec3 color;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = mvp * vec4(position, 1.0);\n"
                                   "color = color_in;\n"
                                   "}\0";

const GLchar *fragmentShaderSource = "#version 330 core\n"
                                     "in vec3 color;\n"
                                     "out vec4 color_out;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color_out = vec4(color, 1.0);\n"
                                     "}\n\0";

// Global variables
float rotationX = 0;
float rotationY = 0;
float rotationZ = 0;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 1", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Front side
        -0.6f, -0.4f, 0.4f, 1.f, 0.f, 0.f,
        0.6f, -0.4f, 0.4f, 1.f, 0.f, 0.f,
        0.f, 0.6f, 0.0f, 1.f, 0.f, 0.f,
        // Left side
        -0.6f, -0.4f, 0.4f, 0.f, 1.f, 0.f,
        0.f, -0.4f, -0.6f, 0.f, 1.f, 0.f,
        0.f, 0.6f, 0.0f, 0.f, 1.f, 0.f,
        // Right side
        0.6f, -0.4f, 0.4f, 0.f, 0.f, 1.f,
        0.f, -0.4f, -0.6f, 0.f, 0.f, 1.f,
        0.f, 0.6f, 0.0f, 0.f, 0.f, 1.f,
        // Bottom side
        0.6f, -0.4f, 0.4f, 1.f, 1.f, 1.f,
        -0.6f, -0.4f, 0.4f, 1.f, 1.f, 1.f,
        0.f, -0.4f, -0.6f, 1.f, 1.f, 1.f};

    GLuint color_location, position_location, mvp_location;

    mvp_location = glGetUniformLocation(shaderProgram, "mvp");
    std::cout << "MVP Location: " << mvp_location << std::endl;

    position_location = glGetAttribLocation(shaderProgram, "position");
    std::cout << "Position Location: " << position_location << std::endl;

    color_location = glGetAttribLocation(shaderProgram, "color_in");
    std::cout << "Color Location: " << color_location << std::endl;

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(position_location);

    glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(color_location);

    mat4x4 mvp;
    mat4x4_identity(mvp);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

    glEnable(GL_DEPTH_TEST);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        mat4x4 m;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        mat4x4_identity(mvp);
        mat4x4_identity(m);

        mat4x4_rotate_X(m, m, (float)glfwGetTime());
        mat4x4_mul(mvp, mvp, m);

        // Draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (GLfloat *)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)
        rotationY -= 0.05;
    else if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
        rotationY += 0.05;
    else if (key == GLFW_KEY_UP && action == GLFW_REPEAT)
        rotationX -= 0.05;
    else if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
        rotationX += 0.05;
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
    }
}
