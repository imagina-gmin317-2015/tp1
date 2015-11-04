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

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

#include <iostream>
using namespace std;





//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void drawTerrain();
    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;
    void createBuffer();

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    int m_frame;
};

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
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

    window.setAnimating(false);

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








   /*
    arrayBuf.create();
    indexBuf.create();
   VertexData vertices [] =
           {{QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)}} ; // v2


    GLushort indexes[] = {
        0,1,2
    };

    arrayBuf.bind();
    arrayBuf.allocate((vertices, 3*sizeof(VertexData)));

    indexBuf.bind();
    indexBuf.allocate((indexes, 3*sizeof(GLushort)));


     //arrayBuf.allocate((vertices, 16*16*3*sizeof(float) ));
        //indexBuf.allocate((indexes, (16-1)*(16-1)*2*3*sizeof(unsigned int) ));*/

}
//! [4]



void TriangleWindow::drawTerrain()
{



    GLfloat vertices[16*16];

    int cptPoint = 0;
    for(int i = 0 ; i<16 ; i++)
    {
        for(int j = 0 ; j<16 ; j++)
        {
            //vertices[cptPoint++] = QVector3D((float)i/20.0f,(float)j/20.0f,0.0f);
            vertices[cptPoint++] = (float)i;
            vertices[cptPoint++] = (float)j;
            vertices[cptPoint++] = 0.0f;

        }
    }


    unsigned int indexes[(16-1)*(16-1)*2*3];
    int cptIndex = 0;
    for(int i = 0 ; i<16-1 ; i++)
    {
        for(int j = 0 ; j<16-1 ; j++)
        {
            indexes[cptIndex++] = i*16 + j;
            indexes[cptIndex++] = (i+1) * 16 + j;
            indexes[cptIndex++] = (i+1) * 16 + j+1;

            cout<<"Triangle "<<j+(i*16)<<" ; "<<j+(i*16) +1<<" ; "<<j+1+(i+1)*16<<endl;

            indexes[cptIndex++] = i*16 + j;
            indexes[cptIndex++] = (i+1) * 16 + j+1;
            indexes[cptIndex++] = i*16 + j+1;

            cout<<"Triangle "<<j+(i*16)<<" ; "<<j+(i+1)*16<<" ; "<<j+(i*16) +1<<endl;

        }
    }

    float triangles[(16-1)*(16-1)*2*3*3];
    cptIndex = 0;
    int cptTriangles = 0;
    for(int i = 0; i<=(16-1)*(16-1)*2 ; i+=3)
    {
        triangles[cptTriangles++] = vertices[indexes[i]*3];
        triangles[cptTriangles++] = vertices[indexes[i]*3 + 1];
        triangles[cptTriangles++] = vertices[indexes[i]*3 + 2];

        triangles[cptTriangles++] = vertices[indexes[i+1]*3];
        triangles[cptTriangles++] = vertices[indexes[i+1]*3 + 1];
        triangles[cptTriangles++] = vertices[indexes[i+1]*3 + 2];

        triangles[cptTriangles++] = vertices[indexes[i+2]*3];
        triangles[cptTriangles++] = vertices[indexes[i+2]*3 + 1];
        triangles[cptTriangles++] = vertices[indexes[i+2]*3 + 2];


        cout<< vertices[indexes[i]*3] << " ; " << vertices[indexes[i]*3 + 1] << " ; "<< vertices[indexes[i]*3 + 2]<< endl;
        cout<< vertices[indexes[i+1]*3] << " ; " << vertices[indexes[i+1]*3 + 1] << " ; "<< vertices[indexes[i+1]*3 + 2]<< endl;
        cout<< vertices[indexes[i+2]*3] << " ; " << vertices[indexes[i+2]*3 + 1] << " ; "<< vertices[indexes[i+2]*3 + 2]<< endl;
        cout<< i<<endl;


    }




    cout<<"ok"<<endl;

    GLfloat colors[] = {
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f
    };


    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, triangles);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, (16-1)*(16-1)*2*3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    /*arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    m_program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    cout<<"Drawing..."<<endl;

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);*/

}


//! [5]
void TriangleWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);


    drawTerrain();



}






