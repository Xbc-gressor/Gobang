#include "gamescene.h"

gamescene::gamescene()
{
//    ai.humanList[7][4] = true;
//    ai.humanList[7][5] = true;
//    ai.humanList[7][6] = true;
//    ai.humanList[7][7] = true;
//    ai.humanList[8][6] = true;
//    ai.AllList[7][4] = true;
//    ai.AllList[7][5] = true;
//    ai.AllList[7][6] = true;
//    ai.AllList[7][7] = true;
//    ai.AllList[8][6] = true;

//    ai.AIList[6][4] = true;
//    ai.AIList[6][6] = true;
//    ai.AIList[6][8] = true;
//    ai.AIList[7][8] = true;
//    ai.AllList[6][4] = true;
//    ai.AllList[6][6] = true;
//    ai.AllList[6][8] = true;
//    ai.AllList[7][8] = true;

//    QPoint p = ai.AISearch();
//    qDebug()<< p.rx()<< ' '<< p.ry();

    // 初始化红点
    QPixmap pix = QPixmap(":/res/point.png");
    pix = pix.scaled(2*SIZE, 2*SIZE);
    point = new QLabel(this);
    point->setPixmap(pix);


    isAI = false;
    ////////////////初始化游戏场景////////////////
    //设置固定大小
    this->setFixedSize(BOARDSIZE, BOARDSIZE+30);
    //设置图标
    this->setWindowIcon(QPixmap(":/res/icon.png"));
    //设置标题
    this->setWindowTitle(("游戏进行中"));

    connect(this, &gamescene::letAIGo, [=](){
        AIGo();
    });

    ///////////////创建菜单栏////////////////
        QMenuBar * bar = menuBar();
        this->setStyleSheet("QMenu::item:selected{background-color:#DEB887;}\
                                QMenuBar{background-color:#D2B48C;}");//紫色
        setMenuBar(bar);
        //创建开始菜单
        QMenu * startMenu = bar->addMenu("初级");
        //创建退出菜单项
        QAction * easy = startMenu->addAction("初级");
        QAction * medium = startMenu->addAction("中级");

        //点击调整难度
        connect(easy, &QAction::triggered,[=](){
            ai.SEARCH_DEPTH = 2;
            startMenu->setTitle("初级");
            restart();
        });
        connect(medium, &QAction::triggered,[=](){
            startMenu->setTitle("中级");
            ai.SEARCH_DEPTH = 3;
            restart();
        });

}


void gamescene::paintEvent(QPaintEvent *)
{
    //创建背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/checkerboard.jpg");
    pix = pix.scaled(BOARDSIZE, BOARDSIZE);
    painter.drawPixmap(0,30,pix.width(),pix.height(),pix);

}

void gamescene::mousePressEvent(QMouseEvent *event)
{
    // 必须要轮到人类下棋
    if(isAI)
        return;
    int x = event->x();
    int y = event->y();
    // 越界检测
    if(x < 0 || x > BOARDSIZE || y < 30 || y > 30+BOARDSIZE)
        return;

    // 逻辑位置
    int logX = x / 50;
    int logY = (y-30) / 50;
    int finalX = 25 + logX * 50;
    int finalY = 25 + 30 + logY * 50;
    // 在交叉点的一定范围内才认定
    if((x-finalX)*(x-finalX) + (y-finalY)*(y-finalY) > RANGE*RANGE)
        return;
    // 本来该位置不能有棋子
    if(ai.AllList[logY][logX])
        return;
    qDebug()<< y<< ' '<< x;

    // 绘制棋子
    QPixmap pix = QPixmap(":/res/black.png");
    pix = pix.scaled(2*SIZE, 2*SIZE);
    QLabel* label = new QLabel(this);
    label->setGeometry(finalX-SIZE, finalY-SIZE, pix.width(), pix.height());
    label->setPixmap(pix);
    label->show();


    // 更新棋堆
    ai.humanList[logY][logX] = true;
    ai.AllList[logY][logX] = true;


    isAI = true;
    if(ai.gameOver(ai.humanList))
    {
        QMessageBox:: StandardButton result = QMessageBox::information(NULL, "game over", "you win!", QMessageBox::Yes | QMessageBox::Reset, QMessageBox::Yes);
        if(result == QMessageBox::Reset)
            restart();

        qDebug()<< "你赢了";
    }
    if(isAI)
        emit letAIGo();
}

void gamescene::AIGo()
{
    QPoint nextpoint = ai.AISearch();
    int logX = nextpoint.rx();
    int logY = nextpoint.ry();
    qDebug()<< "ai "<< logX<< logY;
    int finalX = 25 + logY * 50;
    int finalY = 25 + 30 + logX * 50;

    // 绘制棋子
    QPixmap pix = QPixmap(":/res/white.png");
    pix = pix.scaled(2*SIZE, 2*SIZE);
    QLabel* label = new QLabel(this);
    label->setGeometry(finalX-SIZE, finalY-SIZE, pix.width(), pix.height());
    label->setPixmap(pix);
    label->show();

    // 更新棋堆
    ai.AIList[logX][logY] = true;
    ai.AllList[logX][logY] = true;
    // 移动红点
    point->setParent(label);
    point->setGeometry(0, 0, pix.width(), pix.height());
    point->show();

    if(ai.gameOver(ai.AIList))
    {
        QMessageBox:: StandardButton result = QMessageBox::information(NULL, "game over", "you lose!", QMessageBox::Yes | QMessageBox::Reset, QMessageBox::Yes);
        if(result == QMessageBox::Reset)
            restart();

        qDebug()<< "你输了";
    }


    isAI = false;
}

void gamescene::restart()
{


    // 清除所有的棋子
    QLabel* label = this->findChild<QLabel*>();

    while (label != NULL) {
        delete label;
        label = this->findChild<QLabel*>();
    }

    // 初始化每个玩家的棋子堆
    for(int i = 0; i < SIDE_lENGTH; i++){
        for(int j = 0; j < SIDE_lENGTH; j++){
            ai.humanList[i][j] = false;
            ai.AIList[i][j] = false;
            ai.AllList[i][j] = false;
        }
    }
    isAI = false;

    QPixmap pix = QPixmap(":/res/point.png");
    pix = pix.scaled(2*SIZE, 2*SIZE);
    point = new QLabel(this);
    point->setPixmap(pix);
}
