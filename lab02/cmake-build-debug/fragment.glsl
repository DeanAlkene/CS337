#version 420 core

#define NR_LIGHTS 2
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

out vec4 FragColor;

uniform sampler2D texture_specular0;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_normal0;
uniform sampler2D texture_height0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform vec3 viewPos;
uniform Light light[NR_LIGHTS];
uniform float alpha;

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
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = spec * light.specular * texture(texture_specular0, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        result += calcLight(light[i]);
    }
    FragColor = vec4(result, alpha);
}