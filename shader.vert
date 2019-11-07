#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 normal;
out vec2 texCoord;

void main()
{
    fragPosition = vec3(model * vec4(inPosition, 1.0));
    normal = mat3(transpose(inverse(model))) * inNormal ;
    texCoord = inTexCoord;

    gl_Position = projection * view * vec4(fragPosition, 1.0);
}