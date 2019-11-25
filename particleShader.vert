#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inSpeed;
layout (location = 2) in vec3 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct VS_OUT {
    vec3 color;
};

out VS_OUT vsOut;

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    vsOut.color = inColor;
}
