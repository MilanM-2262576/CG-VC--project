#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 waterColor;
uniform float transparency;
uniform sampler2D waterTexture;

void main() {
	vec4 textureColor = texture(waterTexture, TexCoord);

	vec3 finalColor = mix(waterColor, textureColor.rgb, transparency);

	FragColor = vec4(finalColor, transparency);
}