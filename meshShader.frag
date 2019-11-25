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

struct VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLightSpace;
};

in VS_OUT vsOut;

uniform vec3 viewPosition;
uniform Material material;
uniform sampler2D shadowMap;
uniform Light light;

out vec4 fragColor;

float shadowCalculation(vec4 fragPosLightSpace)
{
    float bias = 0.0005;

    vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoord = projCoord * 0.5 + 0.5;
    float closetDepth = texture(shadowMap, projCoord.xy).r;
    float currentDepth = projCoord.z;

    return currentDepth - bias> closetDepth ? 1.0 : 0.0;
}

void main()
{
    vec3 norm = normalize(vsOut.normal);
    vec3 lightDir = normalize(light.position - vsOut.fragPos);
    vec3 viewDir = normalize(viewPosition - vsOut.fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vsOut.texCoord));;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, vsOut.texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, vsOut.texCoord));

    float shadow = shadowCalculation(vsOut.fragPosLightSpace);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse);

    fragColor = vec4(lighting, 1);
}