#include "ebo.h"
ebo_t* ebo_ctor(GLuint* indices, GLsizeiptr size) {
  ebo_t* self = malloc(sizeof(ebo_t));
  glGenBuffers(1, &(self->ID));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);  
  return self;
}

void ebo_bind  (ebo_t* self) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ID);
}

void ebo_unbind(ebo_t* self) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ebo_delete(ebo_t* self) {
  // glDeleteBuffers(1, self->ID);
  free(self);
}