#version 100

precision mediump float;

uniform vec4 lightColor;

void main() {
    gl_FragColor = lightColor;
}
