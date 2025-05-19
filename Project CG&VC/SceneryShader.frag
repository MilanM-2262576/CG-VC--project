#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D colormap;

void main()
{
    vec3 texColor = texture(colormap, TexCoords).rgb;
    FragColor = vec4(texColor, 1.0);
}
