#include <QtGui/QScreen>

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QPoint>
#include <QtGui/QOpenGLShaderProgram>
#include <QtCore/qmath.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <GL/glu.h>

#ifndef TRIANGLE_WINDOW_HPP_DEFINED
#define TRIANGLE_WINDOW_HPP_DEFINED

#include "openglwindow.h"

static const char *vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

static const char *fragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";
//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
  TriangleWindow();
  void initialize() Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;
  bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
  GLuint loadShader(GLenum type, const char *source);

  GLuint m_posAttr;
  GLuint m_colAttr;
  GLuint m_matrixUniform;

  QOpenGLShaderProgram *m_program;
  int m_frame;
};

TriangleWindow::TriangleWindow()
: m_program(0)
, m_frame(0)
{
}

//! [4]
GLuint TriangleWindow::loadShader(GLenum type, const char *source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, 0);
  glCompileShader(shader);
  return shader;
}

void TriangleWindow::initialize()
{
  m_program = new QOpenGLShaderProgram(this);
  m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  m_program->link();
  m_posAttr = m_program->attributeLocation("posAttr");
  m_colAttr = m_program->attributeLocation("colAttr");
  m_matrixUniform = m_program->uniformLocation("matrix");
  //qglClearColor(Qt::darkGray);
  glEnable(GL_DEPTH_TEST);
}
//! [4]

bool TriangleWindow::event(QEvent *event) {
  OpenGLWindow::event(event);
  switch(event->type()) {
    case QEvent::KeyPress : {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

      if( keyEvent->key() == Qt::Key_Z )
      printf("forward");
      else if (keyEvent->key() == Qt::Key_S)
      printf("backward");
      else if (keyEvent->key() == Qt::Key_Q)
      printf("left");
      else if (keyEvent->key() == Qt::Key_D)
      printf("right");
      else
      printf("'%c' not handled",keyEvent->key());
      return true;
    }
    case QEvent::KeyRelease : {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      printf("'%c' free",keyEvent->key());
    }
    break;
    default: {
      return QWindow::event(event);
    }
    //printf("'%c unhandled event",event->type());
  }
  return QWindow::event(event);
}
//! [5]
void TriangleWindow::render()
{
  const qreal retinaScale = devicePixelRatio();// For retina screen

  //Define the whole application as rendering arena
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT);

  m_program->bind();

  // matrice de perception
  QMatrix4x4 matrix;
  //
  matrix.perspective(60.0f, 16.0f/9.0f, 0.1f, 100.0f);
  // Translation
  matrix.translate(0, 0, -2);
  //matrix.translate(0,0, 1.f * m_frame / screen()->refreshRate());
  // Rotation
  matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0.05f, 1, 0);
  // Scaling
  //matrix.scale(1,1,1);

  m_program->setUniformValue(m_matrixUniform, matrix);
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  //
  GLfloat base_value[] = {
    -0.5f, -0.5f,0.0f
  };

  GLfloat incr_value[] = {
    0.04f,0.04f,0.04f
  };

  int grid_size[] = { 16, 16};
  int dimension = 3;

  //Création des points
  QVector<QVector<QVector<GLfloat> > > grid(grid_size[0]);
  for(int i=0; i<grid_size[0] ;++i) {
    grid[i].resize(grid_size[1]);
    for(int j=0; j<grid_size[1] ;++j) {
      grid[i][j].resize(dimension);
      grid[i][j][0] = base_value[0]+ (float) i * incr_value[0];
      if(dimension == 3)
        grid[i][j][1] = base_value[1];
      else
        grid[i][j][1] = base_value[1]+ (float) j * incr_value[1];

      if(dimension == 3)
        grid[i][j][2] = base_value[1]+ (float) j * incr_value[1];
      //grid[i][j][2] = 0;
      // std::cout <<"engine:p["<<i<<","<<j<<"]:"<< grid[i][j][0] <<":"<<grid[i][j][1]<<std::endl;
    }
  }


  //Création d'un vecteur de 2 valeurs (x,y) par le nombre de points
  long vector_size = dimension * grid_size[0] * grid_size[1] + dimension * (grid_size[0]-2) * grid_size[1];
  GLfloat *vertices = new GLfloat[vector_size];

  //std::cout << "engine:VECTOR_SIZE:"<<vector_size<<std::endl;

  long num_point = 0;//nombre de points total

  for(int row=0;row< grid_size[0]-1;++row) {
    //Récupération des deux colonnes pour le triangle stip
    // std::cout << "engine:row:"<<row<<"/"<<grid.size()<<std::endl;
    // std::cout << "engine:row:"<<row<<":content:r0:"<<grid[row].size()<<std::endl;
    // std::cout << "engine:row:"<<row<<":content:r1:"<<grid[row+1].size()<<std::endl;

    QVector<QVector<GLfloat> > r0 = grid[row];
    QVector<QVector<GLfloat> > r1 = grid[row+1];

    for(int k = 0; k < r0.size();++k ) {
      // std::cout << "engine:row:"<<row<<":element:"<<k<<"/"<<r0.size()<<std::endl;
      vertices[num_point++] = r0[k][0];
      // std::cout << "engine:row:"<<row<<":element:r0:"<<k<<"/"<<r0.size()<<"x"<<std::endl;
      vertices[num_point++] = r0[k][1];
      if(dimension==3)
      vertices[num_point++] = r0[k][2];
      // std::cout << "engine:row:"<<row<<":element:r0:"<<k<<"/"<<r0.size()<<"y"<<std::endl;

      vertices[num_point++] = r1[k][0];
      // std::cout << "engine:row:"<<row<<":element:r1:"<<k<<"/"<<r1.size()<<"x"<<std::endl;
      vertices[num_point++] = r0[k][1];
      if(dimension==3)
        vertices[num_point++] = r1[k][2];
      // std::cout << "engine:row:"<<row<<":element:r1:"<<k<<"/"<<r1.size()<<"y"<<std::endl;
    }

  }
  //std::cout <<"engine:USED_SPACE:"<< num_point<<std::endl;

  //Couleurs
  GLfloat *colors = new GLfloat[3*num_point];
  for(int c=0;c<3*num_point;++c) {
    colors[c] = 1.0f;
  }

  glVertexAttribPointer(m_posAttr, dimension, GL_FLOAT, GL_FALSE, 0, vertices);
  // glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  //Draw les 3 premiers points
  //glDrawArrays(GL_TRIANGLES,0,3);

  // glDrawArrays(GL_TRIANGLE_STRIP, 2,num_point/2);
  for(int row=0;row<grid_size[0]-1;++row)
  glDrawArrays(GL_TRIANGLE_STRIP, row *grid_size[1]*dimension,dimension*grid_size[1]);
  // glDrawArrays(GL_TRIANGLE_STRIP,0,grid_size[1]*2);
  //glDrawArrays(GL_TRIANGLE_STRIP,grid_size[1]*2,2*grid_size[1]);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  m_program->release();

  //delete[] vertices;
  //delete[] colors;
  ++m_frame;
}

#endif
