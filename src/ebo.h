#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include<GLFW/glfw3.h>
#endif

#ifdef __EMSCRIPTEN__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#endif

typedef struct {
  GLuint ID;
} ebo_t;

ebo_t* ebo_ctor(GLuint* indices, GLsizeiptr size);
void ebo_bind  (ebo_t* self);
void ebo_unbind(ebo_t* self);
void ebo_delete(ebo_t* self);