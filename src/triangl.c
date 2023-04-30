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

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef float mat4_t[16];

void mat4_rotate(mat4_t m, float angle, vec3_t axis) {
    float c = cos(angle);
    float s = sin(angle);
    float x = axis.x, y = axis.y, z = axis.z;
    float len = sqrt(x * x + y * y + z * z);
    if (len == 0.0f) return;
    x /= len;
    y /= len;
    z /= len;

    float t = 1.0f - c;
    float tx = t * x, ty = t * y, tz = t * z;
    float sxy = tx * y, sxz = tx * z, syz = ty * z;
    float sx = s * x, sy = s * y, sz = s * z;

    mat4_t r = {
        c + tx * x, s * z + sxy, -s * y + sxz, 0.0f,
        -s * z + sxy, c + ty * y, s * x + syz, 0.0f,
        s * y + sxz, -s * x + syz, c + tz * z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    mat4_t tmp;
    memcpy(tmp, m, sizeof(mat4_t));
    for (int i = 0; i < 4; i++) {
        m[i] = tmp[i] * r[0] + tmp[i + 4] * r[1] + tmp[i + 8] * r[2] + tmp[i + 12] * r[3];
        m[i + 4] = tmp[i] * r[4] + tmp[i + 4] * r[5] + tmp[i + 8] * r[6] + tmp[i + 12] * r[7];
        m[i + 8] = tmp[i] * r[8] + tmp[i + 4] * r[9] + tmp[i + 8] * r[10] + tmp[i + 12] * r[11];
        m[i + 12] = tmp[i] * r[12] + tmp[i + 4] * r[13] + tmp[i + 8] * r[14] + tmp[i + 12] * r[15];
    }
}
// Helper function definition
vec3_t vec3(float x, float y, float z) {
    vec3_t result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

void mat4_translate(float *mat, vec3_t vec) {
    mat[12] += vec.x;
    mat[13] += vec.y;
    mat[14] += vec.z;
}

float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

void perspective(float *mat4, float fovy_radians, float aspect, float zNear, float zFar) {
    float tanHalfFovy = tanf(fovy_radians / 2.0f);
    float top = zNear * tanHalfFovy;
    float bottom = -top;
    float right = aspect * top;
    float left = -right;
    
    mat4[0] = 2.0f * zNear / (right - left);
    mat4[1] = 0.0f;
    mat4[2] = 0.0f;
    mat4[3] = 0.0f;

    mat4[4] = 0.0f;
    mat4[5] = 2.0f * zNear / (top - bottom);
    mat4[6] = 0.0f;
    mat4[7] = 0.0f;

    mat4[8] = (right + left) / (right - left);
    mat4[9] = (top + bottom) / (top - bottom);
    mat4[10] = -(zFar + zNear) / (zFar - zNear);
    mat4[11] = -1.0f;

    mat4[12] = 0.0f;
    mat4[13] = 0.0f;
    mat4[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
    mat4[15] = 0.0f;
}
shader_t* shaderProgram;

typedef struct {
  GLsizei count;
  float rotation;
  double prev_time;
  double crnt_time;
  float aspect;
  GLFWwindow* window ;
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
  { //     COORDINATES     /        COLORS      /   TexCoord  //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
  };

  // Indices for vertices order
  GLuint indices[] =
  {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
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
    // "04-12-wall",
    // "02-04-wall",
    "05-01-wall",
     GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE
  );
  texture_unit(pop_cat, shaderProgram, "tex0", 0);

  // LOOP can start here //
  printf("Main loop\n");



  glUniform1f(uniID, 0.5f);
  
  texture_bind(pop_cat);

  // Bind the VAO so OpenGL knows to use it
  vao_bind(VAO1);

	// Variables that help the rotation of the pyramid
	ctx.rotation = 0.0f;
	ctx.prev_time = glfwGetTime();
  ctx.count = sizeof(indices) / sizeof(int);
  ctx.aspect = (float)w / h;
  ctx.window = window;

  	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

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
    // Tell OpenGL which Shader Program we want to use
    shader_activate(shaderProgram);

		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - ctx.prev_time >= 1 / 60)
		{
			ctx.rotation += 0.5f;
			ctx.prev_time = crntTime;
		}
    float model[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};

    float view[16]  = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};

    float proj[16]  = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};     

    mat4_rotate(model, radians(ctx.rotation), vec3(0.0f, 1.0f, 0.0f));
    mat4_translate(view, vec3(0.0f, -0.5f, -2.0f));
    perspective(proj, radians(45.0f), ctx.aspect, 0.1f, 100.0f);

    int model_loc = glGetUniformLocation(shaderProgram->ID, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model);

    int view_loc = glGetUniformLocation(shaderProgram->ID, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat*)(&view));

    int proj_loc = glGetUniformLocation(shaderProgram->ID, "proj");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat*)(&proj));
    // Draw primitives, number of indices, datatype of indices, index of indices
    glDrawElements(GL_TRIANGLES, ctx.count, GL_UNSIGNED_INT, 0);
    
    // Swap the back buffer with the front buffer
    glfwSwapBuffers(ctx.window);
    // Take care of all GLFW events
    glfwPollEvents();
}