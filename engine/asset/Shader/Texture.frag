#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_MainTex;

void main()
{
	color = texture(u_MainTex, v_TexCoord);
}
