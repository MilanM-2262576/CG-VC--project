#version 330 core
#define MAX_LIGHTS 6

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

uniform int numLights;
uniform PointLight lights[MAX_LIGHTS];

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
//uniform vec3 objectColor;
uniform vec3 viewPos;
uniform sampler2D colormap;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    vec4 objectColor = texture(colormap, TexCoords);

    for (int i = 0; i < numLights; ++i) {
        // Attenuation
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));

        // Ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lights[i].color * objectColor.rgb;

        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * objectColor.rgb;

        // Specular
        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lights[i].color;

        // Sum and apply attenuation
        result += attenuation * (ambient + diffuse + specular);
    }

    FragColor = vec4(result, 1.0);
}