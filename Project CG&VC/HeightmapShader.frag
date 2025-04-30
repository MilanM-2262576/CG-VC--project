#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Height;

uniform sampler2D heightmapTexture;

void main() {
	float h = (Height + 16) / 32.0f;	//Shift and scale the height into a grayscale value
	FragColor = texture(heightmapTexture, TexCoord) * vec4(h, h, h, 1.0f);
}