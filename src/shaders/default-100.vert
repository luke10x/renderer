#version 100

attribute vec4 aPos;
attribute vec3 aColor;

varying vec3 color;

void main() {
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1);
   color = aColor;
}
