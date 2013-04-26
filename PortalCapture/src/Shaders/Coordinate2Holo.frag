#version 130

precision highp float;
precision highp sampler2D;

uniform float fringeFrequency;

in vec4 projectorPosition;
out vec4 holoImage;

void main()
{
	//	Constants for encoding
	float pi = 3.14159265;
	float stepHeight = 1.0 / (2.0 * fringeFrequency);
	float stepWidth = 1.0 / fringeFrequency;
	float angularFrequency = 2.0 * pi * fringeFrequency;
	float stairAngularFrequency = angularFrequency * (4.0 + .5);
	
	float b = floor(projectorPosition.x * fringeFrequency) * stepHeight + .5;
   
	holoImage = vec4((1.0 - sin(angularFrequency * projectorPosition.x)) * .5, 
						(1.0 - cos(angularFrequency * projectorPosition.x)) * .5, 
						cos(stairAngularFrequency * (projectorPosition.x - (b * (1.0 / stepHeight) * stepWidth)) + pi) * (stepHeight / 3.0) + (stepHeight / 2.0) + b, 
						1.0);
}

