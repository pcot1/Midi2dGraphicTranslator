#include <stdio.h>
#include "gui.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    setvbuf(stdout, (char*)NULL, _IONBF, 0);            // just for debug printf to console
    //char blabla[128];
    QApplication app(argc, argv);
    Gui guiWindow;
    guiWindow.show();
    return app.exec();
}


