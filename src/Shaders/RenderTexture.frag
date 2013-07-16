#version 130

precision highp float;
precision highp sampler2D;

uniform sampler2D image;

in vec2 fragTexCoord;
out vec4 outImage;

void main(void)
{
	outImage = texture(image, fragTexCoord);
}
