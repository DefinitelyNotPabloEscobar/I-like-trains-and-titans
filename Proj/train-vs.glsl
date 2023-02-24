#version 330 core

uniform sampler3D noiseSampler;

in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;

uniform vec3 LightPosition;
uniform float Levers[4];

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out float exIntensity;
out vec4 mcPosition;
out vec4 ecPosition;
out vec3 ecNormal;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

const float SpecularC = 0.25;
const float DiffuseC = 0.7;
const float AmbientC = 1.0 - DiffuseC - SpecularC;

void main(void)
{
	exPosition = inPosition;
	exTexcoord = inTexcoord;
	exNormal = inNormal;

	mcPosition = vec4(inPosition, 1.0);
	ecPosition = ViewMatrix * ModelMatrix * mcPosition;
	ecNormal = NormalMatrix * inNormal;

	vec4 MCPosition = vec4(inPosition, 1.0);
	vec3 V = vec3(ModelMatrix * MCPosition);
	vec3 N = normalize(NormalMatrix * inNormal);
	vec3 L = normalize(LightPosition - V);
	vec3 R = reflect(-L, N);
	vec3 E = normalize(-V);
	vec3 H = normalize(L + E);

	float diffuse = max(dot(L, N), 0.0);
	float specular = 0.0;
	if(diffuse > 0.0){
		specular = max(dot(R,H), 0.0);
		specular = pow(specular, 16.0);
	}
	exIntensity = DiffuseC * diffuse + SpecularC * specular + AmbientC;

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;
}
