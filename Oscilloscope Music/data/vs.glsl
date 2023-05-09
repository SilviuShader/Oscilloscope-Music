#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

uniform float aspectRatio;

out vec3 fragColor;

void main()
{
	gl_Position = vec4(vertexPosition.x / aspectRatio, vertexPosition.y, 0.0, 1.0);
	fragColor = vec3(0.0, 1.0, 1.0);
}