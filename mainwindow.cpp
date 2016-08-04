#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BenchMark");
    //窗口大小以及棋盘信息：
    //32*19+150*2；32*19+50*2；
    //19*19；
    resize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    memset(board.board, NONE, BOARD_SIZE * BOARD_SIZE * sizeof(int));

    chessmode = MAN_MAN;
    AiIsWin = 0;

    // default security attribute
    init_widget();
    init();


    lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");
    lpszPipename2 = TEXT("\\\\.\\pipe\\mynamedpipe2");
    lpszPipename3 = TEXT("\\\\.\\pipe\\mynamedpipe3");

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(machine_timer()));
    timer->start(100);

}

MainWindow::~MainWindow()
{

}

//控件初始化；
void MainWindow::init_widget()
{
    //Button
    go_on = new QPushButton("继续开始",this);
    go_on->setGeometry(60,20,80,40);
    go_on->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(go_on,SIGNAL(clicked()),this,SLOT(go_on_play()));

    machine = new QPushButton("机器博弈",this);
    machine->setGeometry(60,20 + BUTTON_GAP,80,40);
    machine->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(machine,SIGNAL(clicked()),this,SLOT(game_of_machine()));

    manmachine = new QPushButton("人机博弈",this);
    manmachine->setGeometry(60,20 + BUTTON_GAP * 2,80,40);
    manmachine->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(manmachine,SIGNAL(clicked()),this,SLOT(game_of_manmachine()));

    men = new QPushButton("人人博弈",this);
    men->setGeometry(60,20 + BUTTON_GAP * 3,80,40);
    men->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(men,SIGNAL(clicked()),this,SLOT(game_of_men()));

    regret_ = new QPushButton("悔棋",this);
    regret_->setGeometry(60,20 + BUTTON_GAP * 4,80,40);
    regret_->setFont(QFont("宋体",16,QFont::Bold));
    connect(regret_,SIGNAL(clicked()),this,SLOT(regret_play()));

    put_ = new QPushButton("摆棋",this);
    put_->setGeometry(60,20 + BUTTON_GAP * 5,80,40);
    put_->setFont(QFont("宋体",16,QFont::Bold));
    connect(put_,SIGNAL(clicked()),this,SLOT(put_play()));

    save_ = new QPushButton("棋谱保存",this);
    save_->setGeometry(60,20 + BUTTON_GAP * 6,80,40);
    save_->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(save_,SIGNAL(clicked(bool)),this,SLOT(sava_date()));

    quit_ = new QPushButton("退出",this);
    quit_->setGeometry(60,20 + BUTTON_GAP * 7,80,40);
    quit_->setFont(QFont("宋体",16,QFont::Bold));
    connect(quit_,SIGNAL(clicked(bool)),qApp,SLOT(quit()));

    //File read
    Tips1 = new QLabel("请选择要读取的棋谱文件：",this);
    Tips1->setGeometry(830,20,200,40);
    Tips1->setFont(QFont("宋体",10,QFont::Bold));

    Tips2 = new QLabel("棋谱内容：",this);
    Tips2->setGeometry(830,130,200,40);
    Tips2->setFont(QFont("宋体",10,QFont::Bold));

    Path_choise = new QTextEdit(this);
    Path_choise->setGeometry(830,70,180,50);
    Path_choise->setFont(QFont("宋体",10));

    Content_show = new QTextBrowser(this);
    Content_show->setGeometry(830,180,240,400);
    Content_show->setFont(QFont("宋体",10));

    open_ = new QPushButton("打开",this);
    open_->setGeometry(1070,70,60,25);
    open_->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(open_,SIGNAL(clicked(bool)),this,SLOT(open_file()));

    read_ = new QPushButton("读取",this);
    read_->setGeometry(1070,140,60,25);
    read_->setFont(QFont("宋体",13.5,QFont::Bold));
    connect(read_,SIGNAL(clicked(bool)),this,SLOT(read_file()));

    show_ = new QPushButton("显示",this);
    show_->setGeometry(990,600,80,40);
    show_->setFont(QFont("宋体",14,QFont::Bold));
    connect(show_,SIGNAL(clicked(bool)),this,SLOT(show_file()));
}


//棋盘棋子以及棋谱信息初始化；
void MainWindow:: init()
{
    who_win = NONE;

    if(chessmode == MACHINE_MACHINE)
        endp();
    else if(chessmode < MAN_MAN)
        TerminateProcess(pi.hProcess,4);

    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            board.step_num = 0;
            board.move_history[board.step_num].player = NONE;
            board.board[i][j] = NONE;
            board.step_num++;
        }
    }

    board.step_num = 0;

    //初始化走棋方存储信息；
    for(int i = 0; i < ALL_STEP_NUM; i++)
    {
        if(i%4 == 1 || i%4 == 2)
            board.which_to_move[i] = WHITE;
        else
            board.which_to_move[i] = BLACK;
    }
}

//窗口关闭提示；
void MainWindow::closeEvent( QCloseEvent * event )
{
   switch( QMessageBox::information( this, tr("提示："),
   tr("程序正在运行，确认要退出吗？"),
   tr("确认"), tr("取消"), 0, 1 ) )
   {
    case 0:
    event->accept();
       break;
    case 1:
    default:
       event->ignore();
       break;
   }
 }

int MainWindow::cp()
{

}

int MainWindow::endp()
{
    TerminateProcess(pi.hProcess,4);
    TerminateProcess(pi2.hProcess, 4);
    return 0;
}


//机器博弈；
void MainWindow::machine_timer()
{
    if(chReply[0] == 'r')
    {
        QMessageBox::information(this, "Win", "the first engine Win", QMessageBox::Ok);
        chReply[0] = 0;
    }
    if(AiIsWin)
    {
        QMessageBox::information(this, "Win", "The Comeputer Win", QMessageBox::Ok);
        AiIsWin = 0;
    }
    update();
}

void machine_thread2(void *argv)
{
    MainWindow *th = (MainWindow*)argv;
    th->machine_thread();
}

void MainWindow::game_of_machine()
{
    init();
    if(chessmode != MAN_MAN)
        Sleep(1000);

    hPipe = CreateNamedPipe(
         lpszPipename,             // pipe name
         PIPE_ACCESS_DUPLEX,       // read/write access
         PIPE_TYPE_MESSAGE |       // message type pipe
         PIPE_READMODE_MESSAGE |   // message-read mode
         PIPE_WAIT,                // blocking mode
         PIPE_UNLIMITED_INSTANCES, // max. instances
         BUFSIZE,                  // output buffer size
         BUFSIZE,                  // input buffer size
         0,                        // client time-out
         NULL);                    // default security attribute
    hPipe2 = CreateNamedPipe(
        lpszPipename2,             // pipe name
        PIPE_ACCESS_DUPLEX,       // read/write access
        PIPE_TYPE_MESSAGE |       // message type pipe
        PIPE_READMODE_MESSAGE |   // message-read mode
        PIPE_WAIT,                // blocking mode
        PIPE_UNLIMITED_INSTANCES, // max. instances
        BUFSIZE,                  // output buffer size
        BUFSIZE,                  // input buffer size
        0,                        // client time-out
        NULL);
    hPipe3 = CreateNamedPipe(
        lpszPipename3,             // pipe name
        PIPE_ACCESS_DUPLEX,       // read/write access
        PIPE_TYPE_MESSAGE |       // message type pipe
        PIPE_READMODE_MESSAGE |   // message-read mode
        PIPE_WAIT,                // blocking mode
        PIPE_UNLIMITED_INSTANCES, // max. instances
        BUFSIZE,                  // output buffer size
        BUFSIZE,                  // input buffer size
        0,                        // client time-out
        NULL);

   chessmode = MACHINE_MACHINE;
   STARTUPINFO si;
   memset(&si, 0, sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.dwFlags = STARTF_USESIZE;
   si.dwXSize = 80;
   si.dwYSize = 480;

   QString str = QFileDialog::getOpenFileName(this,"请选择引擎一","/","可执行文件(*.exe)");
   const QChar* szLogin = str.unicode();
   if (CreateProcess((LPCWSTR)szLogin,NULL, NULL, NULL, TRUE,
       NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
       NULL, NULL, &si, &pi))
   {

       CloseHandle(pi.hThread);
//		CloseHandle(pi.hProcess);
   }
   chReply[0] = '1';
   chReply[1] = 0;
   fConnected = ConnectNamedPipe(hPipe3, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
   fSuccess = WriteFile(
       hPipe3,        // handle to pipe
       chReply,      // buffer to write from
       (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
       &cbWritten,   // number of bytes written
       NULL);

   if(!fSuccess)
   {
       CloseHandle(hPipe3);
       return ;
   }

   hPipe3 = CreateNamedPipe(
               lpszPipename3,             // pipe name
               PIPE_ACCESS_DUPLEX,       // read/write access
               PIPE_TYPE_MESSAGE |       // message type pipe
               PIPE_READMODE_MESSAGE |   // message-read mode
               PIPE_WAIT,                // blocking mode
               PIPE_UNLIMITED_INSTANCES, // max. instances
               BUFSIZE,                  // output buffer size
               BUFSIZE,                  // input buffer size
               0,                        // client time-out
               NULL);
   str = QFileDialog::getOpenFileName(this,"请选择引擎二","/","可执行文件(*.exe)");
   const QChar* szLogin2 = str.unicode();
   if (CreateProcess((LPCWSTR)szLogin2,NULL,
       NULL, NULL, TRUE,
       NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
       NULL, NULL, &si, &pi2))
   {
       CloseHandle(pi2.hThread);
//		CloseHandle(pi2.hProcess);
   }
   chReply[0]='2';
   chReply[1]=0;
   fConnected = ConnectNamedPipe(hPipe3, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
   fSuccess = WriteFile(
       hPipe3,        // handle to pipe
       chReply,      // buffer to write from
       (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
       &cbWritten,   // number of bytes written
       NULL);
   if(!fSuccess)
   {
       CloseHandle(hPipe3);
       return ;
   }
   _beginthread(machine_thread2,4096,this);
}

void MainWindow::machine_thread()
{

//   int playercolor = BLACK;
//   int computercolor = WHITE;
   board.step_num = 0;
   chReply[0] = 'n';
   chReply[1] = 'e';
   chReply[2] = 'w';
   chReply[3] = ' ';
   chReply[4] = 'b';
   chReply[5] = 0;
   fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
   fConnected2 = ConnectNamedPipe(hPipe2, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
   if(!fConnected || !fConnected2)
   {
       CloseHandle(hPipe);
       CloseHandle(hPipe2);
       return ;
   }
   fSuccess = WriteFile(
       hPipe,        // handle to pipe
       chReply,      // buffer to write from
       (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
       &cbWritten,   // number of bytes written
       NULL);        // not overlapped I/O
   if(!fSuccess)
   {
       CloseHandle(hPipe);
       CloseHandle(hPipe2);
       return ;
   }
   chReply[4] = 'w';
   fSuccess = WriteFile(
       hPipe2,        // handle to pipe
       chReply,      // buffer to write from
       (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
       &cbWritten,   // number of bytes written
       NULL);        // not overlapped I/O
   if(!fSuccess)
   {
       CloseHandle(hPipe);
       CloseHandle(hPipe2);
       return ;
   }
           while (1)
           {
                int x,y;
               fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
               if(!fConnected)
               {
                   CloseHandle(hPipe);
                   CloseHandle(hPipe2);
                   return ;
               }
                  if (fConnected)
                   {
                       fSuccess = ReadFile(
                           hPipe,
                           chRequest,
                           BUFSIZE*sizeof(char),
                           &cbBytesRead,
                           NULL);
                       if(!fSuccess)
                       {
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
//                       printf("引擎 1 回复: %s\n", chRequest);
//                       if (chRequest[0] == 'q')break;
                       y = chRequest[1] - 48;
                       x = chRequest[2] - 48;
                       board.board[x][y] = board.which_to_move[board.step_num];
                       board.move_history[board.step_num].a = x;
                       board.move_history[board.step_num].b = y;
                       board.move_history[board.step_num].player = board.which_to_move[board.step_num];
                       board.step_num++;
                       chReply[0] = 'm';
                       chReply[1] = 'o';
                       chReply[2] = 'v';
                       chReply[3] = 'e';
                       chReply[4] = ' ';
                       chReply[5] = y+'a';
                       chReply[6] = x+'a';
                       chReply[7] = 0;
                       if(isWin(x, y))
                       {
                           chReply[0] = 'r';
                           fSuccess = WriteFile(
                               hPipe,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                           fSuccess = WriteFile(
                               hPipe2,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                           chessmode = MAN_MAN;
                           endp();
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
                       if(board.step_num > 1)
                       {
                           y = chRequest[3] - 48;
                           x = chRequest[4] - 48;
                           board.board[x][y] = board.which_to_move[board.step_num];
                           board.move_history[board.step_num].a = x;
                           board.move_history[board.step_num].b = y;
                           board.move_history[board.step_num].player = board.which_to_move[board.step_num];
                           board.step_num++;
                           chReply[7] = y + 'a';
                           chReply[8] = x + 'a';
                           chReply[9] = 0;
                           if(isWin(x, y))
                           {
                               chReply[0] = 'r';
                               fSuccess = WriteFile(
                                   hPipe,        // handle to pipe
                                   chReply,      // buffer to write from
                                   (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                                   &cbWritten,   // number of bytes written
                                   NULL);
                               if(!fSuccess)
                               {
                                   CloseHandle(hPipe);
                                   CloseHandle(hPipe2);
                                   return ;
                               }
                               fSuccess = WriteFile(
                                   hPipe2,        // handle to pipe
                                   chReply,      // buffer to write from
                                   (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                                   &cbWritten,   // number of bytes written
                                   NULL);
                               if(!fSuccess)
                               {
                                   CloseHandle(hPipe2);
                                   CloseHandle(hPipe);
                                   return ;
                               }
    //                           setEnabled(false);
                               chessmode = MAN_MAN;
                               endp();
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                       }

                       fSuccess = WriteFile(
                           hPipe2,
                           chReply,
                           (strlen(chReply) + 1)*sizeof(char),
                           &cbWritten,
                           NULL);
                       if(!fSuccess)
                       {
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
                   }
//                    QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
                   fConnected2 = ConnectNamedPipe(hPipe2, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
                   if(!fConnected2)
                   {
                       CloseHandle(hPipe);
                       CloseHandle(hPipe2);
                       return ;
                   }
                   if (fConnected2)
                   {

                       fSuccess = ReadFile(
                           hPipe2,        // handle to pipe
                           chRequest,    // buffer to receive data
                           BUFSIZE*sizeof(char), // size of buffer
                           &cbBytesRead, // number of bytes read
                           NULL);        // not overlapped I/O
//                       QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
                       if(!fSuccess)
                       {
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
                       y = chRequest[1] - 48;
                       x = chRequest[2] - 48;
                       board.board[x][y] = board.which_to_move[board.step_num];
                       board.move_history[board.step_num].a = x;
                       board.move_history[board.step_num].b = y;
                       board.move_history[board.step_num].player = board.which_to_move[board.step_num];
                       board.step_num++;
                       chReply[0] = 'm';
                       chReply[1] = 'o';
                       chReply[2] = 'v';
                       chReply[3] = 'e';
                       chReply[4] = ' ';
                       chReply[5] = y+'a';
                       chReply[6] = x+'a';
                       chReply[7] = 0;
                       if(isWin(x, y))
                       {
                           chReply[0] = 'r';
                           fSuccess = WriteFile(
                               hPipe,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                           fSuccess = WriteFile(
                               hPipe2,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                           chessmode = MAN_MAN;
                           endp();
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
                       y = chRequest[3] - 48;
                       x = chRequest[4] - 48;
                       board.board[x][y] = board.which_to_move[board.step_num];
                       board.move_history[board.step_num].a = x;
                       board.move_history[board.step_num].b = y;
                       board.move_history[board.step_num].player = board.which_to_move[board.step_num];
                       board.step_num++;
                       chReply[7] = y + 'a';
                       chReply[8] = x + 'a';
                       chReply[9] = 0;
                       if(isWin(x, y))
                       {
  //                         update();
                           chReply[0] = 'r';
                           fSuccess = WriteFile(
                               hPipe,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
                           fSuccess = WriteFile(
                               hPipe2,        // handle to pipe
                               chReply,      // buffer to write from
                               (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                               &cbWritten,   // number of bytes written
                               NULL);
                           if(!fSuccess)
                           {
                               CloseHandle(hPipe);
                               CloseHandle(hPipe2);
                               return ;
                           }
//                           setEnabled(false);
                           chessmode = MAN_MAN;
                           endp();
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }
                       fSuccess = WriteFile(
                           hPipe,        // handle to pipe
                           chReply,      // buffer to write from
                           (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                           &cbWritten,   // number of bytes written
                           NULL);        // not overlapped I/O
                       if(!fSuccess)
                       {
                           CloseHandle(hPipe);
                           CloseHandle(hPipe2);
                           return ;
                       }

                   }
           }
    endp();
}


//人机博弈；
void manmachine_thread(void *argv)
{
    MainWindow *th = (MainWindow*)argv;
    th->manmachine_f();
}

void MainWindow::game_of_manmachine()
{
    init();
    update();

    if(chessmode != MAN_MAN)
        Sleep(1000);

    int x,y;
    STARTUPINFO si;
    memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESIZE;
    si.dwXSize = 80;
    si.dwYSize = 480;

   /* 临时注释，不要删
    if (CreateProcess(TEXT("six1.exe"),NULL,
        NULL, NULL, TRUE,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
        NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hThread);
    }
    */

    hPipe2 = CreateNamedPipe(
                lpszPipename3,             // pipe name
                PIPE_ACCESS_DUPLEX,       // read/write access
                PIPE_TYPE_MESSAGE |       // message type pipe
                PIPE_READMODE_MESSAGE |   // message-read mode
                PIPE_WAIT,                // blocking mode
                PIPE_UNLIMITED_INSTANCES, // max. instances
                BUFSIZE,                  // output buffer size
                BUFSIZE,                  // input buffer size
                0,                        // client time-out
                NULL);
    hPipe = CreateNamedPipe(
                lpszPipename,             // pipe name
                PIPE_ACCESS_DUPLEX,       // read/write access
                PIPE_TYPE_MESSAGE |       // message type pipe
                PIPE_READMODE_MESSAGE |   // message-read mode
                PIPE_WAIT,                // blocking mode
                PIPE_UNLIMITED_INSTANCES, // max. instances
                BUFSIZE,                  // output buffer size
                BUFSIZE,                  // input buffer size
                0,                        // client time-out
                NULL);

    QString str = QFileDialog::getOpenFileName(this,"请选择引擎","/","可执行文件(*.exe)");
    const QChar* szLogin = str.unicode();
    if (CreateProcess((LPCWSTR)szLogin,NULL, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL, &si, &pi))
        {
             CloseHandle(pi.hThread);
        }


    chReply[0] = '1';
    chReply[1] = 0;
    fConnected = ConnectNamedPipe(hPipe2, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    fSuccess = WriteFile(
        hPipe2,        // handle to pipe
        chReply,      // buffer to write from
        (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
        &cbWritten,   // number of bytes written
        NULL);
    if(!fSuccess)
    {
        CloseHandle(hPipe2);
        return ;
    }
    chessmode = MAN_MACHINE_FIRST;
    board.step_num = 0;
    chReply[0] = 'n';
    chReply[1] = 'e';
    chReply[2] = 'w';
    chReply[3] = ' ';
    chReply[4] = 'b';
    chReply[5] = 0;

    int choosen = MyMessageBox::choosse_chess_node();
    if(choosen == 0)
        chReply[4]='w';
    fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if(!fConnected)
    {
        CloseHandle(hPipe);
        return ;
    }

    fSuccess = WriteFile(
        hPipe,        // handle to pipe
        chReply,      // buffer to write from
        (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
        &cbWritten,   // number of bytes written
        NULL);
    if(!fSuccess)
    {
        CloseHandle(hPipe);
        return ;
    }

    if(choosen == 1)
    {
        chessmode = MACHINE_MACHINE;
        fSuccess = ReadFile(
            hPipe,        // handle to pipe
            chRequest,    // buffer to receive data
            BUFSIZE*sizeof(char), // size of buffer
            &cbBytesRead, // number of bytes read
            NULL);        // not overlapped I/O
    //                       QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
        if(!fSuccess)
        {
            CloseHandle(hPipe);
            return ;
        }
        y = chRequest[1] - 48;
        x = chRequest[2] - 48;
        board.board[x][y] = board.which_to_move[board.step_num];
        board.move_history[board.step_num].a = x;
        board.move_history[board.step_num].b = y;
        board.move_history[board.step_num].player = board.which_to_move[board.step_num];
        board.step_num++;
        update();

        chessmode = MAN_MACHINE_FIRST;
    }
}

void MainWindow::manmachine_f()
{
    fSuccess = ReadFile(
        hPipe,        // handle to pipe
        chRequest,    // buffer to receive data
        BUFSIZE*sizeof(char), // size of buffer
        &cbBytesRead, // number of bytes read
        NULL);        // not overlapped I/O
//                  QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
    int x,y;
    y = chRequest[1] - 48;
    x = chRequest[2] - 48;
    board.board[x][y] = board.which_to_move[board.step_num];
    board.move_history[board.step_num].a = x;
    board.move_history[board.step_num].b = y;
    board.move_history[board.step_num].player = board.which_to_move[board.step_num];
    board.step_num++;
    if(isWin(x, y))
    {
        AiIsWin = 1;
        CloseHandle(hPipe);
        CloseHandle(hPipe2);
        endp();
        chessmode = MAN_MAN;
        return ;
    }
    y = chRequest[3] - 48;
    x = chRequest[4] - 48;
    board.board[x][y] = board.which_to_move[board.step_num];
    board.move_history[board.step_num].a = x;
    board.move_history[board.step_num].b = y;
    board.move_history[board.step_num].player = board.which_to_move[board.step_num];
    board.step_num++;
    if(isWin(x, y))
    {
        AiIsWin = 1;
        CloseHandle(hPipe);
        CloseHandle(hPipe2);
        endp();
        chessmode = MAN_MAN;
        return ;
    }
    chessmode = MAN_MACHINE_FIRST;
}


//人人博弈；
void MainWindow::game_of_men()
{
    init();
    chessmode = MAN_MAN;
    update();
}



//悔棋槽函数；
void MainWindow::regret_play()
{

    if(chessmode == MAN_MACHINE_FIRST && board.step_num > 1)
    {
        board.step_num -= 4;
        if(board.step_num < 0)
            board.step_num = 0;
        for(int i = board.step_num;i < board.step_num + 4;i++)
            board.board[board.move_history[i].a][board.move_history[i].b] = NONE;

        chessmode = REGRET_PLAY;
        repaint();
        chessmode = MAN_MACHINE_FIRST;
        chReply[0] = 'h';

        fSuccess = WriteFile(
            hPipe,        // handle to pipe
            chReply,      // buffer to write from
            (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
            &cbWritten,   // number of bytes written
            NULL);
    }
    else if(chessmode == MAN_MACHINE_SECOND)
    {
        board.step_num--;
        board.board[board.move_history[board.step_num].a][board.move_history[board.step_num].b] = NONE;
        chessmode = REGRET_PLAY;
        repaint();
        chessmode = MAN_MACHINE_FIRST;
    }
    else if(chessmode == MAN_MAN)
    {
        //悔棋在棋谱中保存记录；
        /*
        board.move_history[board.step_num].a = 0;
        board.move_history[board.step_num].b = 0;
        */
        board.move_history[board.step_num].player = BACK;
        board.step_num--;
        chessmode = REGRET_PLAY;
        repaint();
        chessmode = MAN_MAN;
       // 如果加上，则在连续执行悔棋命令时，无论多少次都只相当于一次。
       // chessmode = PUT_PLAY 情况下同理；
       // board.step_num++;
    }
    else if(chessmode == PUT_PLAY)
    {
       // board.move_history[board.step_num].player = BACK;
        board.move_history[board.step_num].player = BACK;
        board.step_num--;
        chessmode = REGRET_PLAY;
        repaint();
        chessmode = PUT_PLAY;
       // board.step_num++;
    }
    else if(chessmode == GO_ON)
    {
       // board.move_history[board.step_num].player = BACK;
        board.move_history[board.step_num].player = BACK;
        board.step_num--;
        chessmode = REGRET_PLAY;
        repaint();
        chessmode = GO_ON;
       // board.step_num++;
    }
}

//摆棋槽函数；
void MainWindow::put_play()
{
    int choosen = MyMessageBox::choosse_chess_node();
    if(choosen == 0)
    {
        for(int i = board.step_num;i < ALL_STEP_NUM;i++)
            board.which_to_move[i] = BLACK;
    }
    else if(choosen == 1)
    {
        for(int i = board.step_num;i < ALL_STEP_NUM;i++)
            board.which_to_move[i] = WHITE;
    }

    chessmode = PUT_PLAY;
}

//继续开始；
//点击结束当前指令并按照当前棋局界面继续开始；
void MainWindow::go_on_play()
{
    for(int i = board.step_num;i < ALL_STEP_NUM;i++)
    {
        if(i%4 == 1 || i%4 == 2)
            board.which_to_move[i] = WHITE;
        else
            board.which_to_move[i] = BLACK;
    }
    /*
    present_num = board.step_num;
    init();
    board.step_num = present_num;
    */
    chessmode = GO_ON;
}

//绘画事件；
void MainWindow::paintEvent(QPaintEvent *)
{
     QPainter pp(this);
     QPixmap pix;
     pix=QPixmap(CANVAS_X,CANVAS_Y);
     pix.fill(Qt::white);
//   p.setRenderHint(QPainter::Antialiasing, true);

     if(board.step_num == 0||chessmode == REGRET_PLAY || chessmode == PUT_PLAY)
     {
         int ps = 40,px = 40;
         int i;
         QPainter p(&pix);
         for (i = 0; i < BOARD_SIZE; i++)
         {
             p.drawLine(ps,  ps + i * 32,ps + 576, ps + i * 32);
             p.drawLine(px + i * 32, px, px + i * 32, px + 576);
         }

         //横向坐标；
         QFont pen("Arial",20,QFont::Bold,true);//设置字体的类型，大小，加粗，斜体
         pen.setLetterSpacing(QFont::AbsoluteSpacing,3.1);//设置间距
         p.setFont(pen);
         QRectF squareness_1(18,0,612,36);
         p.drawRect(squareness_1);
         p.setPen(QColor(Qt::gray));
         p.drawText(squareness_1,Qt::AlignCenter,"  A B C D E F G H I J K L M N O P Q R S");
         //p2.drawText(36,10,"  A B C D E F G H I J K L M N O P Q R S");

         //纵向坐标;
         pen.setLetterSpacing(QFont::AbsoluteSpacing,2.5);
         p.setFont(pen);
         QRectF squareness_2(0,18,36,612);
         p.drawRect(squareness_2);
         p.setPen(QColor(Qt::gray));
         p.drawText(squareness_2,Qt::AlignCenter," A\n B\n C\n D\n E\n F\n G\n H\n I\n J\n K\n L\n M\n N\n O\n P\n Q\n R\n S");


         QBrush brush;
         brush.setStyle(Qt::SolidPattern);
         brush.setColor(Qt::gray);
         p.setBrush(brush);

         for(i = 0;i < 3;i++)
         {
             for(int j = 0;j < 3;j++)
             {
                 p.drawEllipse(ps - 6 + 96 * (i * 2 + 1),px - 6 + 96 * (j * 2 + 1),12,12);
             }
         }

         brush.setStyle(Qt::SolidPattern);
         p.setBrush(brush);

         //进行悔棋或者摆棋时从第一步绘画到悔棋（摆棋）步；
         for(i = 0;i < board.step_num;i++)
         {
             if (board.move_history[i].player == WHITE)
             {
                 brush.setColor(Qt::white);
                 p.setBrush(brush);
                 p.drawEllipse(QPoint(ps+ board.move_history[i].a * 32,
                             px+board.move_history[i].b * 32),14,14);
             }
             else if (board.move_history[i].player == BLACK)
             {
                 brush.setColor(Qt::black);
                 p.setBrush(brush);
                 p.drawEllipse(QPoint( ps+board.move_history[i].a * 32,
                             px+board.move_history[i].b * 32),14,14);
             }

         }
     }
     else
     {
         int ps = 40,px = 40;
         int i;

         pix = savepix;
         QPainter p(&pix);
         QBrush brush;
         brush.setStyle(Qt::SolidPattern);
         p.setBrush(brush);

         for(i = pristep_num;i < board.step_num;i++)
         {
             if (board.move_history[i].player == WHITE)
             {
                 brush.setColor(Qt::white);
                 p.setBrush(brush);
                 p.drawEllipse(QPoint(ps+ board.move_history[i].a * 32,
                             px+board.move_history[i].b * 32),14,14);
             }
             else if (board.move_history[i].player == BLACK)
             {
                 brush.setColor(Qt::black);
                 p.setBrush(brush);
                 p.drawEllipse(QPoint( ps+board.move_history[i].a * 32,
                             px+board.move_history[i].b * 32),14,14);
             }

         }

     }
     pristep_num = board.step_num;
     savepix = pix;

     pp.drawPixmap(160,30,pix);
}

//鼠标事件；
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    int x, y;
    int ps = 40, px = 40;

    if(e->x() >= 150 + ps && e->x() < 742 + ps && e->y() >= 20 + px && e->y() < 612 + px && chessmode)
    {
        x = (e->x() - 150 - ps) / 32;
        y = (e->y() - 20 - px) / 32;

        if (board.board[x][y] == NONE || board.move_history[board.step_num].player == BACK)
        {
            board.board[x][y] = board.which_to_move[board.step_num];
            board.move_history[board.step_num].a = x;
            board.move_history[board.step_num].b = y;
            board.move_history[board.step_num].player = board.which_to_move[board.step_num];
            board.step_num++;

            //摆棋模式下再点击原来的棋子更换颜色；
            if(chessmode == PUT_PLAY)
            {
                if(board.board[x][y] == BLACK)
                {
                    board.board[x][y] = WHITE;
                    board.move_history[board.step_num].player = WHITE;
                }
                else if(board.board[x][y] == WHITE)
                {
                    board.board[x][y] = BLACK;
                    board.move_history[board.step_num].player = BLACK;
                }

                if(isWin(x, y))
                {
                    update();
                    setEnabled(true);
                    if(who_win == BLACK)
                        QMessageBox::information(this, "胜利方：", "黑方！", QMessageBox::Ok);
                    else if(who_win == WHITE)
                        QMessageBox::information(this, "胜利方:", "白方！", QMessageBox::Ok);
                }
            }

            if(chessmode < MAN_MAN)
            {
                if(chessmode == MAN_MACHINE_FIRST)
                {
                    chReply[0] = 'm';
                    chReply[1] = 'o';
                    chReply[2] = 'v';
                    chReply[3] = 'e';
                    chReply[4] = ' ';
                    chReply[5] = y + 'a';
                    chReply[6] = x + 'a';
                    chReply[7] = 0;
                    chessmode++;
                    if(board.step_num == 1)
                    {
                        chessmode = MACHINE_MACHINE;
                        repaint();
                        fSuccess = WriteFile(
                            hPipe,        // handle to pipe
                            chReply,      // buffer to write from
                            (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                            &cbWritten,   // number of bytes written
                            NULL);
                        _beginthread(manmachine_thread,4096,this);
                        return ;
                    }
                    else if(isWin(x, y))
                    {
                        update();
        //                setEnabled(false);
                        QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
                        CloseHandle(hPipe);
                        CloseHandle(hPipe2);
                        chessmode = MAN_MAN;
                        return ;
                    }
                }
                else
                {
                    chReply[7] = y + 'a';
                    chReply[8] = x + 'a';
                    chReply[9] = 0;
                    fSuccess = WriteFile(
                        hPipe,        // handle to pipe
                        chReply,      // buffer to write from
                        (strlen(chReply) + 1)*sizeof(char), // number of bytes to write
                        &cbWritten,   // number of bytes written
                        NULL);
                    chessmode = MACHINE_MACHINE;
                    repaint();
                    if(isWin(x, y))
                    {
                        QMessageBox::information(this, "Win", "Win", QMessageBox::Ok);
                        CloseHandle(hPipe);
                        CloseHandle(hPipe2);
                        chessmode = MAN_MAN;
                        return ;
                    }
                    _beginthread(manmachine_thread,4096,this);
                }

            }
        }

        /*
        if(board.move_history[board.step_num + 1].player == BACK)
        {
            board.board[x][y] = board.which_to_move[board.step_num + 1];
            board.move_history[board.step_num + 1].a = x;
            board.move_history[board.step_num + 1].b = y;
            board.move_history[board.step_num + 1].player = board.which_to_move[board.step_num + 1];
            board.step_num = board.step_num + 2;
        }
        */

        if((isWin(x, y) && chessmode == MAN_MAN) || (isWin(x, y) && chessmode == PUT_PLAY) || (isWin(x, y) && chessmode == GO_ON))
        {
            update();

            setEnabled(true);
            if(who_win == BLACK)
            {
                QMessageBox::information(this, "胜利方:", "黑方！", QMessageBox::Ok);
            }
            else if(who_win == WHITE)
            {
                QMessageBox::information(this, "胜利方:", "白方！", QMessageBox::Ok);
            }
        }

        update();
    }
}

//输赢结果；
int MainWindow::isWin(int x, int y)
{
     return direction1(x, y) || direction2(x, y) || direction3(x, y) || direction4(x ,y);
}

int MainWindow::direction1(int x, int y)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if(y - i >= 0 &&
           y + 5 - i <= BOARD_SIZE - 1 &&
           board.board[x][y - i] == board.board[x][y + 1 - i] &&
           board.board[x][y - i] == board.board[x][y + 2 - i] &&
           board.board[x][y - i] == board.board[x][y + 3 - i] &&
           board.board[x][y - i] == board.board[x][y + 4 - i] &&
           board.board[x][y - i] == board.board[x][y + 5 - i])
        /*
        if(y - i >= 0 &&
                y + 5 - i <= 18 &&)
                */
        {
            if(board.board[x][y] == BLACK)
            {
                who_win = BLACK;
            }
            else if(board.board[x][y] == WHITE)
            {
                who_win = WHITE;
            }
            return 1;
        }
    }
    return 0;
}

int MainWindow::direction2(int x, int y)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if(x - i >= 0 &&
           x + 5 - i <= BOARD_SIZE - 1 &&
           board.board[x - i][y] == board.board[x + 1 - i][y] &&
           board.board[x - i][y] == board.board[x + 2 - i][y] &&
           board.board[x - i][y] == board.board[x + 3 - i][y] &&
           board.board[x - i][y] == board.board[x + 4 - i][y] &&
           board.board[x - i][y] == board.board[x + 5 - i][y])
        {
            if(board.board[x][y] == BLACK)
                who_win = BLACK;
            else if(board.board[x][y] == WHITE)
                who_win = WHITE;
            return 1;
        }
    }
    return 0;
}

int MainWindow::direction3(int x, int y)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if(x - i >= 0 &&
           y - i >= 0 &&
           x + 5 - i <= BOARD_SIZE - 1 &&
           y + 5 - i <= BOARD_SIZE - 1 &&
           board.board[x - i][y - i] == board.board[x + 1 - i][y + 1 - i] &&
           board.board[x - i][y - i] == board.board[x + 2 - i][y + 2 - i] &&
           board.board[x - i][y - i] == board.board[x + 3 - i][y + 3 - i] &&
           board.board[x - i][y - i] == board.board[x + 4 - i][y + 4 - i] &&
           board.board[x - i][y - i] == board.board[x + 5 - i][y + 5 - i])
        {
           if(board.board[x][y] == BLACK)
               who_win = BLACK;
           else if(board.board[x][y] == WHITE)
               who_win = WHITE;
           return 1;
        }
    }
    return 0;
}

int MainWindow::direction4(int x, int y)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if(x + i <= BOARD_SIZE - 1 &&
           y - i >= 0 &&
           x - 5 + i >= 0 &&
           y + 5 - i <= BOARD_SIZE - 1 &&
           board.board[x + i][y - i] == board.board[x - 1 + i][y + 1 - i] &&
           board.board[x + i][y - i] == board.board[x - 2 + i][y + 2 - i] &&
           board.board[x + i][y - i] == board.board[x - 3 + i][y + 3 - i] &&
           board.board[x + i][y - i] == board.board[x - 4 + i][y + 4 - i] &&
           board.board[x + i][y - i] == board.board[x - 5 + i][y + 5 - i])
        {
            if(board.board[x][y] == BLACK)
                who_win = BLACK;
            else if(board.board[x][y] == WHITE)
                who_win = WHITE;
            return 1;
        }
    }
    return 0;
}


//棋谱保存槽函数；
void MainWindow::sava_date()
{
    QString save_filename = QFileDialog::getSaveFileName(this,
        tr("Save"),
        "a.txt",
        tr("*.txt;; *.bat;; *.cpp")); //选择路径
    if(save_filename.isEmpty())
    {
        return;
    }
//    ofstream outfile(save_filename.toLatin1().data(),ios::out);
    char *ch = save_filename.toLocal8Bit().data();
    std::ofstream outfile(ch,std::ios::out);
    outfile << board.step_num << endl;
    for(int i = 0; i < board.step_num; i++)
    {
        outfile << board.move_history[i].b << " " << board.move_history[i].a << endl;
    }
    outfile.close();
    return ;
}

//文件读取；
void MainWindow::open_file()
{
    file_full = QFileDialog::getOpenFileName(this,tr("打开"),".",tr("Text Files(*.txt)"));

    fi = QFileInfo(file_full);
    file_name = fi.fileName();
    file_path = fi.absolutePath();
    Path_choise->setText(file_path+""+file_name);
}

//棋盘显示；
void MainWindow::read_file()
{
        init();
        QFile file(file_full);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
             QMessageBox::warning(this, tr("Application"),
                                     tr("Cannot read file %1:\n%2.")
                                  .arg(file_full)
                                     .arg(file.errorString()));
             return;
        }
        QTextStream in(&file);
        Content_show->setText(in.readAll());

/*
        int data_1,data_2;
        for(int i = 0; i < board.step_num; i++)
        {
             data_1 = in.read(1);
             board.move_history[i].a = data_1;
             data_2 = in.read(2);
             board.move_history[i].b = data_2;
        }
        file.close();
        chessmode = REGRET_PLAY;
        pristep_num = 0;
        for(int i=0;i<board.step_num;i++)
        {
            board.board[board.move_history[i].a][board.move_history[i].b]=board.which_to_move[i];
        }
        repaint();
        chessmode = MAN_MAN;

 */
        // Content_show->setPlainText(in.readAll());

        /*
        std::fstream infile(file_full.toLocal8Bit().data(),std::ios::out | std::ios::in);
        infile >> board.step_num;
        for(int i = 0; i < board.step_num; i++)
        {
            infile >> board.move_history[i].b >> board.move_history[i].a;
        }
        file.close();

        chessmode = REGRET_PLAY;
        pristep_num = 0;
        for(int i=0;i<board.step_num;i++)
        {
            board.board[board.move_history[i].a][board.move_history[i].b]=board.which_to_move[i];
        }
        repaint();
        chessmode = MAN_MAN;*/
        return ;
}


//下面两个函数有一定问题。相同代码放在另一个程序无警告正常运行，但是在本程序中会有一定程度上报错。
//此函数不再打开棋谱文本文件，直接由上方两个函数关联打开的文本文件读入数据并刷新界面；
/*
void MainWindow::show_file()
{
    init();
    QString filename = file_full;
    if(filename.isEmpty())
    {
        return;
    }

    std::fstream infile(filename.toLocal8Bit().data(), std::ios::out | std::ios::in);
    infile >> board.step_num;
    for(int i = 0; i < board.step_num; i++)
    {
        infile >> board.move_history[i].b >> board.move_history[i].a;
    }
    infile.close();
    chessmode = REGRET_PLAY;
    pristep_num = 0;
    for(int i = 0; i < board.step_num; i++)
    {
        board.board[board.move_history[i].a][board.move_history[i].b]=board.which_to_move[i];
    }
//    chessmode = tmp;
   // repaint();
    update();
    chessmode = MAN_MAN;
    return ;
}
*/

//此函数另外打开棋谱文本文件然后读取并刷新界面。
void MainWindow::show_file()
{
    init();
    QString filename = QFileDialog::getOpenFileName(this,"请选择棋谱","/","棋谱文件(*.txt)");
    if(filename.isEmpty())
    {
        return;
    }

    std::fstream infile(filename.toLocal8Bit().data(), std::ios::out | std::ios::in);
    infile >> board.step_num;
    for(int i = 0; i < board.step_num; i++)
    {
        infile >> board.move_history[i].b >> board.move_history[i].a;
    }
    infile.close();
    chessmode = REGRET_PLAY;
    pristep_num = 0;
    for(int i = 0; i < board.step_num; i++)
    {
        board.board[board.move_history[i].a][board.move_history[i].b] = board.which_to_move[i];
    }

//    chessmode=tmp;
    repaint();
    chessmode = MAN_MAN;
    return ;
}



