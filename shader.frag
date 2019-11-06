#version 450

in vec3 normal;
in vec2 texCoord;

uniform sampler2D textureSampler;

out vec4 fragColor;

void main()
{
    fragColor = texture(textureSampler, texCoord);
}