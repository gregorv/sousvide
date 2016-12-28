#include "sousvide.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    sousvide w;
    w.show();

    return app.exec();
}

