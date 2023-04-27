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


#ifdef __APPLE__
// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";
#endif
#ifdef __EMSCRIPTEN__
const char vertexShaderSource[] = 
"attribute vec4 vPosition; \n"
"void main() \n"
"{ \n"
"   gl_Position = vPosition; \n"
"} \n";

const char fragmentShaderSource[] = 
"precision mediump float; \n"
"void main() \n"
"{ \n"
"   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0), \n"
"                (0.0 , 1.0 , 0.0 , 1.0),\n"
"                (0.0 , 0.0 , 0.1 , 1.0); \n"
"} \n";
#endif

GLuint shaderProgram;

GLuint loadShader(GLenum type, const char *source)
{
    // create shader
    GLuint shader = glCreateShader(type);
    if(shader == 0)
    {
        fprintf(stderr, "Error creating shader\n");
        return 0;
    }

    // load the shader source to the shader object and compile it;
    glShaderSource(shader, 1, &source, NULL);
    //~ glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    // check if the shader compiled successfully
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        fprintf(stderr, "Shader compilation error\n");
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint buildProgram(GLuint vertexShader, GLuint fragmentShader, const char * vertexPositionName)
{
    // create a GL program and link it
    GLuint po = glCreateProgram();
    glAttachShader(po, vertexShader);
    glAttachShader(po, fragmentShader);
    glBindAttribLocation(po, 0, vertexPositionName);
    glLinkProgram(po);

    // check if the program linked successfully
    GLint linked;
    glGetProgramiv(po, GL_LINK_STATUS, &linked);
    if(!linked)
    {
        fprintf(stderr, "Program link error\n");
        glDeleteProgram(po);
        return 0;
    }
    return po;
}

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

  // load vertex and fragment shaders
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  shaderProgram = buildProgram(vertexShader, fragmentShader, "vPosition");

	// Vertices coordinates
	GLfloat vertices[] =
	{
		-0.5f,    -0.5f * (float)(sqrt(3)) / 3,     0.0f, // Lower left corner
		0.5f,     -0.5f * (float)(sqrt(3)) / 3,     0.0f, // Lower right corner
		0.0f,      0.5f * (float)(sqrt(3)) * 2 / 3, 0.0f, // Upper corner
		-0.5f / 2, 0.5f * (float)(sqrt(3)) / 6,     0.0f, // Inner left
		0.5f / 2,  0.5f * (float)(sqrt(3)) / 6,     0.0f, // Inner right
		0.0f,     -0.5f * (float)(sqrt(3)) / 3,     0.0f // Inner down
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0, 3, 5, // Lower left triangle
		3, 2, 4, // Lower right triangle
		5, 4, 1 // Upper triangle
	};

  // Create reference containers for the Vartex Array Object 
  // and the Vertex Buffer Object
	GLuint VAO, VBO, EBO;

	// Generate the VAO, VBO, and EBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Introduce the vertices into the VBO
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(float)*sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
  );

	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// Bind the EBO to 0 so that we don't accidentally modify it
	// MAKE SURE TO UNBIND IT AFTER UNBINDING THE VAO, as the EBO is linked in the VAO
	// This does not apply to the VBO because the VBO is already linked to the VAO during glVertexAttribPointer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// Bind the EBO to 0 so that we don't accidentally modify it
	// MAKE SURE TO UNBIND IT AFTER UNBINDING THE VAO, as the EBO is linked in the VAO
	// This does not apply to the VBO because the VBO is already linked to the VAO during glVertexAttribPointer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // LOOP can start here //

  // Specify the color of the background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  // Clean the back buffer and assign the new color to it
  glClear(GL_COLOR_BUFFER_BIT);
  // Tell OpenGL which Shader Program we want to use
  glUseProgram(shaderProgram);
  // Bind the VAO so OpenGL knows to use it
  glBindVertexArray(VAO);
  // Draw primitives, number of indices, datatype of indices, index of indices
  glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
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