/**
 *  @Author David Lonni
 */

#ifndef TERRAIN_H
#define TERRAIN_H


#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QString>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QVector2D>
#include <iostream>
#include <QGuiApplication>

#include "openglwindow.h"

using namespace std;

class VertexData;

class Terrain : public OpenGLWindow
{
public:
    Terrain(QString str, QGuiApplication*);
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

    QGuiApplication* app;                       //Application

    QImage heightmap;                           //Image de heightmap

    int* hauteur;                               //Tableau des hauteurs, correspond aux pixels de l'image
    int terrain_height, terrain_width;          //Largeur et longueur du terrain

    VertexData* vertices;                       //Tableau de vertices
    GLushort* indices;                          //Tableau d'indices
    QOpenGLBuffer arrayBuf;                     //Buffer du tableau de vertices
    QOpenGLBuffer indexBuf;                     //Buffer du tableau d'indices

    QVector3D position;                         //position camera
    float direction_vue_h, direction_vue_v;     //angle vue caméra
    bool souris_active;                         //clique gauche enfoncé ou pas

    QVector3D direction;                        //vecteur direction de la caméra
    QVector3D right;                            //vecteur right perpendiculaire au vecteur direction de la caméra
    QVector3D up;                               //vecteur indiquant le haut pour la caméra

    bool wireframe;                             //Affichage fil de fer ou pas
};

#endif // TERRAIN_H
