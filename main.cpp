#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    cp();
    MainWindow ChessBoard;
    ChessBoard.show();
//    endp();
    return a.exec();
}
