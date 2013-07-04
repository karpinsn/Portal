#version 150

// We are doing inverses and need high precision so we dont incur error during triangulation
precision highp float;
precision highp sampler2D;

uniform sampler2D actualPhase;

uniform int fringePitch;
uniform float Phi0;

uniform int cameraWidth;
uniform int cameraHeight;
uniform mat3 cameraIntrinsic;
uniform mat4x3 cameraExtrinsic;
uniform float cameraDistortion[5];

// Matrix is actually 3x4. OpenGL is column major and mathmatically backwards
uniform mat4x3 projectorMatrix;

in vec2 fragTexCoord;

out vec4 coordinateMap;

vec2 FixDistortion(vec2 uvCoord, mat3 intrinsic, float distCoeff[5])
{
    vec3 fixedCoordinate = inverse(intrinsic) * vec3(uvCoord, 1.0);
    
    // Fix Radial Distortion first
    float r2 = fixedCoordinate.s * fixedCoordinate.s + fixedCoordinate.t * fixedCoordinate.t;
    float r4 = r2 * r2;
    float r6 = r4 * r2;
    float cdist = 1.0 + distCoeff[0] * r2 + distCoeff[1] * r4 + distCoeff[4] * r6;
	fixedCoordinate = fixedCoordinate * vec3(cdist, cdist, 1.0); 

    // Fix Tangental Distortion
    float a1 = 2.0 * fixedCoordinate.x * fixedCoordinate.y;
	float a2 = r2 + 2.0 * fixedCoordinate.x * fixedCoordinate.x;
	float a3 = r2 + 2.0 * fixedCoordinate.y * fixedCoordinate.y;
	fixedCoordinate = fixedCoordinate + vec3(distCoeff[2] * a1 + distCoeff[3] * a2, distCoeff[2] * a3 + distCoeff[3] * a1, 0.0);

    return (intrinsic * (fixedCoordinate.xyz / fixedCoordinate.zzz)).st;
}

void main()
{
    float pi = 3.14159265;
    vec4 phase = texture(actualPhase, fragTexCoord);
    if( phase.a < 1.0 )
    { 
        coordinateMap = vec4(0.0); 
        return;
    }

	float Phi = phase.r; 
    float uProjector = 1.0 + ( ( Phi - Phi0 ) / ( ( 2.0 * pi ) / float( fringePitch) ) );
    mat4x3 cameraMatrix = cameraIntrinsic * cameraExtrinsic;
     
    mat3 Sl = mat3(cameraMatrix[0][0], cameraMatrix[0][1], projectorMatrix[0][0],
                   cameraMatrix[1][0], cameraMatrix[1][1], projectorMatrix[1][0],
                   cameraMatrix[2][0], cameraMatrix[2][1], projectorMatrix[2][0]);
    
    // PROFILE - Check if this is faster than the method above for building this matrix
    //mat3 Sl = transpose(mat3(transpose(cameraMatrix)[0], transpose(cameraMatrix[1]), transpose(projectorMatrix[0])));

    vec2 distortedUVCamera = fragTexCoord * vec2(cameraWidth, cameraHeight);
    vec2 fixedUVCamera = FixDistortion(distortedUVCamera, cameraIntrinsic, cameraDistortion);

    mat3 Sr = mat3(fixedUVCamera.s * cameraMatrix[0][2], fixedUVCamera.t * cameraMatrix[0][2], uProjector * projectorMatrix[0][2],
                   fixedUVCamera.s * cameraMatrix[1][2], fixedUVCamera.t * cameraMatrix[1][2], uProjector * projectorMatrix[1][2],
                   fixedUVCamera.s * cameraMatrix[2][2], fixedUVCamera.t * cameraMatrix[2][2], uProjector * projectorMatrix[2][2]);
    
    mat3 Sinv = inverse(Sl - Sr);

    vec3 coordinate = Sinv * vec3(fixedUVCamera.s * cameraMatrix[3][2] - cameraMatrix[3][0],
                                  fixedUVCamera.t * cameraMatrix[3][2] - cameraMatrix[3][1],
                                  uProjector * projectorMatrix[3][2] - projectorMatrix[3][0]);

	coordinateMap = vec4( coordinate, 1.0 );
}
