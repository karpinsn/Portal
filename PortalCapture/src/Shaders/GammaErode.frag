#version 130

uniform sampler2D image;
uniform float width;
uniform float height;

in vec2 fragTexCoord;
out vec4 erodedImage;

float step_w = 1.0/width;
float step_h = 1.0/height;

void main(void)
{
    float gamma = 1.0;

    for(int dX = -1; dX <= 1; ++dX)
    {
        for(int dY = -1; dY <= 1; ++dY)
        {
            vec2 offset = vec2(dX, dY);
            gamma = min(gamma, texture(image, fragTexCoord + vec2(float(dX) * step_w, float(dY) * step_h)).a);
        }
    }

    // Return the original phase values with the minimal gamma
    erodedImage = vec4(texture(image, fragTexCoord).xyz, gamma);
}
