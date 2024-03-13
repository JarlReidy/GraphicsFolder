#version 330
in vec3 pixelPos;
in vec3 pixelNorm;
in vec2 pixelUV;
in vec3 pixelTangent;

out vec4 finalColor;
uniform vec3 lightK; // intensities for ambient, diffuse, specular
uniform float alpha; // specular power
uniform mat4 lightPos; // light position, world space
uniform vec3 cameraPos; // camera position, world space
uniform mat4 ambientColor;
uniform mat4 lightColor; // assume diffuse and specular the same
uniform vec4 attenuation; // 1/d_a**2
uniform sampler2D textureSampler; 
uniform sampler2D normalSampler;

void main()
{
// normals and normal mapping
	vec3 n = normalize(pixelNorm); // surface normal
	vec3 t = normalize(pixelTangent); // surface tangent 
	vec3 b = cross(t, n); // bitangent 
	vec4 normalColor = texture(normalSampler, pixelUV); 
	n = 2*(normalColor.r-0.5)*t + 2*(normalColor.g-0.5)*b + 2*(normalColor.b-0.5)*n; 
	n = normalize(n); 
	
// four point lights
	finalColor = vec4(0.0f,0.0f,0.0f,0.0f);
    vec4 materialColor = texture(textureSampler, pixelUV);
    for ( int i = 0; i < 4; i++ )
    {
       vec3 l = normalize(vec3(lightPos[i]) - pixelPos); // light vector
       vec3 e = normalize(cameraPos - pixelPos); // eye vector
       vec3 r = reflect(-l,n); // reflected light vector

       float d = length(vec3(lightPos[i]) - pixelPos);
       float fAttenuation = 1.0f/(1.0f + d*d*attenuation[i]);
       float kdiffuse = max(dot(l,n),0) * lightK.y * fAttenuation;
       float kspecular = pow(max(dot(r,e),0.0f),alpha) * lightK.z * fAttenuation;
       finalColor += materialColor*lightColor[i]*kdiffuse +kspecular*lightColor[i];
    }
    float kambient = lightK.x;
    finalColor.xyzw += materialColor*ambientColor[0]*kambient;
    finalColor.a = 1.0f;
}