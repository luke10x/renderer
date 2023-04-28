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

#include "vbo.h"

typedef struct {
  GLuint ID;
} vao_t;

vao_t* vao_ctor();
void vao_link_attrib(vao_t* self, vbo_t* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
void vao_bind  (vao_t* self);
void vao_unbind(vao_t* self);
void vao_delete(vao_t* self);
 