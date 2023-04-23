
# Raycaster for Web and Native Apps.

This platform is built using C because of its simplicity, efficiency, and speed.
It leverages GLFW, a lightweight and portable library that provides an easy-to-use API
for creating windows, managing OpenGL contexts, and handling events.

The project can be executed in a web environment thanks to its Emscripten build,
which enables the C codebase to be compiled into WebAssembly and render graphics onto an HTML canvas element.
Emscripten can translate OpenGL code into WebGL,
which allows the same OpenGL codebase to run on multiple platforms, including the web.

## Internally Used File Formats

The project uses the Wavefront OBJ format for 3D data.
OBJ files are ideal for this project for two reasons:

- First, OBJ files can be easily created using popular software like Blender,
  which makes it effortless to import custom 3D models and environments into the project.
- Second, OBJ file format is straightforward to work with from C code,
  as it is a text-based format that can be parsed
  without requiring any additional libraries or dependencies.

PPM format is used for textures for similar reasons.
It is straightforward to process it from C code, 
and it is easy to create and modify textures using various image editing software.

## Build

To compile the WebAssembly build of this project,
you only need Docker installed on your system,
as the entire build toolchain is fully containerized.

Simply run the following command in your terminal:

    docker-compose run wasm 'make clean build'

Web application files will be genrated in `./wasm/build/`.