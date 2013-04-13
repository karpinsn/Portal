#version 130

uniform sampler2D image;

uniform float width;
uniform float height;

in vec2 fragTexCoord;
out vec4 rectifiedImage;

void main(void)
{
	//	Inverse of Camera matrix : inv(A)
	mat3 intrinsicInv = mat3(.0005834604494303656, 0.0, 0.0, 0.0, 0.0005841519687703012, 0.0, -0.225859608925248, -0.275495734434701, 1.0);
	mat3 intrinsic = mat3(1713.912230000000, 0.0, 0.0, 0.0, 1711.883300000000, 0.0, 387.1035460000000e+02, 471.6165470000000e+02, 1.0);
	mat3 rotationInv = mat3(-.0006922928920737852, 0.974833857550000, -0.222931435350389, 0.999314707773841, 0.007576099188223, -0.036232019861595, -0.037009153429021, -0.222803733992120, -0.974160588912614);
	
	float distCoeff[5];
	distCoeff[0] = -.179589480;
	distCoeff[1] =	.302766383;
	distCoeff[2] =	-.00217730156; 
	distCoeff[3] = .0000917747675;
	distCoeff[4] =	-1.93590903;
	
	//	Texture Coordinates to camera coordinates
	vec3 newTex = (intrinsicInv * vec3(fragTexCoord.st * vec2(640,480), 1.0));
	
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
	
	//	Remove rotation
	newTex = rotationInv * newTex;
	
	//	Put it back to texture coordinates
	vec3 afineTex = intrinsic * (newTex.xyz / newTex.zzz);// * scale + shift;
	
	afineTex = afineTex / vec3(640, 480, 1);
	
	rectifiedImage = texture2D(image, afineTex.st);
}