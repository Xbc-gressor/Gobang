#include "ai.h"



AI::AI():humanList(SIDE_lENGTH, QVector<bool>(SIDE_lENGTH)), AIList(SIDE_lENGTH, QVector<bool>(SIDE_lENGTH)),AllList(SIDE_lENGTH, QVector<bool>(SIDE_lENGTH))
{

    // 初始化每个玩家的棋子堆
    for(int i = 0; i < SIDE_lENGTH; i++){
        for(int j = 0; j < SIDE_lENGTH; j++){
            humanList[i][j] = false;
            AIList[i][j] = false;
            AllList[i][j] = false;
        }
    }


    // 四个方向
    directs.append({0,1}); directs.append({1,1});
    directs.append({1,0}); directs.append({-1,1});

    // 分值map
    shapeScores.insert({0,1,1,0,0}, 70);     // 强2
    shapeScores.insert({0,1,0,1,0}, 60);
    shapeScores.insert({0,0,1,1,0}, 70);
    shapeScores.insert({0,1,0,0,1,0}, 50);
    shapeScores.insert({1,1,0,1,0}, 300);    // 弱3
    shapeScores.insert({0,0,1,1,1}, 500);
    shapeScores.insert({1,1,1,0,0}, 500);
    shapeScores.insert({0,1,1,1,0}, 5000);   // 强3
    shapeScores.insert({0,1,0,1,1,0}, 4800);
    shapeScores.insert({0,1,1,0,1,0}, 4800);
    shapeScores.insert({1,1,1,0,1}, 4950);   // 弱4
    shapeScores.insert({1,1,0,1,1}, 4900);
    shapeScores.insert({1,0,1,1,1}, 4950);
    shapeScores.insert({1,1,1,1,0}, 5000);
    shapeScores.insert({0,1,1,1,1}, 5000);
    shapeScores.insert({0,1,1,1,1,0}, 500000); // 强4
    shapeScores.insert({1,1,1,1,1}, 99999999); // 连5
}


// 对外无参接口
QPoint AI::AISearch()
{
    search(true, SEARCH_DEPTH, -99999999, 99999999);
    return nextPoint;
}

int AI::search(bool isAI, int depth, int Alpha, int Beta)
{
    if(gameOver(AIList) || gameOver(humanList) || depth == 0) //
        return evaluate(isAI);

    for(int x = 0; x < SIDE_lENGTH; x++)
    {
        for(int y = 0; y < SIDE_lENGTH; y++)
        {
//            qDebug()<< x<<' ' << y;
            // 跳过周围没有棋子的位置
            if(AllList[x][y] || !hasNeighbor(x, y))
                continue;

            if(isAI)
                AIList[x][y] = true;
            else
                humanList[x][y] = true;

            AllList[x][y] = true;


            // 向下搜索一层
            int value = -search(!isAI, depth-1, -Beta, -Alpha);
            // 回溯
            if(isAI)
                AIList[x][y] = false;
            else
                humanList[x][y] = false;
            AllList[x][y] = false;

            if(value > Alpha)
            {
                if(depth == SEARCH_DEPTH)
                {
                    // 回传到了根节点，更新nextPoint
                    nextPoint.setX(x); nextPoint.setY(y);
                }
                if(value >= Beta)
                    // 减枝
                    return Beta;
                Alpha = value;
            }
        }
    }

    return Alpha;

}

bool AI::hasNeighbor(int x, int y)
{
    for(int i = -1; i < 2; i++)
    {
        for(int j = -1; j < 2; j++)
        {
            if(i==0 && j==0)
                continue;
            if(x+i>=0 && x+i<SIDE_lENGTH && y+j>=0 && y+j<SIDE_lENGTH && AllList[x+i][y+j])
                return true;
        }
    }
    return false;
}

// 计算这一步自己的估计函数值
int AI::evaluate(bool isAI)
{

    QVector<QVector<bool>>& myList = isAI? AIList: humanList;
    QVector<QVector<bool>>& enemyList = isAI? humanList: AIList;

    // 自己所有的得分形状，每个形状由棋子 种类列表+方向 组成
    QVector<tuple<int, QVector<QPoint>, QPoint>> scoreAllShape;
    int myScores = 0;

    for(int i = 0; i < SIDE_lENGTH; i++)
    {
        for(int j =0; j < SIDE_lENGTH; j++)
        {
            if(myList[i][j])
            {
                foreach(QPoint dir,directs)
                {
                    myScores += calcu(i, j, dir, myList, enemyList, scoreAllShape);
                }
            }
        }
    }
    //qDebug()<< "===================================";

    // 敌人所有的得分形状
    int enemyScores = 0;
    QVector<tuple<int, QVector<QPoint>, QPoint>> enemyScoreAllShape;

    for(int i = 0; i < SIDE_lENGTH; i++)
    {
        for(int j = 0; j < SIDE_lENGTH; j++)
        {
            if(enemyList[i][j])
            {
                foreach(QPoint dir,directs)
                {
                    enemyScores += calcu(i, j, dir, enemyList, myList, enemyScoreAllShape);
                }
            }
        }
    }

    // 返回自己的分数减去敌人的分数的一部分
    return myScores - enemyScores * RADIO;
}

// 计算某一个棋子与附近棋子构成的棋列的得分
int AI::calcu(int x, int y, QPoint &dir, QVector<QVector<bool> > &myList, QVector<QVector<bool> > &enemyList, QVector<tuple<int, QVector<QPoint>, QPoint>>& allShapes)
{
    // 排除重复情况
    foreach(auto shape, allShapes)
    {
        foreach(auto point, std::get<1>(shape))
        {
            if(point.rx()==x && point.ry()==y && dir==std::get<2>(shape))
                return 0;
        }
    }

    // 该方向上的最高得分
    tuple<int, QVector<QPoint>, QPoint> maxShape;
    std::get<0>(maxShape) = 0;
    std::get<2>(maxShape) = dir;

    for(int offset = -5; offset < 1; offset++)
    {

        // 构造六子的01数组
        QVector<int> shape;
        for(int pos = 0; pos < 6; pos++)
        {
            int tmpX = x + (offset+pos)*dir.rx();
            int tmpY = y + (offset+pos)*dir.ry();

            if(tmpX < 0 || tmpY < 0 || tmpX >= SIDE_lENGTH || tmpY >= SIDE_lENGTH) // 到了棋盘外
                shape.append(2);
            else if(enemyList[tmpX][tmpY]) // 是敌人的棋子
                shape.append(2);
            else if(myList[tmpX][tmpY])
                shape.append(1);
            else
                shape.append(0);
        }
        // 取前五个，不同计分
        QVector<int> shape_{shape[0],shape[1],shape[2],shape[3],shape[4]};

        //查询这两个形状的对应得分
        for(auto s: {shape, shape_})
        {
            auto iter = shapeScores.find(s);
            if(iter != shapeScores.end())
            {
                if(iter.value() > std::get<0>(maxShape)) // 有更大的得分
                {
                    std::get<0>(maxShape) = iter.value();
                    std::get<1>(maxShape) = {{x+(offset+0)*dir.rx(),x+(offset+0)*dir.ry()},
                                       {x+(offset+1)*dir.rx(),x+(offset+1)*dir.ry()},
                                       {x+(offset+2)*dir.rx(),x+(offset+2)*dir.ry()},
                                       {x+(offset+3)*dir.rx(),x+(offset+3)*dir.ry()},
                                       {x+(offset+4)*dir.rx(),x+(offset+4)*dir.ry()}};
                }
            }
        }


    }
    // 交叉的杀局加更多分
    int extraScore = 0;
//    if(std::get<0>(maxShape) > 0)
//    {
//        foreach(auto shape, allShapes)
//        {
//            foreach(auto point1, std::get<1>(shape))
//            {
//                foreach(auto point2, std::get<1>(maxShape))
//                {
//                    if(point1 == point2)
//                    {
//                        qDebug()<< "123";
//                        extraScore += std::get<0>(maxShape) + std::get<0>(shape);
//                    }
//                }
//            }
//        }
//        allShapes.append(maxShape);
//    }

    if(std::get<0>(maxShape) > 0)
        allShapes.append(maxShape);

    return std::get<0>(maxShape) + extraScore;
}

bool AI::gameOver(QVector<QVector<bool>>& myList)
{

    for(int i = 0; i < SIDE_lENGTH; i++)
    {
        for(int j = 0; j < SIDE_lENGTH; j++)
        {
            foreach(auto dir, directs)
            {
                if(i+dir.rx()*4 >= SIDE_lENGTH || i+dir.rx()*4 < 0
                        || j+dir.ry()*4 >= SIDE_lENGTH || j+dir.ry()*4 < 0)
                    continue;
                for(int k = 0; k < 5; k++)
                    if(myList[i+dir.rx()*k][j+dir.ry()*k] == false)
                        goto here;
                return true;
                here: ;
            }
        }
    }
    return false;
}

