#version 420 core

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D texture_specular0;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_normal0;
uniform sampler2D texture_height0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform sampler2D texture_shadow;

uniform vec3 viewPos;
uniform Light light;
uniform float alpha;

uniform bool shadows;

float calcShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;

    float closestDepth = texture(texture_shadow, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.028f * (1.0f - dot(norm, lightDir)), 0.0028f);

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(texture_shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(texture_shadow, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= 9.0f;

    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}

vec3 calcLight(Light light)
{
    //ambient
    vec3 ambient = light.ambient * texture(texture_diffuse0, TexCoords).rgb;
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffuse * texture(texture_diffuse0, TexCoords).rgb;
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * light.specular * texture(texture_specular0, TexCoords).rgb;

    float shadow = shadows ? calcShadow(FragPosLightSpace) : 0.0f;
    shadow = min(shadow, 0.75f);

    return (ambient + (1.0f - shadow) * (diffuse + specular));
    //return ((1.0f - shadow) * (ambient + diffuse + specular));
}

void main()
{
    vec3 result = calcLight(light);
    FragColor = vec4(result, alpha);
}