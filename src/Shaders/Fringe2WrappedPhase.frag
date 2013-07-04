#version 130

uniform sampler2D fringeImage1;
uniform sampler2D fringeImage2;

uniform float gammaCutoff;
uniform float intensityCutoff;

in vec2 fragTexCoord;

out vec4 phase;

void main(void)
{
	float pi = 3.14159265; // Mmmmmm PI
	
	vec4 fringe1 = texture(fringeImage1, fragTexCoord);
	vec4 fringe2 = texture(fringeImage2, fragTexCoord);
	
	float phi1 = atan( sqrt( 3.0 ) * ( fringe1.r - fringe1.b ), ( 2.0 * fringe1.g ) - fringe1.r - fringe1.b );
	float phi2 = atan( sqrt( 3.0 ) * ( fringe2.r - fringe2.b ), ( 2.0 * fringe2.g ) - fringe2.r - fringe2.b );

    // Calculate intensity and gamma for filtering
    float intensity1 = sqrt(pow((2.0 * fringe1.g - fringe1.r - fringe1.b), 2.0) + 3.0 * pow((fringe1.r - fringe1.b), 2.0));
    float intensity2 = sqrt(pow((2.0 * fringe2.g - fringe2.r - fringe2.b), 2.0) + 3.0 * pow((fringe2.r - fringe2.b), 2.0));
    float gamma1 = intensity1 / (fringe1.r + fringe1.g + fringe1.b);	
    float gamma2 = intensity2 / (fringe2.r + fringe2.g + fringe2.b);
  
    float intensity = min(intensity1, intensity2); 
    float gamma     = min(gamma1, gamma2);  	
   
    if(gamma < gammaCutoff || intensity < intensityCutoff)
        { phase = vec4(0.0); }
    else
        { phase = vec4(sin(phi1), cos(phi1), sin(phi2), cos(phi2)); }
}