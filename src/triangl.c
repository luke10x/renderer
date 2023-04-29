#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include<GLFW/glfw3.h>
#endif

#ifdef __EMSCRIPTEN__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#endif

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "dataload.h"

#include "texture.h"

shader_t* shaderProgram;

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW\n");
    return -1;
  }

#ifdef __APPLE__
	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifdef __EMSCRIPTEN__
  // OpenGL ES 2.0
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif

  GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
  glfwMakeContextCurrent(window);
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);

#ifdef __APPLE__
  shaderProgram = shader_ctor("src/shaders/default-330-core.vert", "src/shaders/default-330-core.frag");
#endif
#ifdef __EMSCRIPTEN__
  shaderProgram = shader_ctor("src/shaders/default-100.vert", "src/shaders/default-100.frag");
#endif

  // Vertices coordinates
  GLfloat vertices[] =
  { //     COORDINATES   /        COLORS      /   TexCoord  //
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Lower left corner
    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Upper left corner
    0.5f,   0.5f, 0.0f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Upper right corner
    0.5f,  -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f  // Lower right corner
  };

  // Indices for vertices order
  GLuint indices[] =
  {
    0, 2, 1, // Upper triangle
    0, 3, 2 // Lower triangle
  };

	// Generates Vertex Array Object and binds it
	vao_t* VAO1 = vao_ctor();
  vao_bind(VAO1);

  // Generates Vertex Buffer Object and links it to vertices
	vbo_t* VBO1 = vbo_ctor(vertices, sizeof(vertices));
  // Generates Element Buffer Object and links it to indices
	ebo_t* EBO1 = ebo_ctor(indices, sizeof(indices));

  // Links VBO to VAO
  vao_link_attrib(VAO1, VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
  vao_link_attrib(VAO1, VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  vao_link_attrib(VAO1, VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  vao_unbind(VAO1);
  vbo_unbind(VBO1);
  ebo_unbind(EBO1); 

  GLuint uniID = glGetUniformLocation(shaderProgram->ID, "scale");

  // Texture
  texture_t* pop_cat = texture_ctor(
    "30-03-ground-with-grass-on-top",
     GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE
  );
  texture_unit(pop_cat, shaderProgram, "tex0", 0);

  // LOOP can start here //
  printf("Main loop\n");

  // Specify the color of the background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  // Clean the back buffer and assign the new color to it
  glClear(GL_COLOR_BUFFER_BIT);
  // Tell OpenGL which Shader Program we want to use
  shader_activate(shaderProgram);

  glUniform1f(uniID, 0.5f);
  
  texture_bind(pop_cat);

  // Bind the VAO so OpenGL knows to use it
  vao_bind(VAO1);

  // Draw primitives, number of indices, datatype of indices, index of indices
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  
  // Swap the back buffer with the front buffer
  glfwSwapBuffers(window);
  // Take care of all GLFW events
  glfwPollEvents();
#ifdef __APPLE__
	// Main while loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
  }
#endif
}