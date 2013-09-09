#version 130

precision highp float;
precision highp sampler2D;

uniform mat4 modelView;
uniform mat4 projectionMatrix;

uniform sampler2D coordinateMap;
uniform sampler2D textureMap;

uniform float pointSize;
 
in vec3 vert;
in vec2 vertTexCoord;

out vec3 fragPosition;
out vec3 fragTexture;

void main()
{ 
    vec4 newVert = texture(coordinateMap, vertTexCoord);

    // Check if we are supposed to discard this
    if( newVert.a == 0.0 )
    {
        // Just set the vec to an invalid value and it will go away
        newVert = vec4( vec3( -100.0 ), 0.0 );
    }
   	
	//	Need Modelview for the boundingbox. That will give us world coord relative to the box
    fragPosition = newVert.xyz;  
	fragTexture = texture( textureMap, vertTexCoord ).rgb;
	
    gl_PointSize = pointSize;
    gl_Position  = projectionMatrix * modelView * newVert; 
} 
