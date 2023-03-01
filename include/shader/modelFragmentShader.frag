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

   

 

     // spot light
    vec4 color2 = useSpotLight(
        s_light1,
        texture(material.texture_diffuse1,fs_in.TexCoords),// ambient
        texture(material.texture_diffuse1,fs_in.TexCoords),// diffuse
        texture(material.texture_specular1,fs_in.TexCoords),// specular
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

    
    if(material.numTextures > 0)
      color = color2;
    else
        color = color2 + color5;

}







//                                        SHADOW
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    // Get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;

    
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    // Check whether current frag pos is in shadow
    


    // smothing edges of shadow
    vec2 pixelSize = 1.0 / textureSize(shadowMap,0);
    int sampleRadius = 2;
    float shadow = 0;

    for(int y = -sampleRadius; y < sampleRadius; y++ )
    {
        for( int x = -sampleRadius; x < sampleRadius; x++)
        {
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * pixelSize).r;
            if(currentDepth > closestDepth + bias)
                shadow += 1.0f;
        }
    }
    shadow /= pow((sampleRadius * 2 + 1), 2);
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
    float diffuse_z = diffuse.z;
    return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));

}




//                                          POINT LIGHT
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = 0.15 * vec4(p_light.ambient,1.0f) * material_ambient;

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
    float diffuse_z = diffuse.z;
    return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));
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


    float diffuse_z = diffuse.z;
    if(theta > s_light.outerCutOff)
    {  
       // Calculate shadow
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
        return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));
    }

    return ambient;
}

