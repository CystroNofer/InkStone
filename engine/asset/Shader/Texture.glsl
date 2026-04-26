#type vertex
#version 410 core

layout(location = 0) in vec3 a_PositionOS;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_VPMatrix;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;

	gl_Position = u_VPMatrix * u_ModelMatrix * vec4(a_PositionOS, 1);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_MainTex;

void main()
{
	color = texture(u_MainTex, v_TexCoord);
}