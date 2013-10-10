#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtQuick/QQuickWindow>
#include "xex/xex.h"

class QQmlContext;

class MainWindow : public QObject
{
    Q_OBJECT

public:
    MainWindow(QQuickWindow *window);
    ~MainWindow();

    void setContext(QQmlContext *context);
    void show();

private:
   QQuickWindow *m_window;
   QQmlContext *m_qmlContext;
   xex::Loader m_loader;
};

#endif // MAINWINDOW_H
