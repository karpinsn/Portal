#version 130

uniform sampler2D fringeImage1;
uniform sampler2D fringeImage2;

in vec2 fragTexCoord;
in float pitch1;
in float pitch2;

out vec4 phase;

void main(void)
{
	float pi = 3.14159265; // Mmmmmm PI
	float pitch12 = (pitch1 * pitch2) / abs(pitch1 - pitch2);
	
	vec4 fringe1 = texture(fringeImage1, fragTexCoord);
	vec4 fringe2 = texture(fringeImage2, fragTexCoord);
	
	float phi1 = atan( sqrt( 3.0 ) * ( fringe1.r - fringe1.b ), ( 2.0 * fringe1.g ) - fringe1.r - fringe1.b );
	float phi2 = atan( sqrt( 3.0 ) * ( fringe2.r - fringe2.b ), ( 2.0 * fringe2.g ) - fringe2.r - fringe2.b );
	float phi12 = mod( phi1 - phi2, 2.0 * pi );
	
	float k = floor( ( phi12 * ( pitch12 / pitch1 ) - phi1 ) / ( 2.0 * pi ) );
	phase = vec4( phi1 + k * 2.0 * pi );
}
