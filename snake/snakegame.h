#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QWidget>
#include <QtWidgets/qpushbutton.h>

class QTimer;
class QKeyEvent;

class SnakeGame : public QWidget
{
    Q_OBJECT

public:
    SnakeGame(QWidget *parent = nullptr);
    ~SnakeGame();

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPushButton *restartButton = nullptr;

    enum Direction {LEFT, RIGHT, UP, DOWN};
    static const int BORD_WIDTH = 10;
    static const int BORD_HEIGHT = 10;
    static const int DOT_SIZE = 10;
    static const int ALL_DOTS = 100;

    QTimer *timer;
    Direction direction;

    bool inGame;
    bool paused;

    int snakeLength;
    int score;
    int snakeX[ALL_DOTS];
    int snakeY[ALL_DOTS];
    int foodX;
    int foodY;

    void initGame();
    void locateFood();
    void checkCollision();
    void move();
    void doDrawing();
    void showPauseMenu();
    void restartGame();
    void resetState();

private slots:
    void continueGame();
    void resetGame();
};
#endif // SNAKEGAME_H
