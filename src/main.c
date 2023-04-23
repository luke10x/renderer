
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h> // include glad header

#ifdef __EMSCRIPTEN__

// Emscripten
#include <GLFW/glfw3.h>
#include <GL/gl.h> // Include OpenGL header

#include <emscripten/emscripten.h>
#else
#ifdef __APPLE__

// NOT Emscripten && Apple

#include <GLFW/glfw3.h>

#else

// NOT Emsripeten && NOT Apple
#include <GL/glut.h>

#endif
#endif

#define SW          64
#define SH          64
#define PIXEL_SCALE 4           // OpenGL pixel scale
#define GLSW        (SW*PIXEL_SCALE) // OpenGL window width
#define GLSH        (SH*PIXEL_SCALE) // OpenGL window height

int init_gl();
void do_frame();
void shutdown_gl();
 
GLFWwindow* window;



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    printf("size resetted = %dx%d\n", width, height);

    glViewport(0, 0, width, height);
}

// void pixel_put(int x, int y, int r, int g, int b) {
//   glColor3ub(r, g, b);
//   glBegin(GL_POINTS);
//   glVertex2i(x * PIXEL_SCALE + 2, y * PIXEL_SCALE + 2);
//   glEnd();
// }
 
 void pixel_put(int x, int y, int r, int g, int b) {
  // Create and compile the shader program
  GLuint shaderProgram;
  shaderProgram = glCreateProgram();

  const char* vertexShaderSource = "#version 330 core\n"
                                   "layout (location = 0) in vec2 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
                                   "}\0";

  const char* fragmentShaderSource = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
                                     "}\n\0";

  // Vertex shader
  GLuint vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // Fragment shader
  GLuint fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Link the shaders
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glUseProgram(shaderProgram);

  // Set the vertex data
  float vertices[] = {
    (float)x * PIXEL_SCALE, (float)y * PIXEL_SCALE
  };
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Set the vertex attributes pointers
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Draw the pixel
  glDrawArrays(GL_POINTS, 0, 1);

  // Delete the VBO and VAO
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // Delete the shaders and program
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteProgram(shaderProgram);
}

void do_frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pixel_put(10, 10, 0xff, 0x00, 0xff);

  // Swap front and back buffers
  glfwSwapBuffers(window);

  // Poll for and process events
  glfwWaitEvents();
}
 
void shutdown_gl() {
    glfwTerminate();
}


/**
 * Initialize GL 
 */
int init_gl() {

  if (glfwInit() != GL_TRUE) {
      printf("glfwInit() failed\n");
      return GL_FALSE;
  }

  window = glfwCreateWindow( GLSW, GLSH, "Hello World", NULL, NULL );
  if (!window) {
    glfwTerminate();
    return GL_FALSE;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);


  glLoadIdentity();
  glOrtho(0, GLSW, 0, GLSH, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D );
  glPointSize(PIXEL_SCALE);

  return GL_TRUE;
}


int main() {
  printf("Started\n");
  if (init_gl() == GL_TRUE) {
    // emscripten_set_main_loop(do_frame, 0, 1);
#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    // emscripten_set_main_loop_arg(mainLoop, mainLoopArg, fps, true);
    emscripten_set_main_loop(do_frame, 0, 1);
#else
    while (!glfwWindowShouldClose(window)) do_frame();
#endif
  }
  printf("ended\n");
 
  shutdown_gl();
  return 0;
}