#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D emissiveMap; // Bind your emissive texture here

void main()
{
    vec3 emissive = texture(emissiveMap, TexCoords).rgb;
    FragColor = vec4(emissive, 1.0);
}