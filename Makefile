demo.exe:
	gcc demo.c  -framework OpenGL -framework GLUT -o demo.exe

calc.o:
	gcc calc.c -c -o calc.o

engine.exe: # calc.o
	gcc engine.c  -framework OpenGL -framework GLUT -o engine.exe

Grid2D.exe:
	gcc Grid2D.c -framework OpenGL -framework GLUT -o Grid2D.exe

clean:
	rm -f *.out
	rm -f *.exe

run: clean engine.exe
	./engine.exe

.PHONY=clean run
