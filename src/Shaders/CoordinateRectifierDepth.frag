#version 130

precision highp float;
precision highp sampler2D;

uniform mat4 modelView;
uniform mat4 projectionMatrix;

uniform float delta;

in vec3 fragPosition;

void main()
{
    // In this pass we are just trying to determine what the depth is so that
    // we can blend nearby fragments
    gl_FragDepth = gl_FragCoord.z - delta;
}
