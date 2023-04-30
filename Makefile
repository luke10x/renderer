#
# Desktop application build
#

dataload.o:
	clang src/dataload.c -c -o tmp/dataload.o
	
main.app:
	clang \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-L/opt/homebrew/lib \
	-lglfw -framework CoreVideo -framework OpenGL -framework GLUT \
	-framework IOKit -framework Cocoa -framework Carbon \
	src/main.c -o main.app


texture.o:
	clang src/texture.c -c -o tmp/texture.o \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include


shader.o:
	clang src/shader.c -c -o tmp/shader.o \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include

vao.o:
	clang src/vao.c -c -o tmp/vao.o \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include 

ebo.o:
	clang src/ebo.c -c -o tmp/ebo.o \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include 
	
vbo.o:
	clang src/vbo.c -c -o tmp/vbo.o \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include 



triangl.app: texture.o shader.o ebo.o vbo.o vao.o dataload.o
	clang \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/cglm/0.8.9/include \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include \
	-L/opt/homebrew/lib \
	-lglfw -lglew -framework CoreVideo -framework OpenGL -framework GLUT  \
	-framework IOKit -framework Cocoa -framework Carbon \
	src/triangl.c tmp/texture.o tmp/dataload.o tmp/shader.o tmp/vao.o tmp/ebo.o tmp/vbo.o -o triangl.app

clean:
	rm -f tmp/*.o
	rm -f *.out
	rm -f *.exe
	rm -f *.app

run: clean main.app
	./main.app

.PHONY=clean run
