#version 450

in vec3 outNormal;
out vec4 fragColor;

void main()
{
    fragColor = vec4(abs(outNormal), 1.0);;
}