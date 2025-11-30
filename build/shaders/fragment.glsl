#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// Material do objeto
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

// Luzes
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;     // luz da câmara
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool cameraLightEnabled;

vec3 calculateLight(vec3 lightPos, vec3 norm, vec3 viewDir, float intensity)
{
    vec3 lightDir = normalize(lightPos - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

    vec3 ambient  = material.ambient  * 0.2 * intensity;
    vec3 diffuse  = material.diffuse  * diff * intensity;
    vec3 specular = material.specular * spec * intensity;

    return ambient + diffuse + specular;
}

void main()
{
    // ----- two-sided lighting: dar cor igual a backfaces -----
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) {
        norm = -norm;
    }
    // ---------------------------------------------------------

    vec3 viewDir = normalize(viewPos - FragPos);

    // componente ambiente base (ligada ao material)
    vec3 ambient = material.ambient * 0.25;

    // Luz 1 e 2 ambientes
    vec3 light1 = calculateLight(lightPos1, norm, viewDir, 0.7) * lightColor;
    vec3 light2 = calculateLight(lightPos2, norm, viewDir, 0.4) * lightColor;

    // Luz 3 (câmara - lanterna)
    vec3 light3 = vec3(0.0);
    if (cameraLightEnabled) {
        light3 = calculateLight(lightPos3, norm, viewDir, 0.8) * lightColor * vec3(1.0, 0.95, 0.9);
    }

    vec3 result = ambient + light1 + light2 + light3;

    // gamma correction
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}
