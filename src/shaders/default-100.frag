#version 100

precision mediump float;

varying vec3 color;
varying vec2 texCoord;
varying vec3 Normal;
varying vec3 crntPos;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;

// for specular
uniform vec3 camPos;


float fmax(float x, float y) {
    return x > y ? x : y;
}

void main() {

	// ambient lighting
	float ambient = 0.2;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = fmax(dot(normal, lightDirection), 0.0);

	// specular lighting
	float specularLight = 0.5;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(fmax(dot(viewDirection, reflectionDirection), 0.0), 8.0);
	float specular = specAmount * specularLight;

	// outputs final color
	gl_FragColor = texture2D(tex0, texCoord) * lightColor * (diffuse + ambient + specular);
}
