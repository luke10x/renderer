#define GL_SILENCE_DEPRECATION


#ifdef __EMSCRIPTEN__

// Emscripten
#include <GLFW/glfw3.h>
#include <GL/gl.h> // Include OpenGL header
#include <GL/glut.h>

#include <emscripten/emscripten.h>
#else
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#endif

#include "display.h"
#include "dataload.h"

#include "debug.c"
#include "dataload.c"
#include "cast.c"

#define pixelScale 4               // OpenGL pixel scale
#define GLSW       (SW*pixelScale) // OpenGL window width
#define GLSH       (SH*pixelScale) // OpenGL window height

typedef struct {
  int fr1, fr2;  // frame1 frame2, to create constant frame rate
} time;
time T;

typedef struct {
  int w, s, a, d; // move up, down, left, right
  int sl, sr;     // strafe left, right
  int m;          // move up, down, look up, down
} keys;
keys K;

typedef struct {
  float cos[360];
  float sin[360];
} math;
math M;

typedef struct {
  int x, y, z;
  int a;
  int l;
} player; 
player P;

t_face* faces = NULL;
int NUM_FACES = 0;

void pixel_put(int x, int y, int r, int g, int b) {
  glColor3ub(r, g, b);
  glBegin(GL_POINTS);
  glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
  glEnd();
}

void movePlayer() {
  // turn left, right
  if (K.a == 1 && K.m == 0) { 
    P.a-=4;
    if (P.a < 0) { P.a += 360; } 
    printf("left\n"); 
  }
  if (K.d == 1 && K.m == 0) {
    P.a+=4;
    if (P.a > 359) { P.a -= 360; } 
    printf("right\n");
  }

  int dx = M.sin[P.a] * 10.0;
  int dy = M.cos[P.a] * 10.0;

  // move forward and back
  if (K.w == 1 && K.m == 0) { P.x+=dx; P.y+=dy; printf("fwd\n"); }
  if (K.s == 1 && K.m == 0) { P.x-=dx; P.y-=dy; printf("back\n"); }

  // strafe left, right
  if (K.sr == 1) { P.x+=dy; P.y-=dx; printf("strafe left\n"); }
  if (K.sl == 1) { P.x-=dy; P.y+=dx; printf("strafe right\n"); } 
  // move up, down, look up, look down
  if (K.a == 1 && K.m == 1) { P.l-=1; printf("look up\n"); }
  if (K.d == 1 && K.m == 1) { P.l+=1; printf("look down\n"); }
  if (K.w == 1 && K.m == 1) { P.z+=4; printf("move up\n"); }
  if (K.s == 1 && K.m == 1) { P.z-=4; printf("move down\n"); }

  // printf("X=%d Y=%d Z=%d a=%d l=%d \n", P.x, P.y, P.z, P.a, P.l);
}


int myt;
void display() { 
  int x, y, t;
  if (T.fr1 - T.fr2 >= 50) { // Only draw 20 frames per second

    sky_render(SW, SH);
    movePlayer();

    float x = P.x;
    float y = P.y;
    float z = P.z;

    float rotation_angle = P.a * M_PI / 180.0;
    float head_lift = P.l * M_PI / 180.0;

    project(SW, SH, x, y, z, rotation_angle, head_lift, faces, NUM_FACES);

    // myt++;
    // testTextures(faces, (myt / 20) % NUM_FACES);

    T.fr2 = T.fr1;

    glutSwapBuffers();
    glutReshapeWindow(GLSW, GLSH); // Prevent window scaling
  }

  T.fr1 = glutGet(GLUT_ELAPSED_TIME);
  glutPostRedisplay();
}


void KeysDown(unsigned char key, int x, int y) {
  printf("key %d", key);
  if (key=='w') { K.w  = 1; }
  if (key=='s') { K.s  = 1; }
  if (key=='a') { K.a  = 1; }
  if (key=='d') { K.d  = 1; }
  if (key=='m') { K.m  = 1; }
  if (key==',') { K.sl = 1; }
  if (key=='.') { K.sr = 1; }
}


void KeysUp(unsigned char key, int x, int y) {
  if (key=='w') { K.w  = 0; }
  if (key=='s') { K.s  = 0; }
  if (key=='a') { K.a  = 0; }
  if (key=='d') { K.d  = 0; }
  if (key=='m') { K.m  = 0; }
  if (key==',') { K.sl = 0; }
  if (key=='.') { K.sr = 0; }
}


void init() {
  int x;
  // store sin/cos degrees
  for (x = 0; x < 360; x++ ) {
    M.cos[x] = cos(x / 180.0 * M_PI);
    M.sin[x] = sin(x / 180.0 * M_PI);
  }

  // init character
  P.x = 0; P.y = 0; P.z=20; P.a = 0; P.l = 0;

  // init faces
  NUM_FACES = faces_load_from_file("assets/export/cube.obj", &faces);
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(GLSW/2, GLSW/2);
  glutInitWindowSize(GLSW, GLSH);
  glutCreateWindow("window title");
  glPointSize(pixelScale);
  gluOrtho2D(0, GLSW, 0, GLSH);

  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(KeysDown);
  glutKeyboardUpFunc(KeysUp);
  glutMainLoop();
  return 0;
}