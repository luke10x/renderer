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

typedef struct {
  float x1, y1, z1; // Coordinates of first vertex
  float x2, y2, z2; // Coordinates of second vertex
  float x3, y3, z3; // Coordinates of third vertex
} Face;

Face face = {
   100, 0,  100,

  -100, 0, -100,
  -100, 0,  100
};

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
  if (K.w == 1 && K.m == 1) { P.z-=4; printf("move up\n"); }
  if (K.s == 1 && K.m == 1) { P.z+=4; printf("move down\n"); }

  printf("X=%d Y=%d Z=%d a=%d l=%d \n", P.x, P.y, P.z, P.a, P.l);
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

  // wall texture
  int wt = W[w].wt  ;

  // Horizontal wall texture starting and step value
  float ht = 0, ht_step = (float)Textures[wt].w * W[w].u
                        / (float)(x2 - x1);

  // hold difference in distance 
  int dyb = b2 - b1; // y distance of bottom line
  int dyt = t2-t1;   // y distance of top line
  int dx  = x2 - x1; // x distance
  if (dx == 0) { dx = 1; } 
  int xs = x1;        // hold inital x1 starting position

  // Clip X
  if (x1 <  0) { ht -= ht_step * x1; x1 = 0; }
  if (x2 <  0) { x2 = 0; }
  if (x1 > SW) { x1 = SW; }
  if (x2 > SW) { x2 = SW; }

  // draw x verticle lines
  for (x = x1; x < x2; x++) {
    // the Y start and end point
    int y1 = dyb * (x - xs + 0.5) / dx + b1;
    int y2 = dyt * (x - xs + 0.5) / dx + t1;

    // Vertical wall texture starting and step value
    float vt = 0, vt_step = (float)Textures[wt].h  * W[w].v
                          / (float)(y2 - y1);

    // Clip Y
    if (y1 <  0) { vt -= vt_step * y1; y1 = 0; }
    if (y2 <  0) { y2 = 0; }
    if (y1 > SH) { y1 = SH; }
    if (y2 > SH) { y2 = SH; }

    
    // draw front facing walls
    if (frontBack == 0) {
      if (S[s].surface ==  1) { S[s].surf[x] = y1; } // bottom surface save top row
      if (S[s].surface ==  2) { S[s].surf[x] = y2; } // top surface save top row
      for (y = y1; y < y2; y++) { 

        int pixel = (int) (Textures[wt].h - ((int) vt % Textures[wt].h)-1)
            * 3 * Textures[wt].w
            + ((int) ht % Textures[wt].w) * 3;

        int r = Textures[wt].name[pixel + 0] - W[w].shade / 1.5; if (r < 0) { r = 0; }
        int g = Textures[wt].name[pixel + 1] - W[w].shade / 3; if (g < 0) { g = 0; }
        int b = Textures[wt].name[pixel + 2] - W[w].shade / 3; if (b < 0) { b = 0; }
        drawPixel(x, y, r, g, b);

        vt += vt_step;
      }
      ht += ht_step;
    }

    // draw back wall and surface
    if (frontBack == 1) {

      int xo = SW / 2; // x offset
      int yo = SH / 2; // y offset
      float fov = 200.0;
      int x2 = x - xo;
      int wo; // wall offset
      float tile = S[s].ss * 70;

      if (S[s].surface ==  1) { y2 = S[s].surf[x]; wo = S[s].z1; } // bottom surface
      if (S[s].surface ==  2) {y1 = S[s].surf[x]; wo = S[s].z2; }  // top surface
  
      float lookUpDown = P.l * 6.2;         if (lookUpDown > SH) { lookUpDown = SH;  }
      float moveUpDown = (P.z - wo) / yo; if (moveUpDown == 0) {  moveUpDown = 0.0001; }
      int ys=y1 - yo, ye = y2 - yo;
      
      // if (moveUpDown < 0) { ys = -lookUpDown; ye = yo + lookUpDown; }
  for (y = ys; y < ye; y++) { 
      float z = y + lookUpDown; if (z == 0) { z = 0.0001; }

      float fx =   x2   / z * moveUpDown * tile;                        // world floor
      float fy =   fov / z * moveUpDown * tile;  
 
      float rx = fx * M.sin[P.a] - fy * M.cos[P.a]  + (P.y/60.0 * tile);
      float ry = fx * M.cos[P.a] + fy * M.sin[P.a]  - (P.x/60.0 * tile);

      if (rx < 0) { rx = -rx + 1; }
      if (ry < 0) { rx = -ry + 1; }
      // if (rx <= 0 || ry <= 0 || rx >= 5 || ry <= 5) { continue; }

      int st = S[s].st;
              int pixel = (int) (Textures[wt].h - ((int) vt % Textures[wt].h)-1)
            * 3 * Textures[wt].w
            + ((int) ht % Textures[wt].w) * 3;

      int r = Textures[wt].name[pixel + 0]; 
      int g = Textures[wt].name[pixel + 1];
      int b = Textures[wt].name[pixel + 2];
      drawPixel(x2 + xo, y + yo, r, g, b);
  }
  ////

      // if (S[s].surface ==  1) { y2 = S[s].surf[x]; }
      // if (S[s].surface ==  2) {y1 = S[s].surf[x]; }
      // for (y = y1; y < y2; y++) { drawPixel(x, y, 255, 0, 0); }

    }

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

void floors() {
  int x, y;
  int xo = SW / 2; // x offset
  int yo = SH / 2; // y offset
  float fov = 200.0;
  float lookUpDown = P.l * 4; if (lookUpDown > SH) { lookUpDown = SH;  }
  float moveUpDown = P.z / 16.0; if (moveUpDown == 0) {  moveUpDown = 0.0001; }

  int ys=-yo, ye=-lookUpDown;
  if (moveUpDown < 0) { ys = -lookUpDown; ye = yo + lookUpDown; }

  for (y = ys; y < ye; y++) {
    for (x = -xo; x < xo; x++) {
      float z = y + lookUpDown; if (z == 0) { z = 0.0001; }

      float fx =   x   / z * moveUpDown;                        // world floor
      float fy =   fov / z * moveUpDown;
 
      float rx = fx * M.sin[P.a] - fy * M.cos[P.a]  + (P.y/30.0);
      float ry = fx * M.cos[P.a] + fy * M.sin[P.a]  - (P.x/30.0);

      if (rx < 0) { rx = -rx + 1; }
      if (ry < 0) { rx = -ry + 1; }
      if (rx <= 0 || ry <= 0 || rx >= 5 || ry <= 5) { continue; }


      if ((int) rx % 2 == (int) ry % 2) {
        drawPixel(x + xo, y + yo, 200, 50, 50); 
      } else { 
        drawPixel(x + xo, y + yo, 50, 200, 50);

      }
    }
  }
}


#define EPSILON 0.000001

typedef struct {
    float x, y, z;
} Point;

// typedef struct {
//     Point v1, v2, v3;
// } Face;

float dot_product(Point a, Point b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point cross_product(Point a, Point b) {
    Point result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Point subtract(Point a, Point b) {
    Point result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

float magnitude(Point a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

Point normalize(Point a) {
    Point result;
    float mag = magnitude(a);
    result.x = a.x / mag;
    result.y = a.y / mag;
    result.z = a.z / mag;
    return result;
}

Face* raycast(float x, float y, float z, float dir_x, float dir_y, float dir_z, Face *faces, int num_faces) {
    float min_distance = INFINITY;
    Face* closest_face = NULL;
    for (int i = 0; i < num_faces; i++) {
        float x1 = faces[i].x1;
        float y1 = faces[i].y1;
        float z1 = faces[i].z1;
        float x2 = faces[i].x2;
        float y2 = faces[i].y2;
        float z2 = faces[i].z2;
        float x3 = faces[i].x3;
        float y3 = faces[i].y3;
        float z3 = faces[i].z3;

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
        if (t < min_distance) {
            min_distance = t;
            closest_face = &faces[i];
        }
    }
    return closest_face;
}


void project(float x, float y, float z, float rx, float ry, float rz, Face *faces) {
    // Calculate the field of view in radians
    float fov = 90.0f * M_PI / 180.0f;
    // Calculate the angle step size for both horizontal and vertical directions
    float angle_step = fov / SW;
    // Calculate the vertical angle step size based on the aspect ratio
    float aspect_ratio = (float)SH / (float)SW;
    float v_angle_step = angle_step * aspect_ratio;
    // Calculate the initial horizontal and vertical angles
    float h_angle = -(fov / 2.0f);
    float v_angle = -(fov / 2.0f) * aspect_ratio;
    
    for (int j = 0; j < SW; j++) {
    // Loop through every pixel in the screen
        for (int i = 0; i < SH; i++) {
            // Calculate the direction vector based on the current horizontal and vertical angles
            float dir_x = cos(h_angle) * cos(v_angle);
            float dir_y = sin(v_angle);
            float dir_z = sin(h_angle) * cos(v_angle);
            // Normalize the direction vector
            float length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
            dir_x /= length;
            dir_y /= length;
            dir_z /= length;
            // Call the raycast function for the current pixel and direction
            Face* found = raycast(
              x, y, z,
              dir_x, dir_y, dir_z,
              // 0, 0, 1,
              // 0, 0.5, 0,
              faces, 1);

              // printf("faycast(%f, %f, %f, %f, %f, %f)", x, y, z,
              // dir_x, dir_y, dir_z);

            if (found == NULL) {} else {
            drawPixel( i,j, 200, 100, 100);

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
    // floors();
    draw3D();
    // drawRcast();

    float x = P.x;
    float y = P.y;
    float z = P.z;

    float rx = M_PI*0.5;
    float ry = M_PI*0.5;
    float rz = M_PI*0.5;


    project(x, y, z, rx, ry, rz, &face );

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