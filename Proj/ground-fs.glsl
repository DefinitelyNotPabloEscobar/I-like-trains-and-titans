#version 330 core

uniform sampler2D Text_Grass;
uniform sampler2D Displacement;
uniform sampler2D Text_Dirt;
uniform sampler2D Text_Snow;

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in float displacement;

out vec4 FragmentColor;

void main(void)
{
	if(displacement <= 0.1f){
		FragmentColor = texture(Text_Grass, exTexcoord * 250);
	}
	else if(displacement > 0.1f && displacement <= 0.5f){
		FragmentColor = texture(Text_Dirt, exTexcoord * 250);
	}
	else{
		FragmentColor = texture(Text_Snow, exTexcoord * 250);
	}
}