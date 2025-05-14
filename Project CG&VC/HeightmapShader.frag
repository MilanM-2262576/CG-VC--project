#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Height;

uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

void main() {
    // Sample all textures
    vec3 sand = texture(sandTexture, TexCoord).rgb;
    vec3 grass = texture(grassTexture, TexCoord).rgb;
    vec3 rock = texture(rockTexture, TexCoord).rgb;
    vec3 snow = texture(snowTexture, TexCoord).rgb;

    // Calculate blend factors based on height
    float h = Height;
    float sandWeight = clamp(1.0 - abs(h - 0.0) / 10.0, 0.0, 1.0);
    float grassWeight = clamp(1.0 - abs(h - 10.0) / 10.0, 0.0, 1.0);
    float rockWeight = clamp(1.0 - abs(h - 20.0) / 10.0, 0.0, 1.0);
    float snowWeight = h >= 30.0 ? 1.0 : clamp(1.0 - abs(h - 30.0) / 10.0, 0.0, 1.0);

    // Normalize weights
    float total = sandWeight + grassWeight + rockWeight + snowWeight;
    sandWeight /= total;
    grassWeight /= total;
    rockWeight /= total;
    snowWeight /= total;

    // Blend textures
    vec3 color = sand * sandWeight + grass * grassWeight + rock * rockWeight + snow * snowWeight;

    FragColor = vec4(color, 1.0);
}

