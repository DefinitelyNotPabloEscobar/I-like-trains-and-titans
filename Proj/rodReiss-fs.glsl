#version 330 core

uniform sampler2D Displacement;

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

out vec4 FragmentColor;


void main(void)
{
	// no texture

	FragmentColor = vec4(0.88f , 0.6f + exNormal.x*0.1f, 0.57f + exNormal.y*0.1f, 1.0f);

	//FragmentColor = texture(Text, exTexcoord);
}