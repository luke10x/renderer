
#include <stdlib.h>
#include <stdio.h>

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

#include "dataload.h"
#include "dataload.c"
#include "cast.c"

#define SW          64
#define SH          64
#define PIXEL_SCALE 4           // OpenGL pixel scale
#define GLSW        (SW*PIXEL_SCALE) // OpenGL window width
#define GLSH        (SH*PIXEL_SCALE) // OpenGL window height

int init_gl();
void do_frame();
void shutdown_gl();

typedef struct {
  float cos[360];
  float sin[360];
} math;
math M;

GLFWwindow* window;

typedef struct {
  int x, y, z;
  int rot_h, rot_v;
} t_camera;
t_camera cam;

t_face* faces = NULL;
int NUM_FACES = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void cam_move() {
  int dx = M.sin[cam.rot_h] * 10.0;
  int dy = M.cos[cam.rot_h] * 10.0;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cam.x-=dy; cam.y+=dx; printf("strafe right\n");
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cam.x+=dy; cam.y-=dx; printf("strafe left\n");
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cam.x+=dx; cam.y+=dy; printf("fwd\n");
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam.x-=dx; cam.y-=dy; printf("back\n");
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    cam.rot_v-=1; printf("look up\n");
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    cam.rot_v+=1; printf("look down\n");
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    cam.rot_h-=4;
    if (cam.rot_h < 0) { cam.rot_h += 360; } 
    printf("left\n"); 
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    cam.rot_h+=4;
    if (cam.rot_h > 359) { cam.rot_h -= 360; } 
    printf("right\n");
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      cam.z+=4; printf("move up\n");
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      cam.z-=4; printf("move down\n");
    }
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    printf("size resetted = %dx%d\n", width, height);

    glViewport(0, 0, width, height);
}

void pixel_put(int x, int y, int r, int g, int b) {
  glColor3ub(r, g, b);
  glBegin(GL_POINTS);
  glVertex2i(x * PIXEL_SCALE + 2, y * PIXEL_SCALE + 2);
  glEnd();
}
 
void do_frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  sky_render(SW, SH);
  cam_move();

  float rotation_angle = cam.rot_h * M_PI / 180.0;
  float head_lift = cam.rot_v * M_PI / 180.0;
  
  project(SW, SH, cam.x, cam.y, cam.z, rotation_angle, head_lift, faces, NUM_FACES);

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

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);

  glLoadIdentity();
  glOrtho(0, GLSW, 0, GLSH, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPointSize(PIXEL_SCALE);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);

  return GL_TRUE;
}

int main() {
  printf("Started\n");
  int x;

    // store sin/cos degrees
  for (x = 0; x < 360; x++ ) {
    M.cos[x] = cos(x / 180.0 * M_PI);
    M.sin[x] = sin(x / 180.0 * M_PI);
  }
  if (init_gl() == GL_TRUE) {
    // Set camera position
    cam.x = 0; cam.y = 0; cam.z=20; cam.rot_h = 0; cam.rot_v = 0;

    // init faces
    NUM_FACES = faces_load_from_file("assets/export/cube.obj", &faces);
#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop(do_frame, fps, 1);
#else
    while (!glfwWindowShouldClose(window)) do_frame();
#endif
  }
  printf("Ended\n");
 
  shutdown_gl();
  return 0;
}