#include "openglwindow.h"
#include "HeightMapWindow.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);
    if(argc<2) {
        printf("%s path",argv[0]);
        return -1;
    }

    QString test(argv[1]);
    HeightMapWindow window(test);

    if(argc==4) {
        window.setDimX(atof(argv[2]));
        window.setDimY(atof(argv[3]));

    }
    if(argc==5) {
        window.setMagnitude((atof(argv[4])));
    }




    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
