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
    QDbf::QDbfTable myCallsTable;
    QDbf::QDbfTable myExtnamesTable;
    QStringList myExtnamesList;
    QHash<QString, QStringList> myPhoneBook;

    bool readPhoneBook();
    QString formatPhone(const QString &number) const;

    int fillTestDb();

    void test();
};

#endif // MAINWINDOW_H
