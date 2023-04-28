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
} vbo_t;

vbo_t* vbo_ctor(GLfloat* vertices, GLsizeiptr size);
void vbo_bind  (vbo_t* self);
void vbo_unbind(vbo_t* self);
void vbo_delete(vbo_t* self);