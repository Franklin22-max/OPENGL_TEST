#version 330 core


in VS_OUT {
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
} vs_in;


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


uniform sampler2D samplers[32];


uniform vec3 viewPos;
uniform sampler2D shadowMap;

out vec4 color;

uniform DirectionalLight d_light1;
uniform PointLight p_light1;
uniform SpotLight s_light1;


vec4 get_texture(int id);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
//vec4 useDirectionalLight(DirectionalLight d_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
//vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess);



void main()
{
    vec4 t_diffuse = get_texture(vs_in.diffuseTextureID);
    vec4 t_specular = get_texture(vs_in.specularTextureID);

    vec4 color1 = useSpotLight(s_light1, vs_in.materialAmbient, vs_in.materialDiffuse, vs_in.materialSpecular, vs_in.materialShininess);
    vec4 color2 = useSpotLight(s_light1, t_diffuse, t_diffuse, t_specular, vs_in.materialShininess);
    vec4 color3 = useSpotLight(s_light1, vs_in.materialAmbient, vs_in.materialDiffuse, vs_in.materialSpecular, vs_in.materialShininess);
    vec4 color4 = useSpotLight(s_light1, t_diffuse, t_diffuse, t_specular, vs_in.materialShininess);
    vec4 color5 = useSpotLight(s_light1, vs_in.materialAmbient, vs_in.materialDiffuse, vs_in.materialSpecular, vs_in.materialShininess);
    vec4 color6 = useSpotLight(s_light1, t_diffuse, t_diffuse, t_specular, vs_in.materialShininess);

    if(vs_in.diffuseTextureID != -1 &&  vs_in.materialDiffuse.r + vs_in.materialDiffuse.g + vs_in.materialDiffuse.b  == 0)
        color = vec4(vec3(color1),0.f) + color2;
    else 
        color = color1 + color2;
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

    // Get depth of current fragment from light�s perspective
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
    vec3 normal = normalize(vs_in.Normal);
    vec3 lightDir = normalize(-d_light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(d_light.diffuse * diff,1.0f) * material_diffuse;

        

    // Specular
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(d_light.specular * spec * vec3(material_specular), 0.0f);

   // Calculate shadow
    float shadow = ShadowCalculation(vs_in.FragPosLightSpace, normal, lightDir);
    float diffuse_z = diffuse.z;
    return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));

}




//                                          POINT LIGHT
vec4 usePointLight(PointLight p_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess )
{

    // ambient
    vec4 ambient = 0.15 * vec4(p_light.ambient,1.0f) * material_ambient;

    // Diffuse
    vec3 normal = normalize(vs_in.Normal);
    vec3 lightDir = normalize(p_light.position - vs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(p_light.diffuse * diff,1.0f) * material_diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(p_light.specular * spec * vec3(material_specular), 0.0f);

    // point lighting
    float distance = length(p_light.position -  vs_in.FragPos);
    float attenuation = 1.0f / (p_light.constant + p_light.linear * distance + p_light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

   
   // Calculate shadow
    float shadow = ShadowCalculation(vs_in.FragPosLightSpace, normal, lightDir);
    float diffuse_z = diffuse.z;
    return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));
}







//                                              SPOT LIGHT
vec4 useSpotLight(SpotLight s_light, vec4 material_ambient, vec4 material_diffuse, vec4 material_specular, float material_shininess)
{
   vec3 lightDir = normalize(s_light.position - vs_in.FragPos);

    // ambient
    vec4 ambient = vec4(s_light.ambient,1.0f) *  material_ambient;
   
    // Diffuse
    vec3 normal = normalize(vs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = vec4(s_light.diffuse * diff,1.0f) * material_diffuse;
    


    // Specular
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec4 specular = vec4(s_light.specular * spec * vec3(material_specular), 0.0f);

   float epsilon = s_light.cutOff - s_light.outerCutOff;
   float theta = dot(lightDir, normalize(-s_light.direction));
   float intensity = clamp((theta - s_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // We�ll leave ambient unaffected so we always have a little light.
    diffuse *= intensity;
    specular *= intensity;


    float diffuse_z = diffuse.z;
    if(theta > s_light.outerCutOff)
    {  
       // Calculate shadow
        float shadow = ShadowCalculation(vs_in.FragPosLightSpace, normal, lightDir);
        return (ambient +   vec4((1.0 - shadow) * vec3(diffuse + specular), diffuse_z));
    }

    return ambient;
}



vec4 get_texture(int id)
{
    vec4 color = vec4(0.0);

    if(id == 0)
        color = texture(samplers[0], vs_in.TexCoords);
    else if(id == 1)
        color = texture(samplers[1], vs_in.TexCoords);
    else if(id == 2)
        color = texture(samplers[2], vs_in.TexCoords);
    else if(id == 3)
        color = texture(samplers[3], vs_in.TexCoords);
    else if(id == 4)
        color = texture(samplers[4], vs_in.TexCoords);
    else if(id == 5)
        color = texture(samplers[5], vs_in.TexCoords);
    else if(id == 6)
        color = texture(samplers[6], vs_in.TexCoords);
    else if(id == 7)
        color = texture(samplers[7], vs_in.TexCoords);
    else if(id == 8)
        color = texture(samplers[8], vs_in.TexCoords);
    else if(id == 9)
        color = texture(samplers[9], vs_in.TexCoords);
    else if(id == 10)
        color = texture(samplers[10], vs_in.TexCoords);
    else if(id == 11)
        color = texture(samplers[11], vs_in.TexCoords);
    else if(id == 12)
        color = texture(samplers[12], vs_in.TexCoords);
    else if(id == 13)
        color = texture(samplers[13], vs_in.TexCoords);
    else if(id == 14)
        color = texture(samplers[14], vs_in.TexCoords);
    else if(id == 15)
        color = texture(samplers[15], vs_in.TexCoords);
    else if(id == 16)
        color = texture(samplers[16], vs_in.TexCoords);
    else if(id == 17)
        color = texture(samplers[17], vs_in.TexCoords);
    else if(id == 18)
        color = texture(samplers[18], vs_in.TexCoords);
    else if(id == 19)
        color = texture(samplers[19], vs_in.TexCoords);
    else if(id == 20)
        color = texture(samplers[20], vs_in.TexCoords);
    else if(id == 21)
        color = texture(samplers[21], vs_in.TexCoords);
    else if(id == 22)
        color = texture(samplers[22], vs_in.TexCoords);
    else if(id == 23)
        color = texture(samplers[23], vs_in.TexCoords);
    else if(id == 24)
        color = texture(samplers[24], vs_in.TexCoords);
    else if(id == 25)
        color = texture(samplers[25], vs_in.TexCoords);

    return color;
}


