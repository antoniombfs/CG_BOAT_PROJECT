#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 WaterCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main() {
    // Simulação de ondas com múltiplas frequências
    float wave1 = sin(aPos.x * 0.5 + time * 2.0) * 0.08;
    float wave2 = sin(aPos.z * 0.3 + time * 1.5) * 0.06;
    float wave3 = sin((aPos.x + aPos.z) * 0.4 + time * 2.5) * 0.04;
    
    vec3 pos = aPos;
    pos.y += wave1 + wave2 + wave3;
    
    FragPos = vec3(model * vec4(pos, 1.0));
    
    // Calcular a normal dinâmica baseada nas ondas
    float dx = cos(aPos.x * 0.5 + time * 2.0) * 0.04 +
               cos((aPos.x + aPos.z) * 0.4 + time * 2.5) * 0.016;
    float dz = cos(aPos.z * 0.3 + time * 1.5) * 0.018 +
               cos((aPos.x + aPos.z) * 0.4 + time * 2.5) * 0.016;
    
    vec3 normal = normalize(vec3(-dx, 1.0, -dz));
    Normal = mat3(transpose(inverse(model))) * normal;
    
    WaterCoord = aPos.xz * 0.1;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}