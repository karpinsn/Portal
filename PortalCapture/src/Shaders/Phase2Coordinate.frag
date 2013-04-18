#version 150

// We are doing inverses and need high precision so we dont incur error during triangulation
precision highp float;
precision highp sampler2D;

uniform sampler2D actualPhase;

uniform int fringePitch;
uniform float Phi0;
uniform int cameraWidth;
uniform int cameraHeight;

// Matrix is actually 3x4. OpenGL is column major and mathmatically backwards
uniform mat4x3 cameraMatrix;
uniform mat4x3 projectorMatrix;

in vec2 fragTexCoord;

out vec4 coordinateMap;

void main()
{
    float pi = 3.14159;
	float Phi = texture(actualPhase, fragTexCoord).r; 
    float uProjector = 1.0 + ( ( Phi - Phi0 ) / ( ( 2.0 * pi ) / float( fringePitch) ) );
   
    mat3 Sl = mat3(cameraMatrix[0][0], cameraMatrix[0][1], projectorMatrix[0][0],
                   cameraMatrix[1][0], cameraMatrix[1][1], projectorMatrix[1][0],
                   cameraMatrix[2][0], cameraMatrix[2][1], projectorMatrix[2][0]);
    
    // PROFILE - Check if this is faster than the method above for building this matrix
    //mat3 Sl = transpose(mat3(transpose(cameraMatrix)[0], transpose(cameraMatrix[1]), transpose(projectorMatrix[0])));

    vec2 uvCamera = fragTexCoord * vec2(cameraWidth, cameraHeight);
 
    mat3 Sr = mat3(uvCamera.s * cameraMatrix[0][2], uvCamera.t * cameraMatrix[0][2], uProjector * projectorMatrix[0][2],
                   uvCamera.s * cameraMatrix[1][2], uvCamera.t * cameraMatrix[1][2], uProjector * projectorMatrix[1][2],
                   uvCamera.s * cameraMatrix[2][2], uvCamera.t * cameraMatrix[2][2], uProjector * projectorMatrix[2][2]);
    
    mat3 Sinv = inverse(Sl - Sr);

    vec3 coordinate = Sinv * vec3(uvCamera.s * cameraMatrix[3][2] - cameraMatrix[3][0],
                                  uvCamera.t * cameraMatrix[3][2] - cameraMatrix[3][1],
                                  uProjector * projectorMatrix[3][2] - projectorMatrix[3][0]);

	coordinateMap = vec4( coordinate, 1.0 );
}
