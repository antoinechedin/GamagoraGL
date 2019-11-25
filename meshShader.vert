#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

struct VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLightSpace;
};

out VS_OUT vsOut;

void main()
{
    vsOut.fragPos = vec3(model * vec4(inPosition, 1.0));
    vsOut.normal = mat3(transpose(inverse(model))) * inNormal;
    vsOut.texCoord = inTexCoord;
    vsOut.fragPosLightSpace = lightSpaceMatrix * vec4(vsOut.fragPos, 1.0);

    gl_Position = projection * view * vec4(vsOut.fragPos, 1.0);
}