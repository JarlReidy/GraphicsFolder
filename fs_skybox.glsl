#version 420

uniform samplerCube cubeMap;
in vec3 texCoord;
out vec4 finalColor;

void main() {
    vec4 color = texture(cubeMap, texCoord);
   finalColor = color;
}