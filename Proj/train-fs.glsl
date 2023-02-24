#version 330 core

uniform sampler3D noiseSampler;
uniform mat4 ModelMatrix;

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec4 mcPosition;
in vec4 ecPosition;
in vec3 ecNormal;

in float exIntensity;

uniform vec3 LightPosition;
uniform float Levers[4];
uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

out vec4 FragmentColor;

const vec3 LightWood = vec3(0.9, 0.45, 0.1);
const vec3 DarkWood = vec3(0.4, 0.2, 0.05);
const float NoiseScale = 0.8;
const float RingFrequency = 0.8;
const float RingSharp = 0.8;
const float RingScale = 3.0;

void main(void)
{

	// no texture
	//FragmentColor = vec4(color, 1.0);
	// 2d text
	//FragmentColor = texture(noiseSampler, exTexcoord) * exIntensity;
	
	float NoiseScale_ = Levers[0] * NoiseScale;
	float RingFrequency_ = Levers[1] * RingFrequency;
	float RingSharp_ = Levers[2] * RingSharp;
	float RingScale_ = Levers[3] * RingScale;

	vec3 p = vec3(mcPosition) * 0.5 + 0.5;
	float signed_noise = texture(noiseSampler, p * NoiseScale_).r;

	float r = fract(RingFrequency_ * p.z + RingScale_ * signed_noise);

	float invMax = pow(RingSharp_, RingSharp_/(RingSharp_ - 1.0)) / (RingSharp_ - 1.0);
	float ring = invMax * (r - pow(r, RingSharp_));

	float lrp = sin(fract(RingFrequency_ * p.z + RingScale_ * signed_noise)) + RingSharp_ * signed_noise;

	vec3 base = mix(DarkWood, LightWood, lrp);

	FragmentColor = vec4(vec3(base) * exIntensity, 1.0);
}