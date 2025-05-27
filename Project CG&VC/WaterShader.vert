#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float u_Time;

out vec2 TexCoord;

void main()
{
    vec3 pos = aPos;
    // Simple wave: displace y by a sine function
    pos.y += sin(pos.x * 0.1 + u_Time) * 0.5 + cos(pos.z * 0.1 + u_Time) * 0.5;
    gl_Position = projection * view * model * vec4(pos, 1.0);
    TexCoord = aTexCoord;
}
