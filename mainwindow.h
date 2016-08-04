#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QPushButton>
#include <QMouseEvent>
#include <QAbstractButton>
#include <QPaintEvent>
#include <QWidget>
#include <QLabel>
#include <QFileDialog>
#include <colordlg.h>
#include <cstring>
#include <QApplication>
#include <stack>
#include <pthread.h>
#include <QtCore>
#include <QCloseEvent>
#include <QIODevice>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextBrowser>
#include <QTextEdit>
#include <QFont>
#include <QRectF>

//全局使用常量；
#define BUFSIZE 4096

#define WINDOW_SIZE_X 1150
#define WINDOW_SIZE_Y 688
#define CANVAS_X 650
#define CANVAS_Y 650
#define BUTTON_GAP 80

#define ALL_STEP_NUM 361

#define MACHINE_MACHINE 0
#define MAN_MACHINE_FIRST 1
#define MAN_MACHINE_SECOND 2
#define MAN_MAN 3
#define REGRET_PLAY 4
#define PUT_PLAY 5
#define GO_ON 6

//using namespace std;

// move resuem new regret
typedef int Player;
const Player NONE = 0;
const Player BLACK = 1;
const Player WHITE = 2;
const Player BACK = 3;

//棋盘大小；
const int BOARD_SIZE = 19;
//棋盘棋子最大数；
const int SQUARE_NUM = BOARD_SIZE * BOARD_SIZE;

struct Move
{
    int a;
    int b;
    Player player;
};

struct Board
{
    //走棋方存储；
    Player which_to_move[ALL_STEP_NUM];
    //棋盘；
    int board[BOARD_SIZE][BOARD_SIZE];
    //棋谱：
    //横坐标*纵坐标*执子方；
    Move move_history[SQUARE_NUM];
    //目前棋盘落子数；
    int step_num;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void closeEvent( QCloseEvent * event );

    //棋盘；
    Board board;
    //管道连接判断；
    BOOL fConnected, fConnected2;
    BOOL fSuccess;

    //	DWORD dwThreadId;
    HANDLE hPipe,hPipe2,hPipe3;
    LPTSTR lpszPipename;
    LPTSTR lpszPipename2;
    LPTSTR lpszPipename3;

    PROCESS_INFORMATION pi,pi2;
    //请求字符串；
    char chRequest[BUFSIZE];
    //返回字符串；
    char chReply[BUFSIZE];

    DWORD cbBytesRead, cbWritten;

    //模式选择：
    //MACHINE--机器
    //MAN--人
    //REGRET--悔棋
    //PUT--摆棋
    //GO_ON--继续开始；
    int chessmode;

    int AiIsWin;

    //画布
    QPixmap savepix;
    //画布上保存的走棋步数;
    int pristep_num;

    //功能按钮；
    QPushButton *go_on;
    QPushButton *machine;
    QPushButton *manmachine;
    QPushButton *men;
    QPushButton *regret_;
    QPushButton *put_;
    QPushButton *save_;
    QPushButton *quit_;

    //文件读取；
    QLabel *Tips1, *Tips2;
    QPushButton *open_, *read_, *show_;
    QTextEdit *Path_choise;
    QTextBrowser *Content_show;
    QString file_full, file_name, file_path;
    QFileInfo fi;

    //输赢棋方及其结果判断；
    int who_win;
    int isWin(int, int);
    int direction1(int, int);
    int direction2(int, int);
    int direction3(int, int);
    int direction4(int, int);

    //初始化；
    void init_widget();
    void init ();

    //控件按钮选择槽函数
    int cp();
    int endp();

    void machine_thread();

    void manmachine_f();

private slots:

    void game_of_machine();
    void machine_timer();

    void game_of_manmachine();

    void game_of_men();

    void regret_play();
    void put_play();
    void go_on_play();
    void sava_date();

    void open_file();
    void read_file();
    void show_file();
};


//初始化棋子轮下信息；
class MyMessageBox : public QObject
{
public:
    MyMessageBox();
    ~MyMessageBox();
    static int choosse_chess_node()
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("开始"));
        msg.setText("请选择所持棋子");
        msg.setStyleSheet("color:rgb(0, 0,0);font: 14pt");
        msg.setIcon(QMessageBox::Information);
        msg.addButton(tr("黑子"),QMessageBox::ActionRole);
        msg.addButton(tr("白子"),QMessageBox::ActionRole);
        return msg.exec();
    }
};
#endif // MAINWINDOW_H
