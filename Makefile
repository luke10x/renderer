engine.exe: # calc.o
	gcc src/engine.c -Lsrc -Isrc -framework OpenGL -framework GLUT -o engine.exe

main.exe:
	clang \
	-DGL_SILENCE_DEPRECATION \
	-I/opt/homebrew/Cellar/glfw/3.3.8/include \
	-L/opt/homebrew/lib \
	-lglfw -framework CoreVideo -framework OpenGL -framework GLUT \
	-framework IOKit -framework Cocoa -framework Carbon \
	src/main.c -o main.exe

clean:
	rm -f *.out
	rm -f *.exe

run: clean engine.exe
	./engine.exe

.PHONY=clean run
