#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "qdbftablemodel.h"
#include "qdbfrecord.h"

namespace Ui {
    class MainWindow;
}



class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool openFile();

private slots:
    void on_fromCalendarWidget_clicked(const QDate &date);
    void on_fromDateEdit_dateChanged(const QDate &date);
    void on_tillCalendarWidget_clicked(const QDate &date);
    void on_tillDateEdit_dateChanged(const QDate &date);
    void on_openDbfButton_clicked();
    void on_processButton_clicked();

private:
    Ui::MainWindow *ui;
    QDbf::QDbfTable myCallsDbfTable;
    QDbf::QDbfTable myExtnamesDbfTable;
    QStringList myExtnamesList;
    QHash<quint64, QString> myPhoneBook;
    bool createSqlLiteDb();
    bool readPhoneBook();
};

#endif // MAINWINDOW_H
