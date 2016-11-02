#version 330 core

in vec3 passedColor;

uniform vec3 Color;

out vec4 color;

void main()
{
	vec3 finalColor = passedColor * Color;
    color = vec4(finalColor, 1.0f);
} 