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
} t_shader;

t_shader* shader_ctor(const char* vertex_file, const char* fragment_file);

void shader_activate(const t_shader* self);

void shader_deactivate(const t_shader* self);

