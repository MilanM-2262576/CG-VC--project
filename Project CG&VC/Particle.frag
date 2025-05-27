#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D fireTexture;
uniform float alpha;

void main() {
    vec4 tex = texture(fireTexture, TexCoord);

    vec3 fireColor = vec3(1.0, 0.4, 0.1);
    vec3 colored = tex.rgb * fireColor;

    FragColor = vec4(colored, tex.a * alpha);
    if (FragColor.a < 0.1) discard;
}
