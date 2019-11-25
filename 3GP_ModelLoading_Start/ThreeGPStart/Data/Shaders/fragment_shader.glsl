#version 330
uniform sampler2D sampler_tex;
in vec3  varying_normal;
in vec2 varying_texcoord;
out vec4 fragment_colour;

void main(void)
{
	
	vec3 tex_colour = texture(sampler_tex, varying_texcoord).rgb;
	fragment_colour =vec4(tex_colour,1.0) ;
}