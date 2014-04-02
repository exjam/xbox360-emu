#include "mainwindow.h"
#include "disassembleview.h"

#include <QQmlContext>
#include <fstream>

DisassembleView *m_disassembleView;
xex::Binary binary;

MainWindow::MainWindow(QQuickWindow *window)
    : m_window(window)
{
   /* Totally legal copy of the game. */
   std::ifstream file;
   file.open(L"D:\\Downloads\\Ultimate_Marvel_vs_Capcom_3_RF_XBOX360\\Default.xex", std::ifstream::in | std::ifstream::binary);
   m_loader.load(file, binary);
   file.close();

   m_disassembleView = new DisassembleView();
   m_disassembleView->setStartAddress(0x82000400);
   m_disassembleView->setEndAddress(0x8318ee08);
}

MainWindow::~MainWindow()
{

}

void MainWindow::show()
{
   m_window->show();
}

void MainWindow::setContext(QQmlContext *context)
{
   m_qmlContext = context;
   m_qmlContext->setContextProperty("disassembleView", m_disassembleView);
}