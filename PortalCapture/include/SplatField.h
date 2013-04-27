/**
 @file  		SplatField.h
 @author:		Nikolaus Karpinsky


*/

#ifndef _SPLAT_FIELD_H_
#define _SPLAT_FIELD_H_

#ifdef __APPLE__
	#include <glew.h>
	#include <OpenGL/gl.h>
#elif _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <GL/glew.h>
	#include <GL/gl.h>
#else
	#include <GL/glew.h>
	#include <GL/gl.h>
#endif

#include <memory>
#include <stdio.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <wrench/gl/IBO.h>
#include <wrench/gl/VBO.h>
#include <wrench/gl/VAO.h>

using namespace wrench::gl;

class SplatField
{
private:	
    VAO m_field;
    VBO m_vertices;
    VBO m_texCoords;

    unsigned int    m_width;
    unsigned int    m_height;
		
public:
	SplatField(int width, int height);
	virtual void initMesh(void);
	
	void draw();
};

#endif // _SPLAT_FIELD_H_