#include "HeightMapWindow.hpp"
#include "DirtyPlane.hpp"
#include <QMouseEvent>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

const char* HeightMapWindow::vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

const char* HeightMapWindow::fragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";
HeightMapWindow::HeightMapWindow(QString filename)
: m_program(0)
, m_frame(0)
{
  this->filename = filename;
}

// Loading Shader par compilation
GLuint HeightMapWindow::loadShader(GLenum type, const char *source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, 0);
  glCompileShader(shader);
  return shader;
}

/**
* Initialisation du programme, matrices et autres éléments OpenGL
*
*/
void HeightMapWindow::initialize()
{
  m_program = new QOpenGLShaderProgram(this);
  m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  m_program->link();
  m_posAttr = m_program->attributeLocation("posAttr");
  m_colAttr = m_program->attributeLocation("colAttr");
  m_matrixUniform = m_program->uniformLocation("matrix");
  //qglClearColor(Qt::darkGray);
  //glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LEQUAL);
  glEnable(GL_LIGHTING); 	// Active l'éclairage
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_FOG) ;
  GLfloat fogcolor[4] = {0.5, 0.5, 0.5, 1} ;
  GLint fogmode = GL_EXP2 ;
  glFogi (GL_FOG_MODE, fogmode) ;
  glFogfv(GL_FOG_COLOR, fogcolor) ;
  glFogf(GL_FOG_DENSITY, 0.75) ;
  glFogf(GL_FOG_START, 0.0) ;
  glFogf(GL_FOG_END, 100.0) ;
  initGround();
}
//! [4]

bool HeightMapWindow::event(QEvent *event) {
  OpenGLWindow::event(event);
  switch(event->type()) {
    case QEvent::KeyPress : {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      //keyPressEvent(keyEvent);
      return true;
    }
    case QEvent::KeyRelease : {
      //QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

    }

    default: {
      return QWindow::event(event);
    }
    //printf("'%c unhandled event",event->type());
  }
  return QWindow::event(event);
}
void HeightMapWindow::mousePressEvent( QMouseEvent * event ) {
  if(event->type() == QEvent::MouseButtonPress){
    if(event->buttons() == Qt::LeftButton){
      setCursor(Qt::BlankCursor);
      mouse_on = true;
    }
  }
}
void HeightMapWindow::mouseReleaseEvent( QMouseEvent * event ) {
  if(event->type() == QEvent::MouseButtonRelease){
    setCursor(Qt::BitmapCursor);
    mouse_on = false;
  }
}
void HeightMapWindow::mouseMoveEvent(QMouseEvent* event) {
  float mouseSpeed = 0.005f;

  if(mouse_on && event->type() == QEvent::MouseMove){
    float xm, ym;

    xm = (float)event->x()/width()  - 0.5f;
    ym = (float)event->y()/height() - 0.5f;

    if( xm < -0.25f )
    {
      xm = 0.25f;
    }
    else if( xm > 0.25f )
    {
      xm = -0.25f;
    }

    if( ym < -0.25f )
    {
      ym = -0.25f;
    }
    else if( ym > 0.25f )
    {
      ym = 0.25f;
    }

    camera_angle_x -= mouseSpeed * float(width()/2 - event->x() );
    camera_angle_y += mouseSpeed * float( height()/2 - event->y() );

    camera_direction = QVector3D(
      cos(camera_angle_y) * sin(camera_angle_x),
      sin(camera_angle_y),
      cos(camera_angle_y) * cos(camera_angle_x)
    );

    right = QVector3D(
      sin(camera_angle_x - 3.14f/2.0f),
      0,
      cos(camera_angle_x - 3.14f/2.0f)
    );

    camera_up = QVector3D::crossProduct(right, camera_direction);

    QCursor::setPos(width()/2 + QWindow::x(), height()/2 + QWindow::y());
  }
}
void HeightMapWindow::keyPressEvent( QKeyEvent * event )
{
  float speed = 0.5f;

  //Camera movement
  if(event->key() == Qt::Key_Z)
  camera_position += camera_direction * speed;
  if(event->key() == Qt::Key_S)
  camera_position -= camera_direction * speed;
  if(event->key() == Qt::Key_Q)
  camera_position -= right * speed;
  if(event->key() == Qt::Key_D)
  camera_position += right * speed;

  //More control
  if(event->key() == Qt::Key_W) {
    static GLint polygonMode = GL_FILL;
    if ( polygonMode == GL_LINE ) {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);//
      polygonMode = GL_FILL;
  }  else if ( polygonMode == GL_FILL ) {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
      polygonMode = GL_LINE;
    }

}

}


void HeightMapWindow::initGround() {
  hm = new HeightMapper(filename);
  d = hm->getDirtyPlane(dX,dY,magni);
  //GLfloat *vertices = d.vertify();
  // int grid_size[] = { 240, 240};
  // DirtyPlane d = hm.getDirtyPlane();
  //Couleurs

  GLfloat *vertices = d.vertify();
  GLfloat *colors = new GLfloat[3*d.compute_vector_size()*2];
  int row=0;
  int k=0;
  for(long i =0 ; i <3*d.compute_vector_size();i++) {
    colors[i]=1.0f;
  }

  glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

}

void HeightMapWindow::displayGround() {
  //Only one init
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  for(int row=0;row<d.countTriangleStripRows();++row)
    glDrawArrays(GL_TRIANGLE_STRIP, row *d.countPlaneTriangleStripVertices(),
  d.countPlaneTriangleStripVertices());
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
}
//! [5]
void HeightMapWindow::render()
{
  const qreal retinaScale = devicePixelRatio();// For retina screen
  static int LightPos[4] = {1,1,3,0};
  static int MatSpec [4] = {1,1,1,1};
  static GLfloat light_ambient[4] = {0.0,0.0,0.0,1.0};
  static GLfloat light_diffuse[4] = {1.0,1.0,1.0,1.0};
  static GLfloat light_specular[4] = {1.0,1.0,1.0,0.0};

  //Define the whole application as rendering arena
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_program->bind();
  glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,50);
  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
  glLightiv(GL_LIGHT0,GL_POSITION,LightPos);


  //m_camera
  // matrice de perception
  QMatrix4x4 matrix;
  //
  matrix.perspective(60.0f, 16.0f/9.0f, 0.1f, 100.0f);
  //Same as gluLookAt
  matrix.translate(0,0,-2);
  matrix.lookAt(camera_position, camera_position+camera_direction, camera_up);
  glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
  glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
  glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.5);
  glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.5);
  //matrix.rotate(10.f*m_frame/screen()->refreshRate(),1,1,0);
  // Rotation
  //angle;x;y;z
  //matrix.rotate(angle_x,1,0,0);
  //matrix.rotate(angle_y,0,1,0);
  //matrix.rotate(angle_z,0,0,1);
  // Scaling
  //matrix.scale(2,2,2);
  m_program->setUniformValue(m_matrixUniform, matrix);
  displayGround();

  m_program->release();
  ++m_frame;
}
