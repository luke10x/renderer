#version 100

attribute vec4 aPos;

uniform mat4 model;
uniform mat4 camMatrix;

void main() {
	gl_Position = camMatrix * model * vec4(aPos.rgb, 1);
}
