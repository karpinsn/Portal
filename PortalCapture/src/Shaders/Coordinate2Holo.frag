#version 130

precision highp float;
precision highp sampler2D;

uniform float fringeFrequency;
uniform mat4 projectorModelView;
uniform mat4 projectionMatrix;
uniform sampler2D coordinateMap;

in vec2 fragTexCoord;
out vec4 holoImage;

void main()
{
    vec4 coord = texture(coordinateMap, fragTexCoord);
    if(coord.a == 0.0) // If we have zero in alpha, then we haven't blended and shouldn't render
        discard;

	//	Constants for encoding
	float pi = 3.14159265;
	float stepHeight = 1.0 / (2.0 * fringeFrequency);
	float stepWidth = 1.0 / fringeFrequency;
	float angularFrequency = 2.0 * pi * fringeFrequency;
	float stairAngularFrequency = angularFrequency * (4.0 + .5);
	
    float projectorX = (projectionMatrix * projectorModelView * vec4(coord.rgb, 1.0)).x;

	float b = floor(projectorX * fringeFrequency) * stepHeight + .5;

	holoImage = vec4((1.0 - sin(angularFrequency * projectorX)) * .5, 
						(1.0 - cos(angularFrequency * projectorX)) * .5, 
						cos(stairAngularFrequency * (projectorX - (b * (1.0 / stepHeight) * stepWidth)) + pi) * (stepHeight / 3.0) + (stepHeight / 2.0) + b, 
						1.0);
}

