#version 130

precision highp float;
precision highp sampler2D;

uniform sampler2D blendMap;

in vec3 fragPosition;
out vec4 rectifiedCoordMap;

void main()
{
    float blendValue = texture(blendMap, gl_PointCoord).r;

    rectifiedCoordMap = vec4(fragPosition, blendValue); 
}
