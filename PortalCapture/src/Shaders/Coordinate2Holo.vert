#version 130

precision highp float;
precision highp sampler2D;

uniform mat4 modelView;
uniform mat4 projectorModelView;
uniform mat4 projectionMatrix;
uniform sampler2D coordinateMap;

in vec3 vert;
in vec2 vertTexCoord;

out vec4 projectorPosition;

void main()
{
    vec4 newVert = texture(coordinateMap, vertTexCoord);
    	
	//	Need Modelview for the boundingbox. That will give us world coord relative to the box
	projectorPosition = projectionMatrix * projectorModelView * newVert;  
    gl_Position = projectionMatrix * modelView * newVert; 
} 
