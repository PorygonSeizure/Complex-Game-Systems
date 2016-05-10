#version 150

in vec4 position;
in vec4 colour;

out vec4 vertexColour;

uniform mat4 projectionView;

void main()
{
	vertexColour = colour;
	gl_Position = projectionView * position;
}