#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0); // Zet de punten in clip space
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); //kleur
}