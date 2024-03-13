#version 330
in vec3 vertexPos;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec3 vertexTangent;

out vec3 pixelPos; // passthrough - world space position
out vec3 pixelNorm; // passthrough - world space normal
out vec2 pixelUV; // passthrough - uv coordinate
out vec3 pixelTangent; // passthrough - world space tangent
uniform mat4 model;
uniform mat4 viewProj; // view-projection 

void main()
{
	// transform vertex position and normal to world space
	pixelPos = vec3(model * vec4( vertexPos, 1.0f));
	pixelNorm = vec3(model * vec4( vertexNormal, 0.0f));
	pixelTangent = vec3(model * vec4( vertexTangent, 0.0f));
	// passthrough
	pixelUV = vertexUV;
	// final vertex position
	gl_Position = viewProj * vec4( pixelPos, 1.0f);
}