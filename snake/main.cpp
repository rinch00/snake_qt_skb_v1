#include "snakegame.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SnakeGame w;
    w.setWindowTitle("Snake");
    w.resize(400, 400);
    w.show();
    return a.exec();
}
