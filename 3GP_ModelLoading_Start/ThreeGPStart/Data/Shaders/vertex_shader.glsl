#version 330

// TODO
uniform sampler2D sampler_tex;
uniform mat4 combined_xform;
uniform mat4 model_xform;
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 tex_coord;

out vec3 varying_normal;
out vec2 varying_texcoord;
void main(void)
{	
	varying_texcoord = tex_coord;
	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}