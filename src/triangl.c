#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

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
// #include "matrix.h"
#include "camera.h"

shader_t* shaderProgram;
shader_t* light_shader;

typedef struct {
  GLsizei count;
  GLsizei light_count;
  float rotation;
  double prev_time;
  double crnt_time;
  float aspect;
  GLFWwindow* window;
  camera_t* camera;
  vao_t*    pyramid_vao;
  vao_t*    light_vao;
}  main_loop_ctx_t;
main_loop_ctx_t ctx;

void main_loop();

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
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};

  GLfloat lightVertices[] =
  { //     COORDINATES     //
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
    0.1f,  0.1f, -0.1f,
    0.1f,  0.1f,  0.1f
  };

  GLuint lightIndices[] =
  {
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
  };

	// Generates Vertex Array Object and binds it
	vao_t* VAO1 = vao_ctor();
  vao_bind(VAO1);

  // Generates Vertex Buffer Object and links it to vertices
	vbo_t* VBO1 = vbo_ctor(vertices, sizeof(vertices));
  // Generates Element Buffer Object and links it to indices
	ebo_t* EBO1 = ebo_ctor(indices, sizeof(indices));

  // Links VBO to VAO
  vao_link_attrib(VAO1, VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*) 0);
  vao_link_attrib(VAO1, VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
  vao_link_attrib(VAO1, VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
  vao_link_attrib(VAO1, VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));


  vao_unbind(VAO1);
  vbo_unbind(VBO1);
  ebo_unbind(EBO1); 


	// Shader for light cube
#ifdef __APPLE__
  light_shader = shader_ctor("src/shaders/light-330-core.vert", "src/shaders/light-330-core.frag");
#endif
#ifdef __EMSCRIPTEN__
  light_shader = shader_ctor("src/shaders/light-100.vert", "src/shaders/light-100.frag");
#endif

	// Generates Vertex Array Object and binds it
	vao_t* lightVAO = vao_ctor();
	vao_bind(lightVAO);

	// Generates Vertex Buffer Object and links it to vertices
	vbo_t* lightVBO = vbo_ctor(lightVertices, sizeof(lightVertices));
	// Generates Element Buffer Object and links it to indices
	ebo_t* lightEBO = ebo_ctor(lightIndices, sizeof(lightIndices));
	// Links VBO attributes such as coordinates and colors to VAO
	vao_link_attrib(lightVAO, lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	// Unbind all to prevent accidentally modifying them
	vao_unbind(lightVAO);
	vbo_unbind(lightVBO);
	ebo_unbind(lightEBO);

	vec4 lightColor;
  glm_vec4_copy((vec4){ 1.0f, 1.0f, 1.0f, 1.0f }, lightColor);
  vec3 lightPos;
	glm_vec3_copy((vec3){ 0.5f, 0.5f, 0.5f }, lightPos);
	mat4 lightModel = GLM_MAT4_IDENTITY_INIT;
  glm_translate(lightModel, lightPos);
	vec3 pyramidPos;
  glm_vec3_copy((vec3){ 0.0f, 0.0f, 0.0f }, pyramidPos);
	mat4 pyramidModel = GLM_MAT4_IDENTITY_INIT;
	glm_translate(pyramidModel, pyramidPos);

  // Texture
  texture_t* pop_cat = texture_ctor(
    // "04-12-wall",
    // "02-04-wall",
    "05-01-wall",
     GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE
  );

  
  texture_bind(pop_cat);
	shader_activate(light_shader);
	glUniformMatrix4fv(glGetUniformLocation(light_shader->ID, "model"), 1, GL_FALSE, (GLfloat*)lightModel);
	glUniform4f(glGetUniformLocation(light_shader->ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
  texture_unit(pop_cat, shaderProgram, "tex0", 0);
	
  shader_activate(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "model"), 1, GL_FALSE, (GLfloat*)pyramidModel);
	glUniform4f(glGetUniformLocation(shaderProgram->ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
	glUniform3f(glGetUniformLocation(shaderProgram->ID, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);


  // Bind the VAO so OpenGL knows to use it
  vao_bind(VAO1);

  	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


  camera_t* camera = camera_ctor(w, h, (vec3){0.0f, 0.0f, 2.0f});

	// Variables that help the rotation of the pyramid
	ctx.rotation = 0.0f;
	ctx.prev_time = glfwGetTime();
  ctx.count = sizeof(indices) / sizeof(int);
  ctx.light_count = sizeof(lightIndices) / sizeof(int);
  ctx.aspect = (float)w / h;
  ctx.window = window;
  ctx.camera = camera;
  ctx.pyramid_vao = VAO1;
  ctx.light_vao = lightVAO;

#ifdef __EMSCRIPTEN__
  int fps = 0; // Use browser's requestAnimationFrame
  emscripten_set_main_loop(main_loop, fps, 1);
#else
	// Main while loop
	while (!glfwWindowShouldClose(window)) { main_loop(); }
#endif
}

void main_loop() {
    // Specify the color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		camera_inputs(ctx.camera, ctx.window);
    camera_update_matrix(ctx.camera, 45.0f, 0.1f, 100.0f);

    // Tell OpenGL which Shader Program we want to use
    shader_activate(shaderProgram);


		// Exports the camera Position to the Fragment Shader for specular lighting
		glUniform3f(
      glGetUniformLocation(shaderProgram->ID, "camPos"),
      ctx.camera->Position[0],
      ctx.camera->Position[1],
      ctx.camera->Position[2]
      );

    camera_matrix(ctx.camera, shaderProgram, "camMatrix");
    // Draw primitives, number of indices, datatype of indices, index of indices
    vao_bind(ctx.pyramid_vao);
    glDrawElements(GL_TRIANGLES, ctx.count, GL_UNSIGNED_INT, 0);

		// Tells OpenGL which Shader Program we want to use
    shader_activate(light_shader);
		// Export the camMatrix to the Vertex Shader of the light cube
		camera_matrix(ctx.camera, light_shader, "camMatrix");
		// Bind the VAO so OpenGL knows to use it
    vao_bind(ctx.light_vao);
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, ctx.light_count, GL_UNSIGNED_INT, 0);
    
    // Swap the back buffer with the front buffer
    glfwSwapBuffers(ctx.window);
    // Take care of all GLFW events
    glfwPollEvents();
}