#include "vbo.h"
vbo_t* vbo_ctor(GLfloat* vertices, GLsizeiptr size) {
  vbo_t* self = malloc(sizeof(vbo_t));
  glGenBuffers(1, &(self->ID));
  glBindBuffer(GL_ARRAY_BUFFER, self->ID);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);  
  return self;
}

void vbo_bind  (vbo_t* self) {
  glBindBuffer(GL_ARRAY_BUFFER, self->ID);
}

void vbo_unbind(vbo_t* self) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vbo_delete(vbo_t* self) {
  // glDeleteBuffers(1, self->ID);
  free(self);
}