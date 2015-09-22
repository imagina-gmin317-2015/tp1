#ifndef TERRAIN_H
#define TERRAIN_H


#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QString>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <iostream>

#include "openglwindow.h"

using namespace std;

class Terrain : public OpenGLWindow
{
public:
    Terrain(QString str);
    ~Terrain();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

    void keyPressEvent( QKeyEvent * event );
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void mouseMoveEvent(QMouseEvent* event);

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

    ////////////////////////////////////////////////

    QImage heightmap;

    int* hauteur;
    int terrain_height, terrain_width;

    QVector3D* vertices;
    GLushort* indices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    static const int nbCoord = 3;

    QVector3D position; //position camera
    float direction_vue_h, direction_vue_v; //direction vue caméra
    bool souris_active; //clique gauche enfoncé ou pas

    QVector3D direction;
    QVector3D right;
    QVector3D up;
};

#endif // TERRAIN_H
