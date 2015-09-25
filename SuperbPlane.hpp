#ifndef SUPERBPLANE_HPP_DEFINED
#define SUPERBPLANE_HPP_DEFINED

#include <QVector>
#include <QVector3D>
#include <QtGui/QOpenGLFunctions>

class SuperbPlane {
protected:
  QVector<QVector<QVector3D> > grid_vertices;
  QVector<QVector<QVector3D> > grid_colors;
  GLfloat model_width;
  GLfloat model_height;

public:
  SuperbPlane(int grid_width, int grid_height);
  SuperbPlane(int grid_width, int grid_height, GLfloat model_width, GLfloat model_height);

  int getGridWidth();
  int getGridHeight();

  GLfloat getModelWidth();
  GLfloat getModelHeight();

  QVector3D getVerticePosition(int i_width, int i_height);
  QVector3D getVerticeColor(int i_width, int i_height);

  GLfloat* vectorise_vertices();
  GLfloat* vectorise_colors();
  GLushort* vectorise_face_indexes();

};
#endif
