#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

typedef struct {
  float x1, y1, z1; // Coordinates of first vertex
  float x2, y2, z2; // Coordinates of second vertex
  float x3, y3, z3; // Coordinates of third vertex
  int c;
  float us, vs;
} Triangle;
//  x,  y,  z

Triangle triangles[] = {
  {
    0,   200,  30,
    0,   200,   0,
    100, 200,   0,
    1, 1, 1 // color, u-scale, v-scale
  }, {
    0,   250,   0,
    0,   250,  30,
    100, 250,   0,
    2 , 1, 1 // color, u-scale, v-sclae
  },{
    200, 0,   30,
    200, 0,    0,
    200, 100,  0,
    2 , 1, 1// color
  },
  
  { //slant wall back
     50,   150,  70,
    -10,   150,  70,
     60,   150,   0,
     0 , -1, -3// color
  },{ ///front
     0,    150,   0,
    60,   150,    0,
    -10,   150,  70,
    0 , 1, 3
  },

  {
    // water
    512, 0, 0,
    0, 0,   0,
    0, 512, 0,
    6, 16, 16
  },
  {
    // water2
    512, 0, 0,
    512, 512,   0,
    0, 512, 0,
    6, -16, -16
  },

  {
    // island grass
    480, 20, 10,
    20, 20,   10,
    20, 480, 10,
    1, 46, 46
  },
  {
    // island grass
    480, 20, 10,
    480, 480,   10,
    20, 480, 10,
    1, -46, -46
  }

,
   {
    // island bank south
     480, 20,  0,
      20, 20,  0,
     480, 20, 10,
        0, 46, 1
  },
  {
    // island bank south
     20,  20,  10,
     480, 20,  10,
     20 , 20,   0,
    0, -46, -1
  }
  ,
     {
    // island bank west
     20, 480,  0,
     20,  20,  0,
     20, 480, 10,
        0, 46, 1
  },
  {
    // island bank west
     20, 20,    10,
     20, 480,   10,
     20, 20 ,    0,
    0,   -46,     -1
  },
     {
    // island bank north
     480, 480,  0,
      20, 480,  0,
     480, 480, 10,
        0, 46, 1
  },
  {
    // island bank north
     20,  480,  10,
     480, 480,  10,
     20 , 480,   0,
    0, -46, -1
  }
  ,     {
    // island bank west
     480, 480,  0,
     480,  20,  0,
     480, 480, 10,
        0, 46, 1
  },
  {
    // island bank west
     480, 20,    10,
     480, 480,   10,
     480, 20 ,    0,
    0,   -46,     -1
  },
}; 
#define NUM_FACES 17

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

void drawPixel(int x, int y, int r, int g, int b) {
  glColor3ub(r, g, b);
  glBegin(GL_POINTS);
  glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
  glEnd();
}

void clearBackground() {
  int x, y;
  for (y = 0; y < SH; y++) {
    for (x = 0; x < SW; x++) {
      drawPixel(x, y, 0, 61, 30 );
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
  if (K.w == 1 && K.m == 1) { P.z+=4; printf("move up\n"); }
  if (K.s == 1 && K.m == 1) { P.z-=4; printf("move down\n"); }

  printf("X=%d Y=%d Z=%d a=%d l=%d \n", P.x, P.y, P.z, P.a, P.l);
}

void testTextures(int t) {
  int x, y;
  for (y = 0; y < Textures[t].h; y++) {
    for (x = 0; x < Textures[t].w; x++) {
      int pixel = (Textures[t].h - y -1) * 3 * Textures[t].w + x * 3;

      int r = Textures[t].name[pixel + 0];
      int g = Textures[t].name[pixel + 1]; 
      int b = Textures[t].name[pixel + 2];
      drawPixel(x, y, r, g, b);
    }
  }
}

#define EPSILON 0.000001

Triangle* raycast(float x, float y, float z, float dir_x, float dir_y, float dir_z, Triangle *triangles, int num_triangles,
// int* u_ret, int* v_ret
unsigned char* r, unsigned char* g, unsigned char* b
) {
    float min_distance = INFINITY;
    Triangle* closest_face = NULL;
    for (int i = 0; i < num_triangles; i++) {
        float x1 = triangles[i].x1;
        float y1 = triangles[i].y1;
        float z1 = triangles[i].z1;
        float x2 = triangles[i].x2;
        float y2 = triangles[i].y2;
        float z2 = triangles[i].z2;
        float x3 = triangles[i].x3;
        float y3 = triangles[i].y3;
        float z3 = triangles[i].z3;

        float e1x = x2 - x1;
        float e1y = y2 - y1;
        float e1z = z2 - z1;
        float e2x = x3 - x1;
        float e2y = y3 - y1;
        float e2z = z3 - z1;
        float px = dir_y * e2z - dir_z * e2y;
        float py = dir_z * e2x - dir_x * e2z;
        float pz = dir_x * e2y - dir_y * e2x;
        float det = e1x * px + e1y * py + e1z * pz;
        if (det > -EPSILON && det < EPSILON) {
            continue;
        }
        float inv_det = 1.0f / det;
        float tx = x - x1;
        float ty = y - y1;
        float tz = z - z1;
        float u = (tx * px + ty * py + tz * pz) * inv_det;
        if (u < 0.0f || u > 1.0f) {
            continue;
        }
        float qx = ty * e1z - tz * e1y;
        float qy = tz * e1x - tx * e1z;
        float qz = tx * e1y - ty * e1x;
        float v = (dir_x * qx + dir_y * qy + dir_z * qz) * inv_det;
        if (v < 0.0f || u + v > 1.0f) {
            continue;
        }
        float t = (e2x * qx + e2y * qy + e2z * qz) * inv_det;
        if (t < 0.0f || t > min_distance) {
            continue;
        }
        min_distance = t;
        closest_face = &triangles[i];

        int wt = closest_face->c;

        float us = closest_face->us; if (us < 0) { us *= -1; }
        float vs = closest_face->vs; if (vs < 0) { vs *= -1; }

        u *= Textures[wt].h * us;
        v *= Textures[wt].w * vs;

        int pixel;
        if (closest_face->us > 0) {
          pixel = (int) (Textures[wt].h - ((int) v % Textures[wt].h)-1)
            * 3 * Textures[wt].w
            + 
            ((int) u % Textures[wt].w)
            * 3;
        } else {
          pixel = 
            (int) v % Textures[wt].h
            * 3 * Textures[wt].w
            + 
            (int) (Textures[wt].w - ((int) u % Textures[wt].w)-1)
            * 3;
        }

        *r = Textures[wt].name[pixel + 0]; if (r < 0) { r = 0; }
        *g = Textures[wt].name[pixel + 1]; if (g < 0) { g = 0; }
        *b = Textures[wt].name[pixel + 2]; if (b < 0) { b = 0; }
        // printf("text:.(u=%f; v=%f)\n", u, v);
    }
    return closest_face;
}

void project(float x, float y, float z, float rotation_z, float rotation_x, Triangle *triangles) {
    int u, v;
    u_int8_t r, g, b;

    // Calculate the field of view in radians
    float fov = 60.0f * M_PI / 180.0f;
    // Calculate the angle step size for both horizontal and vertical directions
    float angle_step = fov / SW;
    // Calculate the vertical angle step size based on the aspect ratio
    float aspect_ratio = (float)SH / (float)SW;
    float v_angle_step = angle_step * aspect_ratio;
    // Calculate the initial horizontal and vertical angles
    float h_angle = -(fov / 2.0f);
    float v_angle = -(fov / 2.0f) * aspect_ratio;

    // Loop through every pixel in the screen
    for (int i = 0; i < SW; i++) {
        for (int j = 0; j < SH; j++) {

            // Calculate the direction vector based on the current horizontal and vertical angles and rotation
            float dir_y = cos(h_angle) * cos(v_angle);
            float dir_x = sin(v_angle);
            float dir_z = sin(h_angle) * cos(v_angle);

            // Rotate the direction vector around the x-axis
            float cos_rot_x = cos(-rotation_x);
            float sin_rot_x = sin(-rotation_x);
            float new_dir_y = dir_y * cos_rot_x - dir_z * sin_rot_x;
            float new_dir_z = dir_y * sin_rot_x + dir_z * cos_rot_x;

            // Rotate the direction vector around the z-axis
            float cos_rot_z = cos(-rotation_z);
            float sin_rot_z = sin(-rotation_z);
            float new_dir_x = dir_x * cos_rot_z - new_dir_y * sin_rot_z;
            new_dir_y = dir_x * sin_rot_z + new_dir_y * cos_rot_z;

            // Normalize the direction vector
            float length = sqrt(new_dir_x * new_dir_x + new_dir_y * new_dir_y + new_dir_z * new_dir_z);
            new_dir_x /= length;
            new_dir_y /= length;
            new_dir_z /= length;

            // Call the raycast function for the current pixel and direction
            Triangle* found = raycast(
              x, y, z,
              new_dir_x, new_dir_y, new_dir_z, 
              triangles, NUM_FACES,
              &r, &g, &b
            );
            if (found != 0) { 
              // int wt = 0;
              drawPixel(i, j, r, g, b);
            }

            // Increment the horizontal angle by the angle step size
            h_angle += angle_step;
        }
        // Increment the vertical angle by the angle step size
        v_angle += v_angle_step;
        // Reset the horizontal angle to the initial value
        h_angle = -(fov / 2.0f);
    }
}


int myt;
void display() { 
  int x, y, t;
  if (T.fr1 - T.fr2 >= 50) { // Only draw 20 frames per second

    clearBackground();
    movePlayer();

    float x = P.x;
    float y = P.y;
    float z = P.z;

    // float rx = M_PI*0.5;
    float ry = M_PI*0.5;
    float rz = M_PI*0.5;

    float rx = 0;
    float rotation_angle = P.a * M_PI / 180.0;
    float head_lift = P.l * M_PI / 180.0;

    project(x, y, z, rotation_angle, head_lift, &triangles );

    // myt++;
    // testTextures((myt / 20) % numText);

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
  for (x = 0; x < 360; x++ ) {
    M.cos[x] = cos(x / 180.0 * M_PI);
    M.sin[x] = sin(x / 180.0 * M_PI);
  }

  // init character
  P.x = 100; P.y = 110; P.z=20; P.a = 0; P.l = 0;

 // define textures
 Textures[ 0].name = T_00; Textures[ 0].h = T_00_HEIGHT; Textures[ 0].w = T_00_WIDTH;
 Textures[ 1].name=T_01; Textures[ 1].h=T_01_HEIGHT; Textures[ 1].w=T_01_WIDTH;
 Textures[ 2].name=T_02; Textures[ 2].h=T_02_HEIGHT; Textures[ 2].w=T_02_WIDTH;
 Textures[ 3].name=T_03; Textures[ 3].h=T_03_HEIGHT; Textures[ 3].w=T_03_WIDTH;
 Textures[ 4].name=T_04; Textures[ 4].h=T_04_HEIGHT; Textures[ 4].w=T_04_WIDTH;
 Textures[ 5].name=T_05; Textures[ 5].h=T_05_HEIGHT; Textures[ 5].w=T_05_WIDTH;
 Textures[ 6].name=T_06; Textures[ 6].h=T_06_HEIGHT; Textures[ 6].w=T_06_WIDTH;
 Textures[ 7].name=T_07; Textures[ 7].h=T_07_HEIGHT; Textures[ 7].w=T_07_WIDTH;
 Textures[ 8].name=T_08; Textures[ 8].h=T_08_HEIGHT; Textures[ 8].w=T_08_WIDTH;
 Textures[ 9].name=T_09; Textures[ 9].h=T_09_HEIGHT; Textures[ 9].w=T_09_WIDTH;
 Textures[10].name=T_10; Textures[10].h=T_10_HEIGHT; Textures[10].w=T_10_WIDTH;
 Textures[11].name=T_11; Textures[11].h=T_11_HEIGHT; Textures[11].w=T_11_WIDTH;
 Textures[12].name=T_12; Textures[12].h=T_12_HEIGHT; Textures[12].w=T_12_WIDTH;
 Textures[13].name=T_13; Textures[13].h=T_13_HEIGHT; Textures[13].w=T_13_WIDTH;
 Textures[14].name=T_14; Textures[14].h=T_14_HEIGHT; Textures[14].w=T_14_WIDTH;
 Textures[15].name=T_15; Textures[15].h=T_15_HEIGHT; Textures[15].w=T_15_WIDTH;
 Textures[16].name=T_16; Textures[16].h=T_16_HEIGHT; Textures[16].w=T_16_WIDTH;
 Textures[17].name=T_17; Textures[17].h=T_17_HEIGHT; Textures[17].w=T_17_WIDTH;
 Textures[18].name=T_18; Textures[18].h=T_18_HEIGHT; Textures[18].w=T_18_WIDTH;
 Textures[19].name=T_19; Textures[19].h=T_19_HEIGHT; Textures[19].w=T_19_WIDTH;

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