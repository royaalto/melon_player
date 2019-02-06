/**
 * @file main.cpp
 * @author Linming Pan
 * @date 12.11.2018
 * @brief Example code to run simple QT
 *
 */


#include <QApplication>
#include "main_window.h"
#include "iostream"


int main(int argc, char *argv[])
{
    // Start QT Application
    QApplication app(argc, argv);

    // Setup out example class
    MainWindow *win = new MainWindow();
    if(!win->init())
    {   
        std::cout << "init failed" << std::endl;
        return -1;
    }
    win->show();
    return app.exec();
}
