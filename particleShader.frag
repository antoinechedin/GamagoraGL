#version 450

struct VS_OUT {
    vec3 color;
};

in VS_OUT vsOut;

out vec4 fragColor;

void main()
{
    fragColor = vec4(vsOut.color, 1);
}