#include "GUI/glavniprozor.h"
#include <QApplication>
#include <QSplashScreen>
#include <unistd.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/images/SplashScreen.png"));
    splash->show();
    GlavniProzor *w = new GlavniProzor();
    a.processEvents();
    sleep(2);
    w->show();
    splash->finish(w);
    delete splash;


    return a.exec();
}
