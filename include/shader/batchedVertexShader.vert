#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in float _diffuseTextureID;
layout (location = 4) in float _specularTextureID;
layout (location = 5) in float _materialID;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


uniform mat4 model;
uniform mat4 lightVP;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;

    int diffuseTextureID;
    int specularTextureID;
    int materialID;


    vec4 materialDiffuse;
    vec4 materialAmbient;
    vec4 materialSpecular;
    float materialShininess;

} vs_out;


struct Material
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    float shininess;
};


uniform Material materials[32];

void get_material_color(int id);

void main()
{

    
    
    gl_Position = projection * view * model * vec4(position,1.0f);

    vs_out.FragPos = vec3(model * vec4(position, 1.0f));// get fragment position in world space
    vs_out.Normal = mat3(transpose(inverse(model))) * normal;
    vs_out.TexCoords = vec2(texCoord.x, 1.0 - texCoord.y);

    vs_out.FragPosLightSpace =  lightVP * model * vec4(position,1.0f);

    vs_out.diffuseTextureID = int(_diffuseTextureID);
    vs_out.specularTextureID = int(_specularTextureID);
    vs_out.materialID = int(_materialID);

    // set the rigth material
    get_material_color(vs_out.materialID);

}




void get_material_color(int id)
{
    Material m;
    

    if(id == 0)
        m = materials[0];
    else if(id == 1)
        m = materials[1];
    else if(id == 2)
        m = materials[2];
    else if(id == 3)
        m = materials[3];
    else if(id == 4)
        m = materials[4];
    else if(id == 5)
        m = materials[5];
    else if(id == 6)
        m = materials[6];
    else if(id == 7)
        m = materials[7];
    else if(id == 8)
        m = materials[8];
    else if(id == 9)
        m = materials[9];
    else if(id == 10)
        m = materials[10];
    else if(id == 11)
        m = materials[11];
    else if(id == 12)
        m = materials[12];
    else if(id == 13)
        m = materials[13];
    else if(id == 14)
        m = materials[14];
    else if(id == 15)
        m = materials[15];
    else if(id == 16)
        m = materials[16];
    else if(id == 17)
        m = materials[17];
    else if(id == 18)
        m = materials[18];
    else if(id == 19)
        m = materials[19];
    else if(id == 20)
        m = materials[20];
    else if(id == 21)
        m = materials[21];
    else if(id == 22)
        m = materials[22];
    else if(id == 23)
        m = materials[23];
    else if(id == 24)
        m = materials[24];
    else if(id == 25)
        m = materials[25];

   vs_out.materialDiffuse = m.diffuse;
   vs_out.materialAmbient = m.ambient;
   vs_out.materialSpecular = m.specular;
   vs_out.materialShininess = m.shininess;
}