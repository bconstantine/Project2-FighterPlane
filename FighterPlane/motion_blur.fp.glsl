#version 410

layout(binding = 0) uniform sampler2D tex1;
layout(binding = 1) uniform sampler2D tex2;
layout(binding = 2) uniform sampler2D tex3;
layout(binding = 3) uniform sampler2D tex4;
layout(binding = 4) uniform sampler2D tex5;

out vec4 color; 
in VS_OUT
{
vec2 texcoord; 
} fs_in;

void main(void)
{
	vec4 colorTex1 = texture(tex1, fs_in.texcoord);
	vec4 colorTex2 = texture(tex2, fs_in.texcoord);
	vec4 colorTex3 = texture(tex3, fs_in.texcoord);
	vec4 colorTex4 = texture(tex4, fs_in.texcoord);
	vec4 colorTex5 = texture(tex5, fs_in.texcoord);
	color = mix(colorTex5, colorTex4, 0.5);
	color = mix(color, colorTex3, 0.5);
	color = mix(color, colorTex2, 0.5);
	color = mix(color, colorTex1, 0.5);

	color.a = 0.8;
	
}