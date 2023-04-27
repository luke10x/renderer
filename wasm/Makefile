#
# WASM build
#

build:
	mkdir -p ./build

	emcc ../src/main.c -o ./build/main.html -I /emsdk/upstream/emscripten/cache/sysroot/include/ \
		-lglfw -lGL \
		-s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 \
		-s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 -s GL_UNSAFE_OPTS=0 \
		--preload-file ../assets/export/cube.obj@assets/export/cube.obj	\
		--preload-file ../assets/export/level-map.obj@assets/export/level-map.obj	\
		--preload-file ../assets/ppm/02-02x2-stone.ppm@assets/ppm/02-02x2-stone.ppm \
		--preload-file ../assets/ppm/02-04-wall.ppm@assets/ppm/02-04-wall.ppm \
		--preload-file ../assets/ppm/04-12-wall.ppm@assets/ppm/04-12-wall.ppm \
		--preload-file ../assets/ppm/05-01-wall.ppm@assets/ppm/05-01-wall.ppm \
		--preload-file ../assets/ppm/06-01-metal.ppm@assets/ppm/06-01-metal.ppm \
		--preload-file ../assets/ppm/07-03-floor.ppm@assets/ppm/07-03-wall.ppm \
		--preload-file ../assets/ppm/08-01-wall.ppm@assets/ppm/08-01-wall.ppm \
		--preload-file ../assets/ppm/09-01-water.ppm@assets/ppm/09-01-water.ppm \
		--preload-file ../assets/ppm/10-11-lava.ppm@assets/ppm/10-11-lava.ppm \
		--preload-file ../assets/ppm/15-03-box-with-board.ppm@assets/ppm/15-03-box-with-board.ppm \
		--preload-file ../assets/ppm/17-03-grass.ppm@assets/ppm/17-03-grass.ppm \
		--preload-file ../assets/ppm/18-17-leaf.ppm@assets/ppm/18-17-leaf.ppm \
		--preload-file ../assets/ppm/21-01-rusty.ppm@assets/ppm/21-01-rusty.ppm \
		--preload-file ../assets/ppm/23-29-multicol.ppm@assets/ppm/23-29-multicol.ppm \
		--preload-file ../assets/ppm/30-03-ground-with-grass-on-top.ppm@assets/ppm/30-03-ground-with-grass-on-top.ppm \
		--preload-file ../assets/ppm/31-03-ground.ppm@assets/ppm/31-03-ground.ppm \
		--preload-file ../assets/ppm/steve.ppm@assets/ppm/steve.ppm \
		--preload-file ../assets/ppm/test.ppm@assets/ppm/test.ppm \
		--preload-file ../assets/ppm/text.ppm@assets/ppm/text.ppm

build/triangl.html:
	mkdir -p ./build

	emcc ../src/shader.c -c -o ./build/shader.o

	emcc ../src/triangl.c ./build/shader.o -o ./build/triangl.html \
		-s FULL_ES2=1 -s USE_GLFW=3 -O0 \
		--preload-file ../src/shaders/default-100.vert@src/shaders/default-100.vert \
		--preload-file ../src/shaders/default-100.frag@src/shaders/default-100.frag


clean:
	rm -fr build/

.PHONY=clean
