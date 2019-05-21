#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    i++;
    QPainter painter(this);
    painter.setPen(QPen(Qt::red));
    if(i%2==0)
    {
        painter.drawLine(1,1,150,200);
    }
    else
    {
        painter.drawLine(10,10,100,100);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pressPos = event->pos();
    if(pressPos.x()>0&&pressPos.y()>0)
    {
        this->update();
    }
}
