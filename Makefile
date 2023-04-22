engine.exe: # calc.o
	gcc engine.c  -framework OpenGL -framework GLUT -o engine.exe

main.exe:
	clang \
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
