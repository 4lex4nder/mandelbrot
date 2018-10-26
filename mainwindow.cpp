#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this->ui->widget, SIGNAL(positionCoordsChanged(QString, QString)),
            this, SLOT(setPositionCoords(QString, QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPositionCoords(const QString& real, const QString& imag) {
    this->ui->real->setText(real);
    this->ui->imag->setText(imag);
}
