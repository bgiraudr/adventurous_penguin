#version 400

in vec4 surfel_color;

uniform float time;

out vec4 fragmentColor;

void main()
{
    fragmentColor = surfel_color;
}