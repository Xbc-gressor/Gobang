#ifndef AI_H
#define AI_H

#include <QPoint>
#include <QMap>
#include <QVector>
#include "config.h"
#include <tuple>
#include <QDebug>

using namespace std;
class AI: public QObject
{
private:
    // 搜索深度
    int SEARCH_DEPTH = 2;
    // 下一步需要走的点
    QPoint nextPoint = {0, 0};
    // 所有搜索的方向
    QVector<QPoint> directs;
    // 各形状对应的分数
    QMap<QVector<int>,int> shapeScores;

    // 玩家和AI的棋子堆
    QVector<QVector<bool>> humanList;
    QVector<QVector<bool>> AIList;
    QVector<QVector<bool>> AllList;

    // 迭代搜索
    int search(bool isAI, int depth, int alpha, int Beta);
    // 是否邻近位置有棋子
    bool hasNeighbor(int x, int y);
    // 评估函数
    int evaluate(bool isAI);
    // 计算某点附近拓展的得分
    int calcu(int x, int y, QPoint& dir, QVector<QVector<bool>>& myList, QVector<QVector<bool>>& enemyList, QVector<tuple<int,QVector<QPoint>,QPoint>>& shape);
    // 游戏值是否结束
    bool gameOver(QVector<QVector<bool>>& myList);
public:
    AI();
    // 非递归入口
    QPoint AISearch();

    friend class gamescene;
};

#endif // AI_H
