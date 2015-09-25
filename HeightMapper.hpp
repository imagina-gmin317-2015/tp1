#ifndef HEIGHTMAPPER_HPP_DEFINED
#define HEIGHTMAPPER_HPP_DEFINED
#include <QImage>
#include <QVector>
#include <QVector3D>
#include "DirtyPlane.hpp"

class HeightMapper {

private:
  QImage heightmap;

public:
  HeightMapper(QString name);
  int getWidthImage() const;
  int getHeightImage() const;
  float getDepthImage(int x, int z);
  DirtyPlane getDirtyPlane(GLfloat dimX, GLfloat dimY, GLfloat magni);
};
#endif
