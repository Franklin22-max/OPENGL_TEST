#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 lightVP;
uniform mat4 model;

void main()
{
	gl_Position = lightVP * model * vec4(position.x, position.y, position.z, 1.0);
}