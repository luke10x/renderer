demo.exe:
	gcc demo.c  -framework OpenGL -framework GLUT -o demo.exe

clean:
	rm -f *.out
	rm -f *.exe
.PHONY=clean
