#version 130

uniform sampler2D unfilteredPhase;
uniform sampler2D filteredPhase;

uniform float pitch1;
uniform float pitch2;

in vec2 fragTexCoord;
out vec4 unwrappedPhase;

void main()
{	
	// Constants
	float pi = 3.14159265; // Mmmmmm PI
    float pitch12 = (pitch1 * pitch2) / abs(pitch1 - pitch2);

	// Unwrapping
	vec4 unfilteredPhases = texture(unfilteredPhase, fragTexCoord);
	vec4 filteredPhases = texture(filteredPhase, fragTexCoord);
	float k = floor( ( filteredPhases.b * ( pitch12 / pitch1 ) - unfilteredPhases.r ) / ( 2.0 * pi ) );
	unwrappedPhase = vec4( unfilteredPhases.r + k * 2.0 * pi );
}
