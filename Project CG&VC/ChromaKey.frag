#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D overlayTexture;

void main() {
    vec4 overlay = texture(overlayTexture, TexCoords);
    vec4 scene = texture(sceneTexture, TexCoords);

    // Simpele chroma-key op fel groen
    float greenThreshold = 0.6;
    float diff = abs(overlay.r - 0.0) + abs(overlay.g - 1.0) + abs(overlay.b - 0.0);

    if (diff < greenThreshold && overlay.a > 0.1)
        FragColor = scene;
    else
        FragColor = overlay;
}
