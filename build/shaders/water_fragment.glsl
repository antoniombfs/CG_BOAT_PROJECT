#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 WaterCoord;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform bool cameraLightEnabled;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float time;

vec3 calculateLight(vec3 lightPos, vec3 norm, vec3 viewDir, float intensity) {
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Especular mais forte para água
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
    
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    vec3 diffuse = diff * vec3(0.2, 0.4, 0.6);
    vec3 specular = spec * vec3(1.0, 1.0, 1.0) * 0.8;
    
    return (diffuse + specular) * attenuation * intensity;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Cor base da água com variação
    vec3 waterColorDeep = vec3(0.0, 0.15, 0.3);
    vec3 waterColorShallow = vec3(0.1, 0.3, 0.5);
    
    // Padrão de ondulação
    float pattern = sin(WaterCoord.x * 10.0 + time) * 0.5 + 0.5;
    pattern *= sin(WaterCoord.y * 10.0 + time * 0.8) * 0.5 + 0.5;
    
    vec3 waterColor = mix(waterColorDeep, waterColorShallow, pattern * 0.3);
    
    // Ambiente
    vec3 ambient = 0.3 * lightColor * waterColor;
    
    // Luzes
    vec3 light1 = calculateLight(lightPos1, norm, viewDir, 1.0) * lightColor;
    vec3 light2 = calculateLight(lightPos2, norm, viewDir, 0.5) * lightColor;
    
    vec3 light3 = vec3(0.0);
    if (cameraLightEnabled) {
        light3 = calculateLight(lightPos3, norm, viewDir, 0.8) * lightColor;
    }
    
    // Reflexo do céu (Fresnel simplificado)
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
    vec3 skyReflection = vec3(0.1, 0.15, 0.25) * fresnel;
    
    vec3 result = ambient + light1 + light2 + light3 + skyReflection;
    
    // Gamma correction
    result = pow(result, vec3(1.0/2.2));
    
    // Transparência baseada no ângulo de visão
    float alpha = 0.85 + fresnel * 0.15;
    
    FragColor = vec4(result, alpha);
}