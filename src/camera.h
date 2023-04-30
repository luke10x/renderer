#pragma once
#pragma once

#include "shader.h"

#include <cglm/cglm.h>
#include <math.h>

typedef struct {
  vec3 Position;
  vec3 Orientation;
  vec3 Up;

  int width;
  int height;

  float speed;
  float sensitivity;

} camera_t;

camera_t* camera_ctor(int width, int height, vec3 position) {
  camera_t* self = malloc(sizeof(camera_t));

  glm_vec3_copy(position, self->Position);

  glm_vec3_copy((vec3){ 0.0f, -1.2f, 1.0f}, self->Position); // should not be here 
  glm_vec3_copy((vec3){ 0.0f, 0.1f, -0.1f }, self->Orientation);
  glm_vec3_copy((vec3){ 0.0f, 0.0f, 1.0f }, self->Up);

  // glm_vec3_copy((vec3){ 0.0f, 0.0f, -1.0f }, self->Orientation);
  // glm_vec3_copy((vec3){ 0.0f, 0.0f, 1.0f }, self->Up);
  self->width = width;
  self->height = height;

  self->speed = 0.1f;
  self->sensitivity = 100.0f;

  return self;
}

void camera_matrix(
  camera_t* self,
  float fov_deg,
  float near_plane,
  float far_plane,
  shader_t* shader,
  const char* uniform
) {
  float aspect = (float) self->width / self->height;

  mat4 sum;
  glm_vec3_add(self->Position, self->Orientation, (float *)sum);

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  glm_lookat(self->Position, (float *)sum, self->Up, view);

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(fov_deg), aspect, near_plane, far_plane, projection);

  mat4 mvp = GLM_MAT4_IDENTITY_INIT;
  glm_mat4_mul(projection, view, mvp);

  GLuint location = glGetUniformLocation(shader->ID, uniform);
  glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)mvp);
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

/*
	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
*/
}