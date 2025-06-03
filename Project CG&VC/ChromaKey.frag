#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D overlayTexture;

void main() {
    vec4 overlay = texture(overlayTexture, TexCoords);

    // Chroma key: remove green background (tweak as needed)
    float greenThreshold = 0.7;
    bool isGreen = overlay.g > greenThreshold && overlay.g > overlay.r && overlay.g > overlay.b;

    if (isGreen || overlay.a < 0.1)
        discard;

    FragColor = overlay;
}