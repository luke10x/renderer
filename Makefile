engine.exe: # calc.o
	gcc engine.c  -framework OpenGL -framework GLUT -o engine.exe

clean:
	rm -f *.out
	rm -f *.exe

run: clean engine.exe
	./engine.exe

.PHONY=clean run
