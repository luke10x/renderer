#
# Desktop application build
#

main.app:
	clang \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-L/opt/homebrew/lib \
	-lglfw -framework CoreVideo -framework OpenGL -framework GLUT \
	-framework IOKit -framework Cocoa -framework Carbon \
	src/main.c -o main.app

triangl.app:
	clang \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-I/opt/homebrew/Cellar/glew/2.2.0_1/include \
	-L/opt/homebrew/lib \
	-lglfw -lglew -framework CoreVideo -framework OpenGL -framework GLUT  \
	-framework IOKit -framework Cocoa -framework Carbon \
	src/triangl.c -o triangl.app

clean:
	rm -f *.out
	rm -f *.exe
	rm -f *.app

run: clean main.app
	./main.app

.PHONY=clean run
