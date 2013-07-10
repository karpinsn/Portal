#include "SplatField.h"

SplatField::SplatField(int width, int height) 
{ 
  auto verticies = unique_ptr<glm::vec3[]>( new glm::vec3[ width * height ] );
  auto texCoords = unique_ptr<glm::vec2[]>( new glm::vec2[ width * height ] );

  for(int row = 0; row < height; ++row)
  {
      for(int col = 0; col < width; ++col)
      {
          verticies[row * width + col].x = float(col) / float(width);
          verticies[row * width + col].y = float(row) / float(height);
          verticies[row * width + col].z = 0.0f;
          
          texCoords[row * width + col].s = float(col) / float(width);
          texCoords[row * width + col].t = float(row) / float(height);          
      }
  }

  m_field.init(GL_POINTS, width * height);
  m_vertices.init(3, GL_FLOAT, GL_ARRAY_BUFFER);
  m_vertices.bufferData(width * height, glm::value_ptr(verticies[0]), GL_STATIC_DRAW);
  m_field.addVBO(m_vertices, "vert");

  m_texCoords.init(2, GL_FLOAT, GL_ARRAY_BUFFER);
  m_texCoords.bufferData(width * height, glm::value_ptr(texCoords[0]), GL_STATIC_DRAW);
  m_field.addVBO(m_texCoords, "vertTexCoord"); 
}

void SplatField::draw()
{
  glEnable(GL_POINT_SPRITE); // Enable point sprites and their ability to replace texture coordinates
  glEnable(GL_BLEND);
  {
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_field.draw();
  } // Put our stuff back
  glDisable(GL_BLEND);
  glDisable(GL_POINT_SPRITE);
}
