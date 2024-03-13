#version 330

// very simple, emissive only shader

out vec4 finalColor;
uniform vec4 color;
void main()
{
	finalColor = color;
}