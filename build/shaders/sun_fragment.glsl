#version 430 core

out vec4 FragColor;

uniform vec3 sunColor;

void main() {
    // Gradiente radial para efeito de brilho
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    float glow = 1.0 - smoothstep(0.0, 0.5, dist);
    
    vec3 color = sunColor * (0.8 + glow * 0.2);
    FragColor = vec4(color, 1.0);
}