#version 450

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fragPosition;
in vec3 normal;
in vec2 texCoord;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

out vec4 fragColor;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPosition);
    vec3 viewDir = normalize(viewPosition - fragPosition);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoord));;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoord));

//     fragColor = vec4(abs(normal), 1.0);
    fragColor = vec4( ambient + (diffuse) * 1.4, 1);
}