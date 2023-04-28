#include "shader.h"

char* _get_file_contents(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file %s", filename);
    return NULL;
  }

  fseek(fp, 0L, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);

  char *buffer = (char *) malloc(sizeof(char) * (file_size + 1));
  if (buffer == NULL) {
    perror("Error allocating memory");
    fclose(fp);
    return NULL;
  }

  size_t result = fread(buffer, 1, file_size, fp);
  if (result != file_size) {
    perror("Error reading file");
    fclose(fp);
    free(buffer);
    return NULL;
  }

  buffer[file_size] = '\0';
  fclose(fp);
  return buffer;
}


void _print_compile_errors(unsigned int shader, int isProgram)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (isProgram > 0)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			fprintf(stderr, "SHADER_COMPILATION_ERROR for: %d\n %s\n", isProgram, infoLog);
		}
	}
	else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      			fprintf(stderr, "SHADER_LINKING_ERROR for: %d\n %s\n", isProgram, infoLog);

    }
  }
}

GLuint _loadShader(GLenum type, const char *source)
{
  // create shader
  GLuint shader = glCreateShader(type);
  if (shader == 0)
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

GLuint _buildProgram(
  GLuint vertexShader,
  GLuint fragmentShader,
  const char* vertexPositionName
) {
  // create a GL program and link it
  GLuint po = glCreateProgram();
  glAttachShader(po, vertexShader);
  glAttachShader(po, fragmentShader);
  glBindAttribLocation(po, 0, vertexPositionName);
  glLinkProgram(po);

  // check if the program linked successfully
  GLint linked;
  glGetProgramiv(po, GL_LINK_STATUS, &linked);
  if(!linked) {
    fprintf(stderr, "Program link error\n");
    glDeleteProgram(po);
    return 0;
  }
  return po;
}

shader_t* shader_ctor(const char* vertex_file, const char* fragment_file) {
  char* vertex_source   = _get_file_contents(vertex_file);
  char* fragment_source = _get_file_contents(fragment_file);

  // load vertex and fragment shaders
  GLuint vertexShader   = _loadShader(GL_VERTEX_SHADER, vertex_source);
  GLuint fragmentShader = _loadShader(GL_FRAGMENT_SHADER, fragment_source);

  _print_compile_errors(vertexShader, 2);
  _print_compile_errors(fragmentShader, 1);

  shader_t* self = malloc(sizeof(shader_t));
  self->ID = _buildProgram(vertexShader, fragmentShader, "vPosition");
  _print_compile_errors(self->ID, 0);


  return self;
}

void shader_activate(const shader_t* self) {
  glUseProgram(self->ID);
}

void shader_delete(const shader_t* self) {
  
}
