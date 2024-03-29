#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;


uniform mat4 lightVP;
uniform mat4 model;


void main()
{

    gl_Position = projection * view * model * vec4(position,1.0f);

    vs_out.FragPos = vec3(model * vec4(position, 1.0f));// get fragment position in world space
    vs_out.Normal = mat3(transpose(inverse(model))) * normal;
    vs_out.TexCoords = vec2(texCoord.x, 1.0 - texCoord.y);

    vs_out.FragPosLightSpace =  lightVP * model * vec4(position,1.0f);

}