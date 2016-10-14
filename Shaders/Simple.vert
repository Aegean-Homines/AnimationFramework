#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 Transform;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 passedColor;

void main()
{
	passedColor = color;
    gl_Position = ProjectionMatrix * ViewMatrix * Transform * vec4(position, 1.0);
}