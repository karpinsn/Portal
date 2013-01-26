#version 130

uniform sampler2D actualPhase;
uniform sampler2D referencePhase;

uniform float scale;
uniform float shift;

in vec2 fragTexCoord;
out vec4 depthMap;

void main()
{	
	float aPhase = texture(actualPhase, fragTexCoord).r;
	float rPhase = texture(referencePhase, fragTexCoord).r;

	depthMap = vec4( vec3( ( ( aPhase - rPhase ) * scale ) + shift ), 1.0);
}
