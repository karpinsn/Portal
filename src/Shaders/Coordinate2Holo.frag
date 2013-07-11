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

    // Step 1 - Normalizing
    // Since we have been doing blending not all the fragments will have an alpha
    // of 1.0 (either more or less). We need to normalize all the colors so that
    // they are correctly sampled
    coord = coord.xyzw / coord.wwww;

    // Step 2 - Holo encoding
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
						cos(stairAngularFrequency * (projectorX - ((b / stepHeight) * stepWidth)) + pi) * (stepHeight / 4.5) + (stepHeight / 2.0) + b, 
						1.0);
}

