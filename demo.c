#define GL_SILENCE_DEPRECATION

#include <math.h>
#include <stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define res        1               // 0=160x120 1=360x240 4=640=480
#define SW         160*res         // screen width
#define SH         120*res         // screen height
#define SW2        (SW/2)          // half of screen width
#define SH2        (SH/2)          // half of screen height
#define pixelScale 4/res           // OpenGL pixel scale
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

void pixel(int x, int y, int c) {
  int rgb[3];
  if (c == 0) { rgb[0] = 255; rgb[1] = 255; rgb[2] =   0; } //Yellow	
  if (c == 1) { rgb[0] = 160; rgb[1] = 160; rgb[2] =   0; } //Yellow darker	
  if (c == 2) { rgb[0] =   0; rgb[1] = 255; rgb[2] =   0; } //Green	
  if (c == 3) { rgb[0] =   0; rgb[1] = 160; rgb[2] =   0; } //Green darker	
  if (c == 4) { rgb[0] =   0; rgb[1] = 255; rgb[2] = 255; } //Cyan	
  if (c == 5) { rgb[0] =   0; rgb[1] = 160; rgb[2] = 160; } //Cyan darker
  if (c == 6) { rgb[0] = 160; rgb[1] = 100; rgb[2] =   0; } //brown	
  if (c == 7) { rgb[0] = 110; rgb[1] =  50; rgb[2] =   0; } //brown darker
  if (c == 8) { rgb[0] =   0; rgb[1] =  60; rgb[2] = 130; } //background 
  glColor3ub(rgb[0], rgb[1], rgb[2]);
  glBegin(GL_POINTS);
  glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
  glEnd();
}

void clearBackground() {
  int x, y;
  for (y = 0; y < SH; y++) {
    for (x = 0; x < SW; x++) {
      pixel(x, y, 8);
    }
  }
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
  if (K.w == 1 && K.m == 1) { P.z-=4; printf("move up\n"); }
  if (K.s == 1 && K.m == 1) { P.z+=4; printf("move down\n"); }
}

void clipBehindPlayer(
  int* x1, int* y1, int* z1, int x2, int y2, int z2
) {
  float da = *y1;
  float db = y2;
  float d = da -db; if (d == 0) { d = 1; }
  float s = da / (da - db);

  *x1 = *x1 + s * (x2 - (*x1));
  *y1 = *y1 + s * (y2 - (*y1)); if (*y1 == 0) { *y1 = 1; }
  *z1 = *z1 + s * (z2 - (*z1)); 
}
void drawWall(int x1, int x2, int b1, int b2, int t1, int t2) {
  int x, y;
  // hold difference in distance 
  int dyb = b2 - b1; // y distance of bottom line
  int dyt = t2-t1;   // y distance of top line
  int dx  = x2 - x1; // x distance
  if (dx == 0) { dx = 1; } 
  int xs = x1;        // hold inital x1 starting position

  // Clip X
  if (x1 <      1) { x1 = 1; }
  if (x2 <      1) { x2 = 1; }
  if (x1 > SW - 1) { x1 = SW - 1; }
  if (x2 > SW - 1) { x2 = SW - 1; }

  // draw x verticle lines
  for (x = x1; x < x2; x++) {
    // the Y start and end point
    int y1 = dyb * (x - xs + 0.5) / dx + b1;
    int y2 = dyt * (x - xs + 0.5) / dx + t1;

    // Clip Y
    if (y1 <      1) { y1 = 1; }
    if (y2 <      1) { y2 = 1; }
    if (y1 > SH - 1) { y1 = SH - 1; }
    if (y2 > SH - 1) { y2 = SH - 1; }


    pixel(x, y1, 0); // bottom
    pixel(x, y2, 0); // bottom

    for (y = y1; y < y2; y++) {
      pixel(x, y, 0);
    }
  }


}
void draw3D() {
  int wx[4], wy[4], wz[4];
  float CS = M.cos[P.a], SN = M.sin[P.a];
  // offset bottom 2 points by player
  int x1 = 40 - P.x, y1 =  10 - P.y;
  int x2 = 40 - P.x, y2 = 190 - P.y;

  // World X position
  wx[0] = x1 * CS - y1 * SN;
  wx[1] = x2 * CS - y2 * SN;
  wx[2] = wx[0]; // top line has the same x
  wx[3] = wx[1];

  // World Y position (depth)
  wy[0] = y1 * CS + x1 * SN;
  wy[1] = y2 * CS + x2 * SN;
  wy[2] = wy[0]; // top line has the same y
  wy[3] = wy[1];

  // World Z position (height)
  wz[0] = 0 - P.z /* next for looks */ + ((P.l * wy[0]) / 32.0);
  wz[1] = 0 - P.z /* next for looks */ + ((P.l * wy[1]) / 32.0);
  wz[2] = wz[0] + 40;
  wz[3] = wz[1] + 40;

  // don't draw if behind the player
  if (wy[0] < 1 && wy[1] < 1) { return; }
  // point 1 behind the player, clip
  if (wy[0] < 1) {
    clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]); // bottom line
    clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]); // top line
  }
  // point 2 behind player, clip
  if (wy[1] < 1) {
    clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]); // bottom line
    clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]); // top line
  
  }


  // projected screen x, screen y position
  wx[0] = wx[0] * 200 / wy[0] + SW2; wy[0] = wz[0] * 200 / wy[0] + SH2;
  wx[1] = wx[1] * 200 / wy[1] + SW2; wy[1] = wz[1] * 200 / wy[1] + SH2;
  wx[2] = wx[2] * 200 / wy[2] + SW2; wy[2] = wz[2] * 200 / wy[2] + SH2;
  wx[1] = wx[3] * 200 / wy[3] + SW2; wy[3] = wz[3] * 200 / wy[3] + SH2;

  // draw points
  // if (wx[0] > 0  && wx[0] < SW && wy[0] > 0 && wy[0] < SH) {
  //   pixel(wx[0], wy[0], 0);
  // }

  // if (wx[1] > 0  && wx[1] < SW && wy[1] > 0 && wy[1] < SH) {
  //    pixel(wx[1], wy[1], 0);
  // }
  drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3]);
}

void display() {
  int x, y;
  if (T.fr1 - T.fr2 >= 50) { // Only draw 20 frames per second

    clearBackground();
    movePlayer();
    draw3D();

    T.fr2 = T.fr1;

    glutSwapBuffers();
    glutReshapeWindow(GLSW, GLSH); // Prevent window scaling
  }

  T.fr1 = glutGet(GLUT_ELAPSED_TIME);
  glutPostRedisplay();
}

void KeysDown(unsigned char key, int x, int y) {
  if (key=='w'==1) { K.w  = 1; }
  if (key=='s'==1) { K.s  = 1; }
  if (key=='a'==1) { K.a  = 1; }
  if (key=='d'==1) { K.d  = 1; }
  if (key=='m'==1) { K.m  = 1; }
  if (key==','==1) { K.sl = 1; }
  if (key=='.'==1) { K.sr = 1; }
}

void KeysUp(unsigned char key, int x, int y) {
  if (key=='w'==1) { K.w  = 0; }
  if (key=='s'==1) { K.s  = 0; }
  if (key=='a'==1) { K.a  = 0; }
  if (key=='d'==1) { K.d  = 0; }
  if (key=='m'==1) { K.m  = 0; }
  if (key==','==1) { K.sl = 0; }
  if (key=='.'==1) { K.sr = 0; }
}

void init() {
  int x;
  // store sin/cos degrees
  for (x = 0; x < 360; x++) {
    M.cos[x] = cos(x / 180.0 * M_PI);
    M.sin[x] = sin(x / 180.0 * M_PI);
  }

  // init character
  P.x = 70; P.y = -110; P.z=20; P.a = 0; P.l = 0;
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