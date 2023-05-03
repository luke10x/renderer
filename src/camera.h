#pragma once
#pragma once

#include "shader.h"

#include <cglm/cglm.h>
#include <math.h>

typedef struct {
  vec3 Position;
  vec3 Orientation;
  vec3 Up;

  mat4 camera_matrix;

	// Prevents the camera from jumping around when first clicking left click
	int firstClick;

  int width;
  int height;

  float speed;
  float sensitivity;

} camera_t;

camera_t* camera_ctor(int width, int height, vec3 position) {
  camera_t* self = malloc(sizeof(camera_t));

  glm_vec3_copy(position, self->Position);
  //                     //  x     z    y
  // glm_vec3_copy((vec3){ 0.5f, 0.25f, 8.0f}, self->Position); // should not be here 
  // glm_vec3_copy((vec3){ -0.25f, 0.0f, -1.0f }, self->Orientation);
  // glm_vec3_copy((vec3){ 0.0f, 0.1f, 0.0f }, self->Up);

  glm_vec3_copy((vec3){ 0.0f, 0.0f, -1.0f },  self->Orientation);
  glm_vec3_copy((vec3){ 0.0f, 1.0f, 0.0f },   self->Up);
  glm_mat4_copy((mat4)GLM_MAT4_IDENTITY_INIT, self->camera_matrix);

  self->width = width;
  self->height = height;

  self->firstClick = 0;

  self->speed = 0.1f;
  self->sensitivity = 100.0f;

  return self;
}

void camera_update_matrix(
  camera_t* self,
  float fov_deg,
  float near_plane,
  float far_plane
) {
  float aspect = (float) self->width / self->height;

  mat4 sum;
  glm_vec3_add(self->Position, self->Orientation, (float *)sum);

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  glm_lookat(self->Position, (float *)sum, self->Up, view);

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(fov_deg), aspect, near_plane, far_plane, projection);

  glm_mat4_mul(projection, view, self->camera_matrix);
}

void camera_matrix(
  camera_t* self,
  shader_t* shader,
  const char* uniform
) {
  GLuint location = glGetUniformLocation(shader->ID, uniform);
  glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)(self->camera_matrix));
}
void camera_inputs(camera_t* self, GLFWwindow* window) {
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    // glm_vec3_scale(self->Position, self->speed);
    vec3 tempVec;
    glm_vec3_scale(self->Orientation, self->speed, tempVec); // multiply Orientation vector by speed and store in a temporary vector
    glm_vec3_add(self->Position, tempVec, self->Position); // add the result to the Position vector

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// Position += speed * -glm::normalize(glm::cross(Orientation, Up));
    vec3 temp1, temp2, result;

    // Compute the cross product between Orientation and Up vectors
    glm_vec3_cross(self->Orientation, self->Up, temp1);

    // Normalize the resulting vector
    glm_normalize_to(temp1, temp2);

    // Compute the vector to be added to the Position
    glm_vec3_scale(temp2, -self->speed, result);

    // Add the resulting vector to the Position
    glm_vec3_add(self->Position, result, self->Position);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 tempVec;
    glm_vec3_scale(self->Orientation, -self->speed, tempVec); // multiply Orientation vector by speed and store in a temporary vector
    glm_vec3_add(self->Position, tempVec, self->Position); // add the result to the Position vector

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		// Position += speed * glm::normalize(glm::cross(Orientation, Up));
    vec3 temp1, temp2, result;

    // Compute the cross product between Orientation and Up vectors
    glm_vec3_cross(self->Orientation, self->Up, temp1);

    // Normalize the resulting vector
    glm_normalize_to(temp1, temp2);

    // Compute the vector to be added to the Position
    glm_vec3_scale(temp2, self->speed, result);

    // Add the resulting vector to the Position
    glm_vec3_add(self->Position, result, self->Position);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		// Position += speed * Up;
    vec3 direction;
    glm_cross(self->Position, self->Up, direction);
    vec3 movement;
    glm_vec3_scale(direction, -self->speed, movement);
    glm_vec3_add(self->Position, movement, self->Position);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		// Position += speed * -Up;
    glm_vec3_scale(self->Up, -1.0f, self->Up);
    glm_vec3_scale(self->Up, self->speed, self->Up);
    glm_vec3_add(self->Position, self->Up, self->Position);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		self->speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		self->speed = 0.1f;
	}

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (self->firstClick == 1)
		{
			glfwSetCursorPos(window, (self->width / 2), (self->height / 2));
			self->firstClick = 0;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = self->sensitivity * (float)(mouseY - (self->height / 2)) / self->height;
		float rotY = self->sensitivity * (float)(mouseX - (self->width / 2)) /  self->width;

		// Calculates upcoming vertical change in the Orientation
    vec3 crossed, normalized, orientation_copy;

    glm_vec3_copy(self->Orientation, orientation_copy);
    glm_vec3_cross(self->Orientation, self->Up, crossed);
    glm_normalize_to(crossed, normalized);
    glm_vec3_rotate(orientation_copy, glm_rad(-rotX), normalized);

    float angle = glm_vec3_angle(normalized, self->Up);
		// Decides whether or not the next vertical Orientation is legal or not
		if (fabsf(angle - glm_rad(90.0f)) <= glm_rad(85.0f)) {
      printf("does it ever go here\n");
      glm_vec3_copy(orientation_copy, self->Orientation);
		}

		// Rotates the Orientation left and right
    glm_vec3_rotate(self->Orientation, glm_rad(-rotY), self->Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (self->width / 2), (self->height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		self->firstClick = 1;
	}
}