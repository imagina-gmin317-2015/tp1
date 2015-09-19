#ifndef TERRAIN_H
#define TERRAIN_H


#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QString>
#include <iostream>

#include "openglwindow.h"

using namespace std;

class Terrain : public OpenGLWindow
{
public:
    Terrain(QString str);

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

private:
    void openImage(QString str);
    void createTerrain();
    void displayTerrain();

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;

    QImage heightmap;

    int* hauteur;

    GLfloat** vertices;

    static const int nbCoord = 3;
};

#endif // TERRAIN_H
