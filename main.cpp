/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "openglwindow.h"

#include <iostream>

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QPoint>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

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
//! [1]

//! [2]
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    TriangleWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
//! [2]


//! [3]
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
//! [3]

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
}
//! [4]

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
    // Rotation
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
    // Scaling
    //matrix.scale(1,1,1);

    m_program->setUniformValue(m_matrixUniform, matrix);
glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //
    GLfloat base_value[] = {
        -0.5f, -0.5f
    };

    GLfloat incr_value[] = {
      0.1f,0.1f
    };

    int grid_size[] = { 2, 5};

    //Création des points
    QVector<QVector<QVector<GLfloat> > > grid(grid_size[0]);
    for(int i=0; i<grid_size[0] ;++i) {
      grid[i].resize(grid_size[1]);
      for(int j=0; j<grid_size[1] ;++j) {
          grid[i][j].resize(2);
          grid[i][j][0] = base_value[0]+ (float) i * incr_value[0];
          grid[i][j][1] = base_value[1]+ (float) j * incr_value[1];
          // std::cout <<"engine:p["<<i<<","<<j<<"]:"<< grid[i][j][0] <<":"<<grid[i][j][1]<<std::endl;
      }
    }


    //Création d'un vecteur de 2 valeurs (x,y) par le nombre de points
    long vector_size = 2 * grid_size[0] * grid_size[1];
    GLfloat *vertices = new GLfloat[vector_size];

    std::cout << "engine:VECTOR_SIZE:"<<vector_size<<std::endl;

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
          // std::cout << "engine:row:"<<row<<":element:r0:"<<k<<"/"<<r0.size()<<"y"<<std::endl;

          vertices[num_point++] = r1[k][0];
          // std::cout << "engine:row:"<<row<<":element:r1:"<<k<<"/"<<r1.size()<<"x"<<std::endl;

          vertices[num_point++] = r1[k][1];
          // std::cout << "engine:row:"<<row<<":element:r1:"<<k<<"/"<<r1.size()<<"y"<<std::endl;
        }

    }
    std::cout <<"engine:USED_SPACE:"<< num_point<<std::endl;

    //Couleurs
    GLfloat *colors = new GLfloat[3*num_point];
    for(int c=0;c<3*num_point;++c) {
      colors[c] = 1.0f;

    }

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    // glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //Draw les 3 premiers points
    //glDrawArrays(GL_TRIANGLES,0,3);

      // glDrawArrays(GL_TRIANGLE_STRIP, 2,num_point/2);
      glDrawArrays(GL_TRIANGLE_STRIP,0,num_point/2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    //delete[] vertices;
    //delete[] colors;
    ++m_frame;
}
//! [5]
