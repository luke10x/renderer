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


//textures
#include "textures/T_NUMBERS.h"
#include "textures/T_VIEW2D.h"
#include "textures/T_00.h"
#include "textures/T_01.h"
#include "textures/T_02.h"
#include "textures/T_03.h"
#include "textures/T_04.h"
#include "textures/T_05.h"
#include "textures/T_06.h"
#include "textures/T_07.h"
#include "textures/T_08.h"
#include "textures/T_09.h"
#include "textures/T_10.h"
#include "textures/T_11.h"
#include "textures/T_12.h"
#include "textures/T_13.h"
#include "textures/T_14.h"
#include "textures/T_15.h"
#include "textures/T_16.h"
#include "textures/T_17.h"
#include "textures/T_18.h"
#include "textures/T_19.h"
int numText=19;                          //number of textures
int numSect= 0;                          //number of sectors
int numWall= 0;                          //number of walls



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

typedef struct {
  int x1, y1; // Bottom line point1
  int x2, y2; // Bottom line point2
  int c;      // wall color 
  int wt,u,v; // wall texture and u/b tile
  int shade;  // shade of the wall
} walls;

walls W[256];

typedef struct {
  int ws, we; // wall number start and end;
  int z1, z2; // height of bottom and top;
  // int x, y;   // center position for sector
  int d;      // add y distances to sort drawing order
  int c1, c2;    // bottom and top color
  int st, ss;    // surface texture,  surface scale
  int surf[SW];  // to hold point for surface
  int surface;   // is there surface to draw
} sectors;
sectors S[128];

typedef struct 
{
 int w,h;                             //texture width/height
 const unsigned char *name;           //texture name
} TexureMaps; TexureMaps Textures[64]; //increase for more textures


void load()
{
 FILE *fp = fopen("level.h","r");
 if(fp == NULL){ printf("Error opening level.h"); return;}
 int s,w;

 fscanf(fp,"%i",&numSect);   //number of sectors 
 for(s=0;s<numSect;s++)      //load all sectors
 {
  fscanf(fp,"%i",&S[s].ws);  
  fscanf(fp,"%i",&S[s].we); 
  fscanf(fp,"%i",&S[s].z1);  
  fscanf(fp,"%i",&S[s].z2); 
  fscanf(fp,"%i",&S[s].st); 
  fscanf(fp,"%i",&S[s].ss);  
 }
 fscanf(fp,"%i",&numWall);   //number of walls 
 for(s=0;s<numWall;s++)      //load all walls
 {
  fscanf(fp,"%i",&W[s].x1);  
  fscanf(fp,"%i",&W[s].y1); 
  fscanf(fp,"%i",&W[s].x2);  
  fscanf(fp,"%i",&W[s].y2); 
  fscanf(fp,"%i",&W[s].wt);
  fscanf(fp,"%i",&W[s].u); 
  fscanf(fp,"%i",&W[s].v);  
  fscanf(fp,"%i",&W[s].shade);  
 }
 fscanf(fp,"%i %i %i %i %i",&P.x,&P.y,&P.z, &P.a,&P.l); //player position, angle, look direction 
 fclose(fp); 
}

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
void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int s, int w, int frontBack) {
  int x, y;
  // hold difference in distance 
  int dyb = b2 - b1; // y distance of bottom line
  int dyt = t2-t1;   // y distance of top line
  int dx  = x2 - x1; // x distance
  if (dx == 0) { dx = 1; } 
  int xs = x1;        // hold inital x1 starting position

  // Clip X
  if (x1 <  0) { x1 = 0; }
  if (x2 <  0) { x2 = 0; }
  if (x1 > SW) { x1 = SW; }
  if (x2 > SW) { x2 = SW; }

  // draw x verticle lines
  for (x = x1; x < x2; x++) {
    // the Y start and end point
    int y1 = dyb * (x - xs + 0.5) / dx + b1;
    int y2 = dyt * (x - xs + 0.5) / dx + t1;

    // Clip Y
    if (y1 <   0) { y1 = 0; }
    if (y2 <   0) { y2 = 0; }
    if (y1 > SH) { y1 = SH; }
    if (y2 > SH) { y2 = SH; }

    
    // draw front facing walls
    if (frontBack == 0) {
      if (S[s].surface ==  1) { S[s].surf[x] = y1; } // bottom surface save top row
      if (S[s].surface ==  2) { S[s].surf[x] = y2; } // top surface save top row
      for (y = y1; y < y2; y++) { pixel(x, y, 0); }
    }
    // draw back wall and surface
    if (frontBack == 1) {
      if (S[s].surface ==  1) { y2 = S[s].surf[x]; }
      if (S[s].surface ==  2) {y1 = S[s].surf[x]; }
      for (y = y1; y < y2; y++) { pixel(x, y, 2); }

    }
    // normal wall
    // for (y = y1; y < y2; y++) {
    //   pixel(x, y, c);
    // }
  }
}

int dist (int x1, int y1, int x2, int y2) {
  int distance = sqrt (
    (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)
  );
  return distance;
}

void draw3D() {
  int x, s, w, frontBack, cycles;
  int wx[4], wy[4], wz[4];
  float CS = M.cos[P.a], SN = M.sin[P.a];

  // order sectors by distance
  for (s = 0; s < numSect - 1; s++) {
    for (w = 0; w < numSect - s - 1; w++) {
      if (S[w].d < S[w+1].d) {
        sectors st = S[w];
        S[w] = S[w+1];
        S[w+1] = st;
      }
    }
  }

  // Draw sectors
  for (s = 0; s < numSect; s++) {
    S[s].d = 0;          // Clear distance

    if      (P.z < S[s].z1) { S[s].surface = 1; cycles=2; for(x=0; x<SW; x++) { S[s].surf[x]=SH; } } // bottom surface
    else if (P.z > S[s].z2) { S[s].surface = 2; cycles=2; for(x=0; x<SW; x++) { S[s].surf[x]= 0; }} // top    surface
    else                    { S[s].surface = 0; cycles=1; } // no     surface

    for (frontBack = 0; frontBack < cycles; frontBack++) {
      for (w = S[s].ws; w < S[s].we; w++) {

        // offset bottom 2 points by player
        // int x1 = 40 - P.x, y1 =  10 - P.y;
        // int x2 = 40 - P.x, y2 = 190 - P.y;
        int x1 = W[w].x1 - P.x,
            y1 = W[w].y1 - P.y;
        int x2 = W[w].x2 - P.x, 
            y2 = W[w].y2 - P.y;

        // swap for surface
        if (frontBack == 1) {
          int swp = x1; x1 = x2; x2 = swp;
              swp = y1; y1 = y2; y2 = swp;
        }

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

        // Store this wall distance
        S[s].d += dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2); 

        // World Z position (height)
        wz[0] = S[s].z1 - P.z /* next for looks */ + ((P.l * wy[0]) / 32.0);
        wz[1] = S[s].z1 - P.z /* next for looks */ + ((P.l * wy[1]) / 32.0);
        wz[2] = S[s].z2 - P.z /* next for looks */ + ((P.l * wy[0]) / 32.0);
        wz[3] = S[s].z2 - P.z /* next for looks */ + ((P.l * wy[1]) / 32.0);

        // don't draw if behind the player
        if (wy[0] < 1 && wy[1] < 1) { continue; }
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

        drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], s, w, frontBack);
      }
      S[s].d /= (S[s].we - S[s].ws); // find average sector distance
      // S[s].surface *= -1;            // flip to negative to draw surface
    }
  }
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
  if (key=='w') { K.w  = 1; }
  if (key=='s') { K.s  = 1; }
  if (key=='a') { K.a  = 1; }
  if (key=='d') { K.d  = 1; }
  if (key=='m') { K.m  = 1; }
  if (key==',') { K.sl = 1; }
  if (key=='.') { K.sr = 1; }
  if (key==13) { load(); }
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
  for (x = 0; x < 360; x++) {
    M.cos[x] = cos(x / 180.0 * M_PI);
    M.sin[x] = sin(x / 180.0 * M_PI);
  }

  // init character
  P.x = 70; P.y = -110; P.z=20; P.a = 0; P.l = 0;

  // load sectors
  // int s, w, v1 = 0, v2 = 0;
  // for (s = 0; s < numSect; s++) {
  //   S[s].ws = loadSectors[v1 + 0]; // wall start number
  //   S[s].we = loadSectors[v1 + 1]; // wall end number
  //   S[s].z1 = loadSectors[v1 + 2]; // sector bottome height
  //   S[s].z2 = loadSectors[v1 + 3] - loadSectors[v1 + 2]; // sector top height
  //   S[s].c1 = loadSectors[v1 + 4]; // sector top color
  //   S[s].c2 = loadSectors[v1 + 5]; // sector bottom color
  //   v1 += 6;
  //   for (w = S[s].ws; w < S[s].we; w++) {
  //     W[w].x1 = loadWalls[v2 + 0]; // bottom x1
  //     W[w].y1 = loadWalls[v2 + 1]; // bottom x2
  //     W[w].x2 = loadWalls[v2 + 2]; // bottom y1
  //     W[w].y2 = loadWalls[v2 + 3]; // bottom y2
  //     W[w].c  = loadWalls[v2 + 4]; // wall color
  //     v2 += 5;
  //   }
  // }
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