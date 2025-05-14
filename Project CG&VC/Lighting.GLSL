struct LightingResult {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

LightingResult calculateLighting(
    vec3 normal,
    vec3 fragPos,
    vec3 viewPos,
    vec3 lightPos,
    vec3 lightColor,
    float ambientStrength,
    float specularStrength,
    float shininess,
    float constant,
    float linear,
    float quadratic
) {
    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    LightingResult result;
    result.ambient = ambient * attenuation;
    result.diffuse = diffuse * attenuation;
    result.specular = specular * attenuation;
    return result;
}

