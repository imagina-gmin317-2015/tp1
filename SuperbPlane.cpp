#include "SuperbPlane.hpp"
#include <GL/gl.h>

SuperbPlane::SuperbPlane(int grid_width, int grid_height) : SuperbPlane(grid_width,grid_height,1.0f,1.0f)
{
}

SuperbPlane::SuperbPlane(int grid_width, int grid_height, GLfloat model_width, GLfloat model_height) {
  grid_vertices.resize(grid_height);
  grid_colors.resize(grid_height);
  this->model_width = model_width;
  this->model_height = model_height;
  GLfloat b_width = - model_width/2;
  GLfloat b_height = - model_height/2;

  GLfloat incr_width = model_width /(float) grid_width;
  GLfloat incr_height = model_height /(float) grid_height;
  for(int i=0;i<grid_height;++i)
  {
    grid_vertices[i].resize(grid_width);
    grid_colors[i].resize(grid_width);
    for(int j=0;j<grid_width;++j) {
      grid_vertices[i][j][0] = b_width + (float) i * incr_width;// X
      grid_vertices[i][j][1] = 0;// Y
      grid_vertices[i][j][2] = b_height + (float) i * incr_height;// Z
      //White
      grid_colors[i][j][0] = 1.0f;// X
      grid_colors[i][j][1] = 1.0f;// Y
      grid_colors[i][j][2] = 1.0f;// Z
    }
  }
}

int SuperbPlane::getGridWidth() {
  return grid_vertices[0].size();
}
int SuperbPlane::getGridHeight() {
  return grid_vertices.size();
}

GLfloat SuperbPlane::getModelWidth() {
  return model_width;
}
GLfloat SuperbPlane::getModelHeight() {
  return model_height;
}

QVector3D SuperbPlane::getVerticePosition(int i_width, int i_height) {
  return grid_vertices[i_height][i_width];
}
QVector3D SuperbPlane::getVerticeColor(int i_width, int i_height) {
  return grid_colors[i_height][i_width];
}

GLfloat* SuperbPlane::vectorise_vertices() {
  GLfloat* vertices = new GLfloat[3*getGridHeight()*getGridWidth()];
  long index=0;
  for(int i=0;i<getGridHeight();++i)
  {
    for(int j=0;j<getGridWidth();++j)
    {
      for(int k=0;k<3;++k)
        vertices[index++] = getVerticePosition(j,i)[k];
    }
  }
  return vertices;
}
GLfloat* SuperbPlane::vectorise_colors() {
  GLfloat* colors = new GLfloat[3*getGridHeight()*getGridWidth()];
  long index=0;
  for(int i=0;i<getGridHeight();++i)
  {
    for(int j=0;j<getGridWidth();++j)
    {
      for(int g=0;g<3;++g)
        colors[index++] = getVerticeColor(j,i)[g];
    }
  }
  return colors;
}
GLushort* SuperbPlane::vectorise_face_indexes() {
  //2 Faces à 3 Sommets indexés sur un nombre de lignes et de colonnes
  GLushort* indices = new GLushort[2*3*(getGridHeight()-1)*(getGridWidth()-1)];
  long index =0;
  for(int i = 0;i <getGridHeight() -1; ++i)
  {
    for(int j=0;j < getGridWidth() - 1;++j)
    {
      indices[index++] = GLushort(i * getGridWidth() + j);
      indices[index++] = GLushort((i+1) * getGridWidth() + j);
      indices[index++] = GLushort((i+1) * getGridWidth() + j + 1);

      indices[index++] = GLushort(i * getGridWidth() + j);
      indices[index++] = GLushort((i+1) * getGridWidth() + j + 1);
      indices[index++] = GLushort(i * getGridWidth() + j + 1);
    }
  }
  return indices;
}
