
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


int init_gl() {
  const int width = 320,
            height = 240;

  if (glfwInit() != GL_TRUE) {
      printf("glfwInit() failed\n");
      return GL_FALSE;
  }

  #ifdef __APPLE__
    /* We need to explicitly ask for a 3.2 context on OS X */
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #endif

  window = glfwCreateWindow( width, height, "Hello World", NULL, NULL );
  if (!window) {
    glfwTerminate();
    return GL_FALSE;
  }

  glfwMakeContextCurrent(window);

  return GL_TRUE;
}
 
void do_frame() {
  /* Render here */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Swap front and back buffers */
  glfwSwapBuffers(window);

  /* Poll for and process events */
  glfwPollEvents();
}
 
void shutdown_gl() {
    glfwTerminate();
}