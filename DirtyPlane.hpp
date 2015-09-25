#ifndef DIRTY_PLANE_HPP_DEFINED
#define DIRTY_PLANE_HPP_DEFINED
#include <QVector>
#include <QVector3D>
#include <QtGui/QOpenGLFunctions>

/**
* Il s'agit d'une implémentation rapide sans index, prenant de la place mémoire
*
**/
class DirtyPlane {
private:
  /// Grille de points générés
  QVector<QVector<QVector3D> > grid;
public:
  ///Construction en fonction du nombre de points voulus en x:z et attribution
  /// des valeurs entre -width/2 et width/2 ET -height/2 à height/2
  DirtyPlane() {
    
  }
  DirtyPlane(int grid_size[2], GLfloat width, GLfloat height);
  virtual ~DirtyPlane();
  //Modification de la hauteur au sommet (i,j)
  void setY(int i, int j, GLfloat y);
  //Retourne la hauteur en un point
  GLfloat getY(int i, int j);
  ///Retourne la largeur de la grille
  long getGridWidth();
  ///Retourne la haute de la grille
  long getGridHeight();

  //Retourne la taille du vecteur de points actuel
  long compute_vector_size();
  //Retourne un pointeur sur un vecteur de vertex [x1,y1,z1,x2,y2,z2]
  GLfloat* vertify();
  //Retourne le nombre de TriangleStrip à exécuter pour faire toute la surface
  long countTriangleStripRows();
  //Retiyrbe ke nombre de sommets à parcourir pour chaque TriangleStrip
  long countPlaneTriangleStripVertices();
};
#endif
