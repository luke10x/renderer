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
  GLenum type;
} texture_t;

#include "shader.h"

texture_t* texture_ctor(const char* image, GLenum tex_type, GLenum slot, GLenum format, GLenum pixel_type);
void texture_unit(texture_t* self, shader_t* shader, const char* uniform, GLuint unit);
void texture_bind(texture_t* self);
void texture_unbind(texture_t* self);
void texture_delete(texture_t* self);
