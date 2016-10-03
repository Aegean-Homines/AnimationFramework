#version 330 core

in vec3 passedColor;

out vec4 color;

void main()
{
    color = vec4(passedColor, 1.0f);
} 