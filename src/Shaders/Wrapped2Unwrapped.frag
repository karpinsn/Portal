#version 130

precision highp float;
precision highp sampler2D;

uniform sampler2D unfilteredPhase;
uniform sampler2D filteredPhase;

uniform int pitch1;
uniform int pitch2;

uniform float m;
uniform float b1;
uniform float b2;
uniform float b3;
uniform bool rightSide;

in vec2 fragTexCoord;
out vec4 unwrappedPhase;

void main()
{
    vec4 unfilteredPhases = texture( unfilteredPhase, fragTexCoord );
    // If the components are zero that means we should filter off the fragment
    if( unfilteredPhases == vec4( 0.0 ) )
        { discard; }

    // Constants
	float twoPi = 6.28318530718; // Mmmmmm PI
    float pitch12 = float( pitch1 * pitch2 ) / float( abs( pitch1 - pitch2 ) );

	// Phase recovery	
    vec4 filteredPhases = texture( filteredPhase, fragTexCoord );
    float phi1 = atan( filteredPhases.x, filteredPhases.y );
    float phi2 = atan( filteredPhases.z, filteredPhases.a );
    float phi12 = mod( phi1 - phi2, twoPi );

    // Unwrapping
    float line1 = m * fragTexCoord.x + b1;
    float line2 = m * fragTexCoord.x + b2;
    float line3 = m * fragTexCoord.x + b3;

    bool unwrap = 
    ( rightSide && phi12 < line2 ) // Right side unwrapping
    ||
    ( !rightSide && ( ( phi12 > line2 && phi12 < line1 ) || ( phi12 < line2 && phi12 < line3 ) ) ); // Left side unwrapping
    phi12 = phi12 + int( unwrap ) * twoPi;

	float k = floor( ( phi12 * float( pitch12 / pitch1 ) - phi1 ) / twoPi );
    
    // 1 in the alpha component signifies that this is good phase
	unwrappedPhase = vec4( vec3( phi1 + k * twoPi ), 1.0 );
}
