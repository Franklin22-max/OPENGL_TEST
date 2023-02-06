#version 330 core

out vec4 color;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D image;

void main()
{
    color = texture(image,texCoord);
}
