#version 450

in vec3 outNormal;
in vec2 outTexCoord;

uniform sampler2D textureSampler;

out vec4 fragColor;

void main()
{
    fragColor = texture(textureSampler ,outTexCoord);
//    fragColor = vec4(1, 0, 0, 1);
}