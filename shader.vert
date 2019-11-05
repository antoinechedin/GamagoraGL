#version 450

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outNormal;
out vec2 outTexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    outNormal = normal ;
    outTexCoord = texCoord;
}