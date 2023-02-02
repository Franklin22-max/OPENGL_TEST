#version 330 core


struct Material
{
    // used incase material has color material instead of images
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    int numTextures;
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


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

out vec4 color;

uniform DirectionalLight d_light1;
uniform PointLight p_light1;
uniform SpotLight s_light1;


uniform Material material;
uniform vec3 viewPos;



float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec4 useDirectionalLight(DirectionalLight d_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);


void main()
{
    // point light
    vec4 color1 = usePointLight(
        p_light1,
        texture(material.texture_diffuse1,fs_in.TexCoords),// ambient
        texture(material.texture_diffuse1,fs_in.TexCoords),// diffuse
        texture(material.texture_specular1,fs_in.TexCoords),// specular
        material.shininess
    );

 

     // spot light
    vec4 color2 = useSpotLight(
        s_light1,
        texture(material.texture_diffuse1,fs_in.TexCoords),// ambient
        texture(material.texture_diffuse1,fs_in.TexCoords),// diffuse
        texture(material.texture_specular1,fs_in.TexCoords),// specular
        material.shininess
    );

    // point light
    vec4 color3 = useDirectionalLight(
        d_light1,
        texture(material.texture_diffuse1,fs_in.TexCoords),// ambient
        texture(material.texture_diffuse1,fs_in.TexCoords),// diffuse
        texture(material.texture_specular1,fs_in.TexCoords),// specular
        material.shininess
    );



    
    // point light
    vec4 color4 = usePointLight(
        p_light1,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );

 

     // spot light
    vec4 color5 = useSpotLight(
        s_light1,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );

    // point light
    vec4 color6 = useDirectionalLight(
        d_light1,
        material.ambient,// ambient
        material.diffuse,// diffuse
        material.specular,// specular
        material.shininess
    );
    
    if(material.numTextures > 0)
      color = color1 + color2 + color3 + vec4(vec3(color4) + vec3(color5) + vec3(color6),0.0);
    else
        color = color4 + color5 + color6;
}




//                                        SHADOW
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light’s perspective (using [0,1] rangefragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}


//                                          DIRECTIONAL LIGHT
vec4 useDirectionalLight(DirectionalLight d_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = vec4(d_light.ambient,1.0f) * material_ambient;

    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-d_light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(d_light.diffuse * diff,1.0f) * material_diffuse;

        

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(d_light.specular * spec * vec3(material_specular), 0.0f);

   // Calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    return (ambient + (1.0 - shadow) * (diffuse + specular));

}




//                                          POINT LIGHT
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = vec4(p_light.ambient,1.0f) * material_ambient;

    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(p_light.position - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(p_light.diffuse * diff,1.0f) * material_diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(p_light.specular * spec * vec3(material_specular), 0.0f);

    // point lighting
    float distance = length(p_light.position -  fs_in.FragPos);
    float attenuation = 1.0f / (p_light.constant + p_light.linear * distance + p_light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

   
   // Calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}







//                                              SPOT LIGHT
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess)
{
   vec3 lightDir = normalize(s_light.position - fs_in.FragPos);

    // ambient
    vec4 ambient = vec4(s_light.ambient,1.0f) *  material_ambient;
   
    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(s_light.diffuse * diff,1.0f) * material_diffuse;
    


    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(s_light.specular * spec * vec3(material_specular), 0.0f);

   float epsilon = s_light.cutOff - s_light.outerCutOff;
   float theta = dot(lightDir, normalize(-s_light.direction));
   float intensity = clamp((theta - s_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // We’ll leave ambient unaffected so we always have a little light.
    diffuse *= intensity;
    specular *= intensity;


    if(theta > s_light.outerCutOff)
    {
        
       // Calculate shadow
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
        return (ambient + (1.0 - shadow) * (diffuse + specular));
    }

    return vec4(0);
}

