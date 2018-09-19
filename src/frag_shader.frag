#version 450 core

uniform vec2 b_x_min_max;
uniform vec2 b_y_min_max;
uniform vec2 b_z_min_max;

out vec4 color;

in VS_OUT{
	vec4 color;
} fs_in;

// scale function
void Scale(in float x, in float min_value, in float max_value, in float min_range, in float max_range, out float scaled_x){
	scaled_x = (max_range-min_range)*(x-min_value)/(max_value-min_value)+min_range;
}
void main(){
	float scaled_b_x = 0;
	float scaled_b_y = 0;
	float scaled_b_z = 0;

	// Scale the color so it looks good in the final picture
	Scale(fs_in.color.x,b_x_min_max.x,b_x_min_max.y,0,1,scaled_b_x);
	Scale(fs_in.color.y,b_y_min_max.x,b_y_min_max.y,0,1,scaled_b_y);
	Scale(fs_in.color.z,b_z_min_max.x,b_z_min_max.y,0,1,scaled_b_z);

	color = clamp(vec4(vec3(fs_in.color.x+fs_in.color.y+fs_in.color.z),1),0,1);
}