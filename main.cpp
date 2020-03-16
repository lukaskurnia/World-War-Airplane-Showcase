#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Linmath
#include "deps/linmath.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void read_vertices(GLfloat *vertices, std::string filename);
GLFWwindow *init();
GLuint compile_shader(const GLchar *shaderSource, GLenum type);

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
float zoom = 0;

int main()
{
    GLFWwindow *window = init();

    GLuint vertexShader, fragmentShader;
    vertexShader = compile_shader(vertexShaderSource, GL_VERTEX_SHADER);
    fragmentShader = compile_shader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[1024];
    read_vertices(vertices, "vertices.txt");

    GLuint color_location, position_location, mvp_location;
    mvp_location = glGetUniformLocation(shaderProgram, "mvp");
    position_location = glGetAttribLocation(shaderProgram, "position");
    color_location = glGetAttribLocation(shaderProgram, "color_in");

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(position_location);

    glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(color_location);

    mat4x4 mvp;
    mat4x4_identity(mvp);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        mat4x4 m, p;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        mat4x4_identity(mvp);
        mat4x4_identity(m);
        mat4x4_identity(p);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        float ratio = width / (float)height;

        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 2.f + zoom, -2.f + zoom);

        vec3 eye = {0.f, 0.f, 1.f};
        vec3 center = {0.f, 0.f, 0.f};
        vec3 up = {0.f, 1.f, 0.f};
        mat4x4_look_at(m, eye, center, up);

        mat4x4_rotate_X(m, m, rotationX);
        mat4x4_rotate_Y(m, m, rotationY);
        mat4x4_rotate_Z(m, m, rotationZ);

        mat4x4_mul(mvp, p, m);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (GLfloat *)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 40);
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
        // rotationY -= 0.05;
        rotationY += 0.05;
    else if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
        // rotationY += 0.05;
        rotationY -= 0.05;
    else if (key == GLFW_KEY_UP && action == GLFW_REPEAT)
        // rotationX -= 0.05;
        rotationX += 0.05;
    else if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
        // rotationX += 0.05;
        rotationX -= 0.05;
    else if (key == GLFW_KEY_Z && action == GLFW_REPEAT)
        // rotationX -= 0.05;
        rotationZ += 0.05;
    else if (key == GLFW_KEY_X && action == GLFW_REPEAT)
        // rotationX += 0.05;
        rotationZ -= 0.05;
    else if (key == GLFW_KEY_W && action == GLFW_REPEAT)
        zoom += 0.05;
    else if (key == GLFW_KEY_S && action == GLFW_REPEAT)
        zoom -= 0.05;
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        zoom = 0;
    }
}

void read_vertices(GLfloat *vertices, std::string filename)
{
    std::ifstream vertices_file;
    std::string line;
    int line_count = 0, LINE_SIZE = 6;

    vertices_file.open(filename);
    if (vertices_file.is_open())
    {
        while (std::getline(vertices_file, line))
        {
            std::istringstream in(line);

            if (line[0] != '#')
            {
                float x, y, z, r, g, b;
                in >> x >> y >> z >> r >> g >> b;

                vertices[line_count * LINE_SIZE + 0] = x;
                vertices[line_count * LINE_SIZE + 1] = y;
                vertices[line_count * LINE_SIZE + 2] = z;
                vertices[line_count * LINE_SIZE + 3] = r;
                vertices[line_count * LINE_SIZE + 4] = g;
                vertices[line_count * LINE_SIZE + 5] = b;

                line_count++;
            }
        }
    }
}

GLFWwindow *init()
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

    return window;
}

GLuint compile_shader(const GLchar *shaderSource, GLenum type)
{
    // Compile shader
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation failed\n"
                  << infoLog << std::endl;
    }

    return shader;
}
