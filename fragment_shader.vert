#version 330 core


struct Material
{
    sampler2D diffuse;
    sampler2D specular;
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
    vec3 ambient = light.ambient *  vec3(texture(material.diffuse,TexCoords));
   
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff *  vec3(texture(material.diffuse,TexCoords));

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    // point lighting
    float distance = length(light.position -  FragPos);
    float attenuation = 1.0f / (p_light.constant + p_light.linear * distance + p_light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

    return  (vec4(result,1.0));
}








vec4 testSpotLight()
{
   vec3 lightDir = normalize(s_light.position - FragPos);

    // ambient
    vec3 ambient = light.ambient *  vec3(texture(material.diffuse,TexCoords));
   
    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff *  vec3(texture(material.diffuse,TexCoords));

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

   float theta = dot(lightDir, normalize(-s_light.direction));
   float epsilon = s_light.cutOff - s_light.outerCutOff;
   float intensity = clamp((theta - s_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // We’ll leave ambient unaffected so we always have a little light.
    diffuse *= intensity;
    specular *= intensity;


    if(theta > s_light.outerCutOff)
    {
        vec3 result = ambient + diffuse + specular;
        return  vec4(result,1.0);
    }

    return vec4(0);
}
