#version 410

in vec4 position;
in vec4 colour;

out vec4 vertexColour;

uniform mat4 projectionView;
uniform float time;
uniform float heightScale;

void main()
{
	vertexColour = colour;
	vec4 p  = position;
	p.y += sin(time + position.x) * heightScale;
	gl_Position = projectionView * p;
}