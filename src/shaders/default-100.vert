#version 100

precision mediump float;

attribute vec4 aPos;
attribute vec3 aColor;
attribute vec2 aTex;
attribute vec3 aNormal;

varying vec3 color;
varying vec2 texCoord;
varying vec3 Normal;
varying vec3 crntPos;

uniform mat4 camMatrix;
uniform mat4 model;

void main() {
   crntPos = vec3(model * vec4(aPos.rgb, 1));
   gl_Position = camMatrix * vec4(crntPos.rgb, 1);

   color = aColor;
   texCoord = aTex;
   Normal = aNormal;
}
