#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QPoint>
#include <QtGui/QOpenGLShaderProgram>
#include <QtCore/qmath.h>
#include <QKeyEvent>
#include <QMouseEvent>

#include "openglwindow.h"
#include "SuperbPlane.hpp"
#include "HeightMapper.hpp"

#ifndef TRIANGLE_WINDOW_HPP_DEFINED
#define TRIANGLE_WINDOW_HPP_DEFINED

//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
  TriangleWindow();
  void initialize() Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;
  bool event(QEvent *event) Q_DECL_OVERRIDE;
  void keyPressEvent( QKeyEvent * event );
  void mousePressEvent( QMouseEvent * event );
  void mouseReleaseEvent( QMouseEvent * event );
  void mouseMoveEvent(QMouseEvent* event);

  void initGround();
  void displayGround();

  static const char *vertexShaderSource;
  static const char *fragmentShaderSource;

private:
  GLuint loadShader(GLenum type, const char *source);

  GLuint m_posAttr;
  GLuint m_colAttr;
  GLuint m_matrixUniform;

  QOpenGLShaderProgram *m_program;
  int m_frame;
  //Camera
  QVector3D camera_position;//glut eye (position de la caméra)
  QVector3D camera_direction;//glut target (cible de la caméra)
  QVector3D camera_up;// glut center
  GLfloat camera_angle_y, camera_angle_x; //modification sur les angles phi et theta
  bool mouse_on;//Mouse boolean
  QVector3D right;

  // Surface
  DirtyPlane* d;

  //heightmap
  HeightMapper* hm;

};


#endif
