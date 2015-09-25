#include "HeightMapper.hpp"
#include <iostream>

HeightMapper::HeightMapper(QString filename) {
  heightmap.load(filename);

}

int HeightMapper::getHeightImage() const {
  return heightmap.height();
}

int HeightMapper::getWidthImage() const {
  return heightmap.width();
}

DirtyPlane HeightMapper::getDirtyPlane(GLfloat dimX, GLfloat dimY, GLfloat magni) {
  if(heightmap.depth() == 0) {
    std::cout << "Not found heightmap" <<std::endl;
    exit(3);
  }
  GLfloat min = qRed(heightmap.pixel(0,0));
  GLfloat max = qRed(heightmap.pixel(0,0));
  for(int i=0;i<heightmap.width();++i) {
    for(int j=0;j<heightmap.height();++j) {
      if(qRed(heightmap.pixel(0,0)) < min)
        min = qRed(heightmap.pixel(0,0));
      if(qRed(heightmap.pixel(0,0))> max)
        max =qRed(heightmap.pixel(0,0));
    }
  }
  max = max-min;

  int grid_size[] = {heightmap.width(),heightmap.height()};
  DirtyPlane d(grid_size,dimX,dimY);
  for(int i=0;i<heightmap.width();++i) {
    for(int j=0;j<heightmap.height();++j) {
      d.setY(i,j,(magni/100.0) * qRed(heightmap.pixel(i,j)));
    }
  }
  return d;
}
