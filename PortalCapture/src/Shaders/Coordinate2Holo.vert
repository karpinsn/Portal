#version 130

uniform mat4 modelView;
uniform mat4 projectorModelView;
uniform mat4 projectionMatrix;
uniform sampler2D coordinateMap;

in vec3 vert;
in vec2 vertTexCoord;

out vec4 projectorPosition;

void main()
{	
	//	Need Modelview for the boundingbox. That will give us world coord relative to the box
	projectorPosition = projectionMatrix * projectorModelView * vec4(vert, 1.0);
	gl_Position = projectionMatrix * modelView * vec4(vert, 1.0);
} 
