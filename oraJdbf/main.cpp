#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QApplication::addLibraryPath(QLatin1String("./"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}