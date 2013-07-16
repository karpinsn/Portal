#version 130

/*
  3x3 Median Filter

  Author: Nik Karpinsky

  Adopted from:
    A Fast, Small-Radius GPU Median Filter
    by Morgan McGuire
    from Shader X6 Advanced Rendering Techniques
*/

#define m2(a,b) 	t = a; a = min(t,b); b = max(t,b);
#define m3(a,b,c) 	m2(b,c); m2(a,c); m2(a,b);
#define m4(a,b,c,d) 	m2(a,b); m2(c,d); m2(a,c); m2(b,d);
#define m5(a,b,c,d,e) 	m2(a,b); m2(c,d); m2(a,c); m2(a,e); m2(d,e); m2(b,e);
#define m6(a,b,c,d,e,f)	m2(a,d); m2(b,e); m2(c,f); m2(a,b); m2(a,c); m2(e,f); m2(d,f);

precision highp float;
precision highp sampler2D;

uniform sampler2D image;
uniform float width;
uniform float height;

in vec2 fragTexCoord;
out vec4 filteredImage;

float step_w = 1.0/width;
float step_h = 1.0/height;

void main(void)
{
  vec4 v[5];
  vec4 t;
  
  for(int dX = -2; dX <= 2; ++dX)
  {
	vec2 offset = vec2(float(dX)*step_w, 0.0);
	v[dX + 2] = texture(image, fragTexCoord + offset);
  }

  m5(v[0], v[1], v[2], v[3], v[4])
  
  filteredImage = v[2];
}
