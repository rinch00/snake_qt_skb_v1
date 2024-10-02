#include "snakegame.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QPushButton>

SnakeGame::SnakeGame(QWidget *parent) : QWidget(parent), inGame(true), paused(false), snakeLength(3), score(0) {
    setStyleSheet("background-color: black;");

    resetState();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&SnakeGame::move));
    timer->start(100);
    locateFood();
}

SnakeGame::~SnakeGame()
{
    delete timer;

}

void SnakeGame::resetState() { // сброс состояния игры
    inGame = true;
    paused = false;
    score = 0;
    snakeLength = 3;
    direction = RIGHT;

    // Инициализация положения змейки
    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = 50 - i * DOT_SIZE;
        snakeY[i] = 50;
    }
    if (restartButton) {
        restartButton->hide();
        delete restartButton;   // Удалить кнопку из памяти
        restartButton = nullptr;  // Сбрасываем указатель
    }
}


 // повторная реализация игры
void SnakeGame::initGame() {
    resetState();
    locateFood();
    timer->start(100);
}


void SnakeGame::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    if (inGame) {
        painter.setBrush(Qt::red);
        painter.drawRect(foodX, foodY, DOT_SIZE, DOT_SIZE);

        for (int i = 0; i < snakeLength; i++) {
            painter.setBrush(Qt::green);
            painter.drawRect(snakeX[i], snakeY[i], DOT_SIZE, DOT_SIZE);
        }

        QFont font("Helvetica", 12, QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::white);

        // Отрисовка счёта в правом нижнем углу
        QString scoreText = "Score: " + QString::number(score);
        int textWidth = painter.fontMetrics().horizontalAdvance(scoreText);  // Ширина текста для правильного позиционирования
        int x = width() - textWidth - 10;  // Отступ от правого края
        int y = height() - 10;  // Отступ от нижнего края
        painter.drawText(x, y, scoreText);
    } else {
        QFont font("Helvetica", 15, QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Game Over\nScore: " + QString::number(score));

        if (!restartButton) {
            restartButton = new QPushButton("Restart", this);
            restartButton->setGeometry(width() / 2 - 50, height() / 2 + 30, 100, 40);  // Позиционирование кнопки по центру
            connect(restartButton, &QPushButton::clicked, this, &SnakeGame::resetGame);
            restartButton->show();
        }
    }
}


void SnakeGame::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);

    if (inGame) {
        checkCollision();
        move();
    }
}


void SnakeGame::keyPressEvent(QKeyEvent *event) {
    if (paused && event->key() == Qt::Key_Space) {
        continueGame();
        return;
    }

    int key = event->key();
    QString text = event->text();

    if ((key == Qt::Key_W ||key == Qt::Key_Up || text == "ц") && direction != DOWN) {
        direction =UP;
    } else if ((key == Qt::Key_S || key == Qt::Key_Down || text == "ы") && direction != UP) {
        direction = DOWN;
    } else if ((key == Qt::Key_A || key == Qt::Key_Left || text == "ф") && direction != RIGHT) {
        direction = LEFT;
    } else if ((key == Qt::Key_D || key == Qt::Key_Right || text == "в")&& direction != LEFT) {
        direction = RIGHT;
    } else if (key == Qt::Key_Space) {
        showPauseMenu();
    }

    QWidget::keyPressEvent(event);
}


void SnakeGame::move() {
    if (paused) return;  // Если игра на паузе, не двигаемся

    // Перемещаем тело змейки
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Перемещаем голову змейки в зависимости от направления
    switch (direction) {
    case LEFT:
        snakeX[0] -= DOT_SIZE;
        break;
    case RIGHT:
        snakeX[0] += DOT_SIZE;
        break;
    case UP:
        snakeY[0] -= DOT_SIZE;
        break;
    case DOWN:
        snakeY[0] += DOT_SIZE;
        break;
    }

    // Проверяем выход за границы поля и телепортируем змейку на противоположную сторону
    if (snakeX[0] < 0) {
        snakeX[0] = width() - DOT_SIZE;  // Если ушли за левую стенку, перемещаем на правую
    } else if (snakeX[0] >= width()) {
        snakeX[0] = 0;  // Если ушли за правую стенку, перемещаем на левую
    }

    if (snakeY[0] < 0) {
        snakeY[0] = height() - DOT_SIZE;  // Если ушли за верхнюю стенку, перемещаем вниз
    } else if (snakeY[0] >= height()) {
        snakeY[0] = 0;  // Если ушли за нижнюю стенку, перемещаем наверх
    }

    // Проверяем, съела ли змейка еду
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        score++;
        locateFood();
    }

    checkCollision();  // Проверка столкновений
    repaint();         // Перерисовка игры
}



void SnakeGame::checkCollision() {
    for (int i = snakeLength - 1; i > 0; i--) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            inGame = false;
        }
    }

    if (snakeX[0] < 0 || snakeX[0] >= width() || snakeY[0] < 0 || snakeY[0] >= height()) {
        inGame = false;
    }

    if (!inGame) {
        timer->stop();
    }
}

void SnakeGame::locateFood() {
    int maxX = (width() / DOT_SIZE) * DOT_SIZE;
    int maxY = (height() / DOT_SIZE) * DOT_SIZE;

    int randomX = QRandomGenerator::global()->bounded(0, maxX/DOT_SIZE) * DOT_SIZE;
    int randomY = QRandomGenerator::global()->bounded(0, maxY/DOT_SIZE) * DOT_SIZE;

    foodX = randomX;
    foodY = randomY;
}

void SnakeGame::showPauseMenu() {
    paused = true;
    timer->stop();

    QMessageBox pauseMenu;
    pauseMenu.setWindowTitle("Pause");
    pauseMenu.setText("Game Paused");
    QPushButton *continueButton = pauseMenu.addButton("Continue", QMessageBox::AcceptRole);
    QPushButton *resetButton = pauseMenu.addButton("Restart", QMessageBox::RejectRole);

    pauseMenu.exec();

    if (pauseMenu.clickedButton() == continueButton) {
        continueGame();
    } else if (pauseMenu.clickedButton() == resetButton) {
        resetGame();
    }
}


void SnakeGame::continueGame() {
    paused = false;
    timer->start(100);
}


void SnakeGame::resetGame() {
    if (restartButton) {
        restartButton->hide();
        delete restartButton;
        restartButton = nullptr;
    }

    initGame();
    repaint();
}
