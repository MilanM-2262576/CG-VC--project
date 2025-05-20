#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloom;
uniform float bloomStrength;

void main() {
    vec3 sceneColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloom, TexCoords).rgb;
    FragColor = vec4(sceneColor + bloomColor * bloomStrength, 1.0f);
    FragColor.a = 1.0f;
}