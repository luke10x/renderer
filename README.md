To compile it in Emscripten:

emcc main.c -o index.html -I /emsdk/upstream/emscripten/cache/sysroot/include/ -lglfw -lGL -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 -s GL_UNSAFE_OPTS=0

