#version 330 core


struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};


struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};


struct PointLight
{
    float constant;
    float linear;
    float quadratic;
};


struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};



in vec2 TexCoords;
out vec4 color;

uniform Light light;
uniform PointLight p_light;
uniform SpotLight s_light;


uniform Material material;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;




vec4 testDirectionalLight();
vec4 testPointLight();
vec4 testSpotLight();


void main()
{
    // point light
    vec4 color1 = testPointLight();
    
    // point light
    vec4 color2 = testSpotLight();

    color = color2 + color1;
}




vec4 testPointLight()
{

    // ambient
    vec4 ambient = vec4(light.ambient,1.0f) * texture(material.texture_diffuse1,TexCoords);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec4 diffuse = vec4(light.diffuse * diff,1.0f) * texture(material.texture_diffuse1,TexCoords);

        

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular * spec, 1.0f) * texture(material.texture_specular1,TexCoords);

    // point lighting
    float distance = length(light.position -  FragPos);
    float attenuation = 1.0f / (p_light.constant + p_light.linear * distance + p_light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec4 result = ambient + diffuse + specular;

    

    return result;
}








vec4 testSpotLight()
{
   vec3 lightDir = normalize(s_light.position - FragPos);

    // ambient
    vec4 ambient = vec4(light.ambient,1.0f) *  texture(material.texture_diffuse1,TexCoords);
   
    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse * diff,1.0f) * texture(material.texture_diffuse1,TexCoords);
    


    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular * spec, 1.0f) * texture(material.texture_specular1,TexCoords);

   float theta = dot(lightDir, normalize(-s_light.direction));
   float epsilon = s_light.cutOff - s_light.outerCutOff;
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
