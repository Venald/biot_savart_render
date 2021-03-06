#version 450 core

// Input/Output buffer
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;


out VS_OUT{
	vec4 color;
} vs_out;

// Transfer the data to fragment shader
void main(){
	gl_Position = position;
	vs_out.color  = color;

}