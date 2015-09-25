#include "DirtyPlane.hpp"
#include <iostream>

DirtyPlane::DirtyPlane(int grid_size[2], GLfloat width, GLfloat height) {
  //Définition de la taille d'une cellule
  GLfloat incr_value[] = {height /(float) grid_size[0], width /(float) grid_size[1]};
  //Centrage de la surface en 0
  GLfloat base_value[] = {- height / 2.0f, - width / 2.0f};

  //Création de la grille
  grid.resize(grid_size[0]);
  for(int i=0; i<grid_size[0] ;++i) {
    grid[i].resize(grid_size[1]);
    for(int j=0; j<grid_size[1] ;++j) {
      grid[i][j][0] = base_value[0] + (float) i * incr_value[0];
      grid[i][j][1] = 0;//Par défaut 0
      grid[i][j][2] = base_value[1]+ (float) j * incr_value[1];
    }
  }
}

DirtyPlane::~DirtyPlane() {}

void DirtyPlane::setY(int i, int j, GLfloat y) {
    grid[i][j][1] = y;
}

GLfloat DirtyPlane::getY(int i, int j) {
  return grid[i][j][1];
}

long DirtyPlane::compute_vector_size() {
  long vector_size = 3 * grid.size() * grid[0].size() + 3 * (grid.size()-2) * grid[0].size();
  return vector_size;
}

GLfloat* DirtyPlane::vertify() {
  GLfloat* vertices = new GLfloat[compute_vector_size()];
  long num_point = 0;
  for(int row=0;row< grid.size()-1;++row) {

    QVector<QVector3D> r0 = grid[row];
    QVector<QVector3D> r1 = grid[row+1];

    for(int k = 0; k < r0.size();++k ) {
      vertices[num_point++] = r0[k][0];
      vertices[num_point++] = r0[k][1];
      vertices[num_point++] = r0[k][2];

      vertices[num_point++] = r1[k][0];
      vertices[num_point++] = r1[k][1];
      vertices[num_point++] = r1[k][2];
    }

  }
  return vertices;
}
long DirtyPlane::countTriangleStripRows() {
  return grid.size()-1;
}
long DirtyPlane::countPlaneTriangleStripVertices() {
  return  grid[0].size()*2;
}

///Retourne la largeur de la grille
long DirtyPlane::getGridWidth() {
  return grid[0].size();
}
///Retourne la haute de la grille
long DirtyPlane::getGridHeight() {
  return grid.size();
}
