#version 100

attribute vec4 aPos;
attribute vec3 aColor;
attribute vec2 aTex;

varying vec3 color;
varying vec2 texCoord;

uniform float scale;

void main() {
   gl_Position = vec4(
      aPos.x + aPos.x * scale,
      aPos.y + aPos.y * scale,
      aPos.z + aPos.z * scale,
      1);
   color = aColor;
   texCoord = aTex;
}