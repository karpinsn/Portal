#version 130

uniform sampler2D unfilteredPhase;
uniform sampler2D filteredPhase;

uniform float gammaCutoff;

uniform int pitch1;
uniform int pitch2;

in vec2 fragTexCoord;
out vec4 unwrappedPhase;

void main()
{	
	// Constants
	float pi = 3.14159265; // Mmmmmm PI
    float pitch12 = float(pitch1 * pitch2) / float(abs(pitch1 - pitch2));

	// Unwrapping
	vec4 unfilteredPhases = texture(unfilteredPhase, fragTexCoord);
	if (unfilteredPhases.a < gammaCutoff)
        { discard; }
    
    vec4 filteredPhases = texture(filteredPhase, fragTexCoord);
	float k = floor( ( filteredPhases.b * float( pitch12 / pitch1 ) - unfilteredPhases.r ) / ( 2.0 * pi ) );
	unwrappedPhase = vec4( unfilteredPhases.r + k * 2.0 * pi );
}
