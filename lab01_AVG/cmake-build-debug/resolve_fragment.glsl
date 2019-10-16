#version 420 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    vec4 color = texture(screenTexture, TexCoords);
    FragColor = vec4(color.rgb / color.a, 1.0f);
}