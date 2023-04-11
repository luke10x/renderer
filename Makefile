demo.exe:
	gcc demo.c  -framework OpenGL -framework GLUT -o demo.exe

Grid2D.exe:
	gcc Grid2D.c -framework OpenGL -framework GLUT -o Grid2D.exe

clean:
	rm -f *.out
	rm -f *.exe
.PHONY=clean
