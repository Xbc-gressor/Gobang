#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QVector>
#include "config.h"
#include "ai.h"
#include <QMainWindow>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class gamescene: public QMainWindow
{
    Q_OBJECT
private:

    // 下棋AI
    AI ai;
    // 轮到谁
    bool isAI;
    // 红点
    QLabel* point;



    // 重写绘图事件
    void paintEvent(QPaintEvent *);
    // 重写鼠标点击事件
    void mousePressEvent(QMouseEvent * event);
    // AI下棋
    void AIGo();
    // 重新开始
    void restart();

public:
    gamescene();

signals:
    void letAIGo();

};

#endif // GAMESCENE_H
