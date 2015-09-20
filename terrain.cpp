#include "terrain.h"

#include <QKeyEvent>

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

Terrain::Terrain(QString heightmap) : m_program(0), m_frame(0), indexBuf(QOpenGLBuffer::IndexBuffer), posXCam(0), posYCam(0), posZCam(-20)
{
    openImage(heightmap);
}

Terrain::~Terrain(){
    delete hauteur;
    delete vertices;
    delete indices;

    arrayBuf.destroy();
    indexBuf.destroy();
}

GLuint Terrain::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void Terrain::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    arrayBuf.create();
    indexBuf.create();

    createTerrain();
}

void Terrain::openImage(QString str){
    heightmap.load(str);

    if(heightmap.depth() != 0){

        terrain_width = heightmap.width();
        terrain_height = heightmap.height();

        hauteur = new int[terrain_width * terrain_height];

        int index = 0;
        for(int i = 0  ; i < terrain_height ; i++){
            for(int j = 0 ; j < terrain_width ; j++){
                QRgb pixel = heightmap.pixel(i,j);
                hauteur[index++] = qRed(pixel);
            }
        }
    }
}

void Terrain::createTerrain(){

    if(heightmap.depth() == 0)
        return;

    //VERTICES /////////////////////////////////////////////////////////////////
    vertices = new QVector3D[terrain_width * terrain_height];

    float gap = 0.5f;
    float posX = -(gap * terrain_width/2.f);
    float posY = -10.f;
    float posZ = -(gap * terrain_height/2.f);

    int index = 0;
    int ind = 0;

    for(int i = 0 ; i < terrain_height ; i++){
        for(int j = 0 ; j < terrain_width ; j++){
            vertices[index++] = QVector3D(posX + gap * j, posY + hauteur[ind++] / 20.f, posZ + gap * i);
        }
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, terrain_width * terrain_height * sizeof(QVector3D));

    //INDICES /////////////////////////////////////////////////////////////////
    indices = new GLushort[(terrain_width-1)*(terrain_height-1)*6];

    index = 0;

    for(int i = 0 ; i < terrain_height-1 ; i++){
        for(int j = 0 ; j < terrain_width-1 ; j++){

            indices[index++] = GLushort(i * terrain_width + j);
            indices[index++] = GLushort((i+1) * terrain_width + j);
            indices[index++] = GLushort((i+1) * terrain_width + j + 1);

            indices[index++] = GLushort(i * terrain_width + j);
            indices[index++] = GLushort((i+1) * terrain_width + j + 1);
            indices[index++] = GLushort(i * terrain_width + j + 1);
        }
    }

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, (terrain_width-1)*(terrain_height-1)*6 * sizeof(GLushort));
}

void Terrain::displayTerrain(){

    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("posAttr");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, (terrain_width-1)*(terrain_height-1)*6, GL_UNSIGNED_SHORT, 0);

}

void Terrain::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 16.0f/9.0f, 0.1f, 200.0f);
    matrix.translate(posXCam, posYCam, posZCam);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    if(heightmap.depth() != 0){
        displayTerrain();
    }

    m_program->release();

    ++m_frame;
}

void Terrain::keyPressEvent( QKeyEvent * event )
{
    float inc = 0.5f;
    if(event->key() == Qt::Key_E){
        posZCam += inc;
    }else if(event->key() == Qt::Key_D){
        posZCam -= inc;
    }else if(event->key() == Qt::Key_S){
        posXCam += inc;
    }else if(event->key() == Qt::Key_F){
        posXCam -= inc;
    }else if(event->key() == Qt::Key_Up){
        posYCam -= inc;
    }else if(event->key() == Qt::Key_Down){
        posYCam += inc;
    }
}

