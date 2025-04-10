#include "mainwindow.h"

#include <QApplication>

#ifdef DEBUG_COMP

#include <csignal>
#include <iostream>

extern "C" void sig_segv_hndl(int sig)
{
    std::cerr << "SIGSEGV" << "\n";
    std::exit(sig);
}

extern "C" void sig_fpe_hndl(int sig)
{
    std::cerr << "SIGFPE" << "\n";
    std::exit(sig);
}

#endif


int main(int argc, char *argv[])
{
    #ifdef DEBUG_COMP

    std::signal(SIGSEGV, sig_segv_hndl);
    std::signal(SIGFPE, sig_fpe_hndl);

    #endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
