#include "SuperbHeightMapWindow.hpp"
#include "SuperbPlane.hpp"
#include <QMouseEvent>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

const char* TriangleWindow::vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

const char* TriangleWindow::fragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";
TriangleWindow::TriangleWindow()
: m_program(0)
, m_frame(0)
{
}

// Loading Shader par compilation
GLuint TriangleWindow::loadShader(GLenum type, const char *source)
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
  //glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LEQUAL);

  initGround();
}
//! [4]

bool TriangleWindow::event(QEvent *event) {
  OpenGLWindow::event(event);
  switch(event->type()) {
    case QEvent::KeyPress : {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      keyPressEvent(keyEvent);
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
void TriangleWindow::mousePressEvent( QMouseEvent * event ) {
  if(event->type() == QEvent::MouseButtonPress){
    if(event->buttons() == Qt::LeftButton){
      setCursor(Qt::BlankCursor);
      mouse_on = true;
    }
  }
}
void TriangleWindow::mouseReleaseEvent( QMouseEvent * event ) {
  if(event->type() == QEvent::MouseButtonRelease){
    setCursor(Qt::BitmapCursor);
    mouse_on = false;
  }
}
void TriangleWindow::mouseMoveEvent(QMouseEvent* event) {
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

    camera_angle_x += mouseSpeed * float(width()/2 - event->x() );
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
void TriangleWindow::keyPressEvent( QKeyEvent * event )
{
  float speed = 0.1f;

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
  if(event->key() == Qt::Key_W)
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
  if(event->key() == Qt::Key_X)
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}


void TriangleWindow::initGround() {
  hm = new HeightMapper("./heightmap-1.png");
  DirtyPlane d = hm->getDirtyPlane();
  //GLfloat *vertices = d.vertify();
  // int grid_size[] = { 240, 240};
  // DirtyPlane d = hm.getDirtyPlane();
  //Couleurs
  GLfloat *vertices = d.vertify();
  GLfloat *colors = new GLfloat[d.compute_vector_size()*2];
  int row=0;
  int k=0;
  for(long i =0 ; i <d.compute_vector_size();i++) {
    if(row*k==d.getGridWidth()*d.getGridHeight())
      break;
    //Seulement toutes les trois valeurs (parcours de x,y,z)
    if(i%3==0) {
      //Seulement pour la première colonne (cas spécial dans un DirtyPlane)
      if(row==0) {
        if(k!=0 && k % d.getGridWidth()==0) {
          row++;
          k=0;
        }
      }else {
        //Copie sur deux fois la même couleur (cas dans un DirtyPlane)
        if(k!=0 && k % (2*d.getGridWidth()) == 0)
        {
          row++;
          k=0;
        }
      }
      std::cout << "rowR:"<<row<<"k:"<<k<<std::endl;
      colors[i]=d.getY(row,k);//R
      std::cout << "rowG:"<<row<<"k:"<<k<<std::endl;
      colors[i+1]=d.getY(row,k);//G
      std::cout << "rowB:"<<row<<"k:"<<k<<std::endl;
      colors[i+2]=d.getY(row,k);//B
      std::cout << "rowFi:"<<row<<"k:"<<k<<std::endl;
      k++;
    }
  }
  std::cout << "here" <<std::endl;
  glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);
}

void TriangleWindow::displayGround() {
  //Only one init
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  for(int row=0;row<d->countTriangleStripRows();++row)
  glDrawArrays(GL_TRIANGLE_STRIP, row *d->countPlaneTriangleStripVertices(),
  d->countPlaneTriangleStripVertices());
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
}
//! [5]
void TriangleWindow::render()
{
  const qreal retinaScale = devicePixelRatio();// For retina screen

  //Define the whole application as rendering arena
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program->bind();

  //m_camera
  // matrice de perception
  QMatrix4x4 matrix;
  //
  matrix.perspective(60.0f, 16.0f/9.0f, 0.1f, 100.0f);
  //Same as gluLookAt
  matrix.translate(0,0,-2);
  matrix.lookAt(camera_position, camera_position+camera_direction, camera_up);
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
