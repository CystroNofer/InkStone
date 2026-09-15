#version 450 core

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
