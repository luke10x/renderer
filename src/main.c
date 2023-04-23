
#include <stdlib.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__

// Emscripten
#include <GL/glfw.h>
#include <emscripten/emscripten.h>
#else
#ifdef __APPLE__

// NOT Emscripten && Apple

#include <GLUT/glut.h>
#include <GLFW/glfw3native.h>
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

int main() {
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
 
  shutdown_gl();
  return 0;
}

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

/**
 * Initialize GL 
 */
int init_gl() {
  // const int width = 320,
  //           height = 240;

  if (glfwInit() != GL_TRUE) {
      printf("glfwInit() failed\n");
      return GL_FALSE;
  }



  window = glfwCreateWindow( GLSW, GLSH, "Hello World", NULL, NULL );
  if (!window) {
    glfwTerminate();
    return GL_FALSE;
  }
  glPointSize(PIXEL_SCALE);

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  return GL_TRUE;
}


void pixel_put(int x, int y, int r, int g, int b) {
  glColor3ub(r, g, b);
  glBegin(GL_POINTS);
  glVertex2i(x * PIXEL_SCALE + 2, y * PIXEL_SCALE + 2);
  glEnd();
}
 
void do_frame() {

  // Set color to red
  unsigned char* data = malloc(sizeof(unsigned char)*100*100*3);
  for (int y = 0; y < 100; ++y) {
    for (int x = 0; x < 100; ++x) {
      data[y * 100 * 3 + x * 3   ] = 0xff;
      data[y * 100 * 3 + x * 3 +1] = 0x00;
      data[y * 100 * 3 + x * 3 +2] = 0x00;
    }
  }
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
  pixel_put(10, 10, 0xff, 0xff, 0x00);

  // Swap front and back buffers
  glfwSwapBuffers(window);

  // Poll for and process events
  glfwWaitEvents();
}
 
void shutdown_gl() {
    glfwTerminate();
}