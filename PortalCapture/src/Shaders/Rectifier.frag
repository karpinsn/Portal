#version 130

uniform sampler2D image;

uniform float width;
uniform float height;

void main(void)
{
	//	Inverse of Camera matrix : inv(A)
	mat3 intrinsicInv = mat3(.0015201485, 0.0, 0.0, 0.0, .0015188412, 0.0, -.4610709568, -.3682513097, 1.0);
	mat3 intrinsic = mat3(657.8304691502, 0.0, 0.0, 0.0, 658.3966698018, 0.0, 303.3065238538, 242.4554359752, 1.0);
	float distCoeff[5];
	distCoeff[0] = -.2553;
	distCoeff[1] =	.1256;
	distCoeff[2] =	-.0002; 
	distCoeff[3] = .0001;
	distCoeff[4] =	0.0;
	
	//	Texture Coordinates to camera coordinates
	vec3 newTex = (intrinsicInv * vec3(gl_TexCoord[0].st * vec2(640,480), 1.0));
	
	//	Fix radial distortion
	float r2 = newTex.s * newTex.s + newTex.t * newTex.s;
	float r4 = r2 * r2;
	float r6 = r4 * r2;
	float cdist = 1.0 + distCoeff[0] * r2 + distCoeff[1] * r4 + distCoeff[4] * r6;
	newTex = newTex * vec3(cdist, cdist, 1.0);
	
	//	Fix tangental distortion
	float a1 = 2.0 * newTex.x * newTex.y;
	float a2 = r2 + 2.0 * newTex.x * newTex.x;
	float a3 = r2 + 2.0 * newTex.y * newTex.y;
	newTex = newTex + vec3(distCoeff[2] * a1 + distCoeff[3] * a2, distCoeff[2] * a3 + distCoeff[3] * a1, 0.0);
	
	//	Put it back to texture coordinates
	vec3 afineTex = intrinsic * (newTex.xyz / newTex.zzz);// * scale + shift;
	
	afineTex = afineTex / vec3(640, 480, 1);
	
	vec4 tmp = texture2D(image, afineTex.st);
	gl_FragColor = vec4(tmp);
}