#version 330

// very simple shader which kust transforms the position

in vec3 vertexPos;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4( vertexPos, 1.0f);
}