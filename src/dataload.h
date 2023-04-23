#pragma once

typedef struct {
  uint8_t r,g,b;
} t_pixel;

typedef struct {
  char* name;
  t_pixel* pixels;
  int width, height;
} t_material;

typedef struct {
  float x, y, z;
} t_vertex;

typedef struct {
  float u, v;
} t_vt;

typedef struct t_object t_object;
typedef struct t_face t_face;

struct t_face {
  t_vertex v1, v2, v3;
  t_vt vt1, vt2, vt3;
  t_object* obj;
  t_material *m;
};

struct t_object {
  char* name;
};