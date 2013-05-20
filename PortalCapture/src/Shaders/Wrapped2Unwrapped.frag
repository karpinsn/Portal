#version 130

uniform sampler2D unfilteredPhase;
uniform sampler2D filteredPhase;

uniform int pitch1;
uniform int pitch2;

in vec2 fragTexCoord;
out vec4 unwrappedPhase;

void main()
{
    vec4 unfilteredPhases = texture( unfilteredPhase, fragTexCoord );
    // If the components are zero that means we should filter off the fragment
    if( unfilteredPhases == vec4( 0.0 ) )
        { discard; }

    // Constants
	float pi = 3.14159265; // Mmmmmm PI
    float pitch12 = float( pitch1 * pitch2 ) / float( abs( pitch1 - pitch2 ) );

	// Unwrapping	
    vec4 filteredPhases = texture( filteredPhase, fragTexCoord );
    float phi1 = atan( filteredPhases.x, filteredPhases.y );
    float phi2 = atan( filteredPhases.z, filteredPhases.a );
    float phi12 = mod( phi1 - phi2, 2.0 * pi );

	float k = floor( ( phi12 * float( pitch12 / pitch1 ) - phi1 ) / ( 2.0 * pi ) );
    // 1 in the alpha component signifies that this is good phase
	unwrappedPhase = vec4( vec3( phi1 + k * 2.0 * pi ), 1.0 );
}
