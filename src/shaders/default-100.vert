#version 100

attribute vec4 aPos;
attribute vec3 aColor;
attribute vec2 aTex;

varying vec3 color;
varying vec2 texCoord;

uniform float scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
   gl_Position = proj * view * model * vec4(aPos.rgb, 1);

   color = aColor;
   texCoord = aTex;
}
