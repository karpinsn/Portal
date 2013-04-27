#include "SplatField.h"

SplatField::SplatField(int width, int height) : m_width(width), m_height(height)
{ }

void SplatField::initMesh(void)
{
  auto verticies = unique_ptr<glm::vec3[]>( new glm::vec3[ m_width * m_height ] );
  auto texCoords = unique_ptr<glm::vec2[]>( new glm::vec2[ m_width * m_height ] );

  for(int row = 0; row < m_height; ++row)
  {
      for(int col = 0; col < m_width; ++col)
      {
          verticies[row * m_width + col].x = float(col) / float(m_width);
          verticies[row * m_width + col].y = float(row) / float(m_height);
          verticies[row * m_width + col].z = 0.0f;
          
          texCoords[row * m_width + col].s = float(col) / float(m_width);
          texCoords[row * m_width + col].t = float(row) / float(m_height);          
      }
  }

  m_field.init(GL_POINTS, m_width * m_height);
  m_vertices.init(3, GL_FLOAT, GL_ARRAY_BUFFER);
  m_vertices.bufferData(m_width * m_height, glm::value_ptr(verticies[0]), GL_STATIC_DRAW);
  m_field.addVBO(m_vertices, "vert");

  m_texCoords.init(2, GL_FLOAT, GL_ARRAY_BUFFER);
  m_texCoords.bufferData(m_width * m_height, glm::value_ptr(texCoords[0]), GL_STATIC_DRAW);
  m_field.addVBO(m_texCoords, "vertTexCoord"); 
}

void SplatField::draw()
{
  // Enable point sprites and their ability to replace texture coordinates
  glEnable(GL_POINT_SPRITE);
  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
 
  // Disable depth and enable blend so that we can get our splat rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE);


  m_field.draw();

  // Put depth back on
  glDepthMask(GL_TRUE);
  glDisable(GL_POINT_SPRITE);
}
