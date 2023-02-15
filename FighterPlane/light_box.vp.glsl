#version 430

layout(location = 0) in vec3 vPosition;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 ModelMatrix;

void main() 
{ 
    gl_Position = Projection*View*ModelMatrix*vec4(vPosition,1);
}