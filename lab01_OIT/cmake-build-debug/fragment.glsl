#version 420 core
layout (early_fragment_tests) in;

layout (binding = 0, offset = 0) uniform atomic_uint index_counter;
layout (binding = 0, rgba32ui) uniform uimageBuffer list_buffer;
layout (binding = 1, r32ui) uniform uimage2D head_pointer_image;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform float alpha;

void main()
{
    uint index = atomicCounterIncrement(index_counter);
    uint old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), index);
    uvec4 item;

    //ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    //specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, alpha);

    item.x = old_head;
    item.y = packUnorm4x8(FragColor);
    item.z = floatBitsToUint(gl_FragCoord.z);
    item.w = 0;
    imageStore(list_buffer, int(index), item);
}