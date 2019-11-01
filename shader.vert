#version 450

in vec3 position;

uniform float logoScale;

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(position * logoScale, 1.0);
    vertexColor = vec4(0.9, 0.5, 0.2, 1.0);
}