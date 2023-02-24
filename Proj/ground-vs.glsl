#version 330 core

uniform sampler2D Text_Grass;
uniform sampler2D Displacement;
uniform sampler2D Text_Dirt;
uniform sampler2D Text_Snow;

in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out float displacement;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exPosition = inPosition;
	exTexcoord = inTexcoord;
	exNormal = inNormal;

	displacement = texture(Displacement, exTexcoord).r;
	vec4 MCPosition = vec4(inPosition, 1.0);
	MCPosition.y += displacement * 250;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;
}
