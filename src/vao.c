#include "vao.h"

vao_t* vao_ctor() {
  vao_t* self = malloc(sizeof(vao_t));
  glGenVertexArrays(1, &(self->ID));
  return self;
}

void vao_link_attrib(vao_t* self, vbo_t* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
  vbo_bind(vbo);

  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  vbo_unbind(vbo);
}

void vao_bind  (vao_t* self) {
  glBindVertexArray(self->ID);
}

void vao_unbind(vao_t* self) {
  glBindVertexArray(0);
}

void vao_delete(vao_t* self) {
  // glDeleteVertexArrays(1, self->ID);
  free(self);
}