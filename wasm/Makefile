build:
	mkdir -p ./build

	# emcc ../src/hello_world_gles.c -s WASM=0 -o ./build/hello_world_gles.js
	
	emcc ../src/main.c -o ./build/main.html -I /emsdk/upstream/emscripten/cache/sysroot/include/ \
		-lglfw -lGL \
		-s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 \
		-s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 -s GL_UNSAFE_OPTS=0 \
		--preload-file ../assets/export/cube.obj@assets/export/cube.obj	\
		--preload-file ../assets/ppm/09-01-water.ppm@assets/ppm/09-01-water.ppm \
		--preload-file ../assets/ppm/15-03-box-with-board.ppm@assets/ppm/15-03-box-with-board.ppm \
		--preload-file ../assets/ppm/17-03-grass.ppm@assets/ppm/17-03-grass.ppm \
		--preload-file ../assets/ppm/18-17-leaf.ppm@assets/ppm/18-17-leaf.ppm \
		--preload-file ../assets/ppm/23-29-multicol.ppm@assets/ppm/23-29-multicol.ppm \
		--preload-file ../assets/ppm/30-03-ground-with-grass-on-top.ppm@assets/ppm/30-03-ground-with-grass-on-top.ppm \
		--preload-file ../assets/ppm/31-03-ground.ppm@assets/ppm/31-03-ground.ppm \
		--preload-file ../assets/ppm/steve.ppm@assets/ppm/steve.ppm \
		--preload-file ../assets/ppm/test.ppm@assets/ppm/test.ppm \
		--preload-file ../assets/ppm/text.ppm@assets/ppm/text.ppm

	# emcc ../src/engine.c \
	# 	-I../src -L../src \
	# 	-lglut -lGL  \
	# 	-s WASM=0 -o ./build/engine.html
	

clean:
	rm -fr build/



.PHONY=clean
