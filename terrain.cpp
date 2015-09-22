#include "terrain.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <math.h>

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

struct VertexData
{
    QVector3D position;
    QVector3D color;
};

Terrain::Terrain(QString heightmap) : m_program(0), m_frame(0), indexBuf(QOpenGLBuffer::IndexBuffer), position(0,0,-20), direction_vue_h(3.14f), direction_vue_v(0), wireframe(false)
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

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);							// Active le Z-Buffer
    glDepthFunc(GL_LEQUAL);

    souris_active = false;

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
    vertices = new VertexData[terrain_width * terrain_height];

    float gap = 0.5f;
    float posX = -(gap * terrain_width/2.f);
    float posY = -10.f;
    float posZ = -(gap * terrain_height/2.f);

    int index = 0;

    for(int i = 0 ; i < terrain_height ; i++){
        for(int j = 0 ; j < terrain_width ; j++){
            vertices[index].position = QVector3D(posX + gap * j, posY + hauteur[index] / 20.f, posZ + gap * i);
            if(hauteur[index] < 85){
                vertices[index].color = QVector3D(0.f,0.5f,0.f);
            }else if(hauteur[index] < 170){
                vertices[index].color = QVector3D(0.33f,0.15f,0.f);
            }else{
                vertices[index].color = QVector3D(1.f,1.f,1.f);
            }
            index++;
        }
    }

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, terrain_width * terrain_height * sizeof(VertexData));

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

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("posAttr");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int colorLocation = m_program->attributeLocation("colAttr");
    m_program->enableAttributeArray(colorLocation);
    m_program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    if(wireframe){
        glDrawElements(GL_LINES, (terrain_width-1)*(terrain_height-1)*6, GL_UNSIGNED_SHORT, 0);
    }else{
        glDrawElements(GL_TRIANGLES, (terrain_width-1)*(terrain_height-1)*6, GL_UNSIGNED_SHORT, 0);
    }


}

void Terrain::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 16.0f/9.0f, 0.1f, 200.0f);


    matrix.lookAt(position, position+direction, up);

    //if(souris_active){
        //matrix.lookAt(QVector3D(posXCam, posYCam, posZCam), QVector3D(posXCam+(float)cos(direction_vue_h), posYCam+(float)sin(direction_vue_v), posZCam+(float)sin(direction_vue_h)), QVector3D(0,1,0));
    /*}else{
        matrix.translate(posXCam, posYCam, posZCam);
        matrix.rotate(0,1,0,0);
        matrix.rotate(0,0,1,0);
        matrix.rotate(0,0,0,1);
    }*/

    m_program->setUniformValue(m_matrixUniform, matrix);

    if(heightmap.depth() != 0){
        displayTerrain();
    }

    m_program->release();

    ++m_frame;
}

void Terrain::keyPressEvent( QKeyEvent * event )
{
    float speed = 1.f;
    if(event->key() == Qt::Key_E){
        position += direction * speed;
    }if(event->key() == Qt::Key_D){
        position -= direction * speed;
    }if(event->key() == Qt::Key_S){
        position -= right * speed;
    }if(event->key() == Qt::Key_F){
        position += right * speed;
    }if(event->key() == Qt::Key_Up){
        position.setY(position.y() + speed);
    }if(event->key() == Qt::Key_Down){
        position.setY(position.y() - speed);
    }

    if(event->key() == Qt::Key_W){
        wireframe = !wireframe;
    }
}

void Terrain::mousePressEvent( QMouseEvent * event )
{
    if(event->type() == QEvent::MouseButtonPress){
        if(event->buttons() == Qt::LeftButton){
            souris_active = true;
        }
    }
}

void Terrain::mouseReleaseEvent( QMouseEvent * event )
{
    if(event->type() == QEvent::MouseButtonRelease){
        souris_active = false;
    }

}

void Terrain::mouseMoveEvent(QMouseEvent* event){

    float mouseSpeed = 0.005f;

    if(souris_active && event->type() == QEvent::MouseMove){
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

        /*direction_vue_h = xm*2.0f*3.14f;
        direction_vue_v = -ym*2.0f*3.14f;*/

        direction_vue_h += mouseSpeed * float(width()/2 - event->x() );
        direction_vue_v += mouseSpeed * float( height()/2 - event->y() );

        direction = QVector3D(
            cos(direction_vue_v) * sin(direction_vue_h),
            sin(direction_vue_v),
            cos(direction_vue_v) * cos(direction_vue_h)
        );

        right = QVector3D(
            sin(direction_vue_h - 3.14f/2.0f),
            0,
            cos(direction_vue_h - 3.14f/2.0f)
        );

        up = QVector3D::crossProduct(right, direction);

        QCursor::setPos(width()/2 + QWindow::x(), height()/2 + QWindow::y());
    }
}

