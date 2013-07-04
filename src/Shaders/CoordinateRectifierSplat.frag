#version 130

precision highp float;
precision highp sampler2D;

uniform mat4 modelView;
uniform mat4 projectionMatrix;

uniform int width;
uniform int height;

uniform sampler2D blendMap;
uniform sampler2D depthMap;

in vec3 fragPosition;

out vec4 rectifiedCoordMap;

void main()
{
    // We use gl_FragCoord.xy since this texture is screen aligned and we want to
    // know for this screen pixel's depth value whether or not we should discard
    // Thus, the UV coords are just the xy / width,height
    vec2 screenUV = gl_FragCoord.xy / vec2( float(width), float(height) );

    // If this fragments position is less than the depth position then discard.
    if( texture( depthMap, screenUV ).r > gl_FragCoord.z )
        { discard; }

    // This is where the actual blending happends. Render out our coord map
    // and then select the alpha based on the pointCoord position in the sprite
    rectifiedCoordMap = vec4( fragPosition, texture( blendMap, gl_PointCoord ).r ); 
}
