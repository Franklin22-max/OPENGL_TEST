#version 330 core


struct Material
{
    int numTextures;

    // used incase material has color material instead of images
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};




struct SpotLight
{
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
};


struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};



in vec2 TexCoords;
out vec4 color;


uniform DirectionalLight d_light;
uniform PointLight p_light;
uniform SpotLight s_light;


uniform Material material;


uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;




vec4 useDirectionalLight(DirectionalLight d_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);


void main()
{
    // point light
    vec4 color1 = usePointLight(
        p_light,
        texture(material.texture_diffuse1,TexCoords),// ambient
        texture(material.texture_diffuse1,TexCoords),// diffuse
        texture(material.texture_specular1,TexCoords),// specular
        material.shininess
    );

 

     // spot light
    vec4 color2 = useSpotLight(
        s_light,
        texture(material.texture_diffuse1,TexCoords),// ambient
        texture(material.texture_diffuse1,TexCoords),// diffuse
        texture(material.texture_specular1,TexCoords),// specular
        material.shininess
    );

    // point light
    vec4 color3 = useDirectionalLight(
        d_light,
        texture(material.texture_diffuse1,TexCoords),// ambient
        texture(material.texture_diffuse1,TexCoords),// diffuse
        texture(material.texture_specular1,TexCoords),// specular
        material.shininess
    );




    // point light
    vec4 color4 = usePointLight(
        p_light,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );

 

     // spot light
    vec4 color5 = useSpotLight(
        s_light,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );

    // point light
    vec4 color6 = useDirectionalLight(
        d_light,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );


        color = color1 + color2 + color3;

        color += (color4.r + color4.g + color4.b > 0)? color4 : vec4(0);
        color += (color5.r + color5.g + color5.b > 0)? color5 : vec4(0);
        color += (color6.r + color6.g + color6.b > 0)? color6 : vec4(0);
        
}






//                                          DIRECTIONAL LIGHT
vec4 useDirectionalLight(DirectionalLight d_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = vec4(d_light.ambient,1.0f) * material_ambient;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-d_light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(d_light.diffuse * diff,1.0f) * material_diffuse;

        

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(d_light.specular * spec, 1.0f) * material_specular;

    vec4 result = ambient + diffuse + specular;
    return result;
}




//                                          POINT LIGHT
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = vec4(p_light.ambient,1.0f) * material_ambient;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(p_light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(p_light.diffuse * diff,1.0f) * material_diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(p_light.specular * spec, 1.0f) * material_specular;

    // point lighting
    float distance = length(p_light.position -  FragPos);
    float attenuation = 1.0f / (p_light.constant + p_light.linear * distance + p_light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec4 result = ambient + diffuse + specular;
    return result;
}







//                                              SPOT LIGHT
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess)
{
   vec3 lightDir = normalize(s_light.position - FragPos);

    // ambient
    vec4 ambient = vec4(s_light.ambient,1.0f) *  material_ambient;
   
    // Diffuse
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(s_light.diffuse * diff,1.0f) * material_diffuse;
    


    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(s_light.specular * spec, 1.0f) * material_specular;

   float epsilon = s_light.cutOff - s_light.outerCutOff;
   float theta = dot(lightDir, normalize(-s_light.direction));
   float intensity = clamp((theta - s_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // We’ll leave ambient unaffected so we always have a little light.
    diffuse *= intensity;
    specular *= intensity;


    if(theta > s_light.outerCutOff)
    {
        vec4 result = ambient + diffuse + specular;
        return  result;
    }

    return vec4(0);
}


