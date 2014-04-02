#include "mainwindow.h"
#include "ppc/disassembler.h"

#include <QtQml/5.1.0/QtQml/private/qqmlchangeset_p.h>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QtQml>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   qRegisterMetaType<QQmlChangeSet>("QQmlChangeSet");
   
   QQmlApplicationEngine engine("Resources/MainWindow.qml");
   MainWindow *window = new MainWindow(qobject_cast<QQuickWindow *>(engine.rootObjects().value(0)));
   
   if(!window) {
      qWarning("Error: The root item must be MainWindow");
      return -1;
   }
      
   QQmlContext *context = engine.rootContext();
   window->setContext(context);
   window->show();
   return app.exec();
}
