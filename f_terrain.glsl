#version 330

out vec4 finalColor;

in float light;
in vec2 textureUV;
in vec3 texWeights;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
	vec4 mat = texture(tex0, textureUV) * texWeights.x +
	texture(tex1, textureUV) * texWeights.y +
	texture(tex2, textureUV) * texWeights.z;
	finalColor = vec4(mat.rgb*light,1);
}