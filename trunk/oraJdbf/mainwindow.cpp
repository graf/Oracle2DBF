#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDate currDate = QDate::currentDate();
    int currDay = currDate.day();
    int curMonth = currDate.month();
    int currYear = currDate.year();
    QDate fromDate;
    fromDate.setDate(currYear, curMonth-1, 1);
    QDate tillDate;
    tillDate = fromDate.addMonths(1).addDays(-1);
    ui->tillDateEdit->setDate(tillDate);
    ui->fromDateEdit->setDate(fromDate);
    readPhoneBook();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fromCalendarWidget_clicked(const QDate &date)
{
    ui->fromDateEdit->setDate(date);
}

void MainWindow::on_fromDateEdit_dateChanged(const QDate &date)
{
    ui->fromCalendarWidget->setSelectedDate(date);
}

void MainWindow::on_tillCalendarWidget_clicked(const QDate &date)
{
    ui->tillDateEdit->setDate(date);
}

void MainWindow::on_tillDateEdit_dateChanged(const QDate &date)
{
    ui->tillCalendarWidget->setSelectedDate(date);
}

bool MainWindow::openFile()
{
    const QString caption = trUtf8("Выберите файл");
    const QString filter = trUtf8("xBase files (*.dbf)");
    const QString filePath = QFileDialog::getOpenFileName(NULL, caption, QDir::currentPath(), filter);

    if (filePath.isNull()) {
        return false;
    }

    if (!myCallsDbfTable.open(filePath, QDbf::QDbfTable::ReadWrite)) {
        const QString title = trUtf8("Ошибка открытия файла");
        const QString text = QString(trUtf8("Невозможно открыть файл %1")).arg(filePath);
        QMessageBox::warning(NULL, title, text, QMessageBox::Ok);
        return false;
    }

    ui->dbfFilePathLabel->setText(trUtf8("Выберите диапазон дат и нажмите \"Запуск\""));


//    myDbfTable.next();
//    QDbf::QDbfRecord rec = myDbfTable.record();
//    rec.setValue(trUtf8("STATUS"), trUtf8("qwerty"));

//    myDbfTable.addRecord(rec);

//    myDbfTable.close();
    return true;
}

bool MainWindow::createSqlLiteDb()
{

}

void MainWindow::on_openDbfButton_clicked()
{
    ui->processButton->setEnabled(openFile());
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(myCallsDbfTable.size());
}

void MainWindow::on_processButton_clicked()
{
    int progress = 1;
    QDate fromDate = ui->fromDateEdit->date();
    QDate tillDate = ui->fromDateEdit->date();
    while (myCallsDbfTable.next()) {
        ui->progressBar->setValue(progress++);
        QDbf::QDbfRecord currentRecord = myCallsDbfTable.record();
        QDate date = QDate::fromString(currentRecord.value(trUtf8("DATE")).toString(), trUtf8("dd.MM.yyyy"));
        if (date < fromDate || date > tillDate)
            continue;
        bool isRealDigit = false;
        //QString rawNumber = currentRecord.value(trUtf8("NUMBER")).toString();
        quint64 number = currentRecord.value(trUtf8("NUMBER")).toUInt(&isRealDigit);
        if (!isRealDigit)
            continue;
        QString name = myPhoneBook[number];
        int accountCode = myExtnamesList.indexOf(name);
        if (accountCode == -1) {
            if (!myExtnamesDbfTable.addRecord())
                continue;
            myExtnamesDbfTable.next();
            QDbf::QDbfRecord newRecord = myExtnamesDbfTable.record();
            accountCode = myExtnamesList.count();
            newRecord.setValue(trUtf8("ACCNTCODE"), accountCode);
            newRecord.setValue(trUtf8("NAME"), name);
            myExtnamesList.append(name);
        }
        currentRecord.setValue(trUtf8("ACCOUNT"), accountCode);
    }
}

bool MainWindow::readPhoneBook()
{
    QSqlDatabase oraDb = QSqlDatabase::addDatabase(trUtf8("QODBC"));
    oraDb.setDatabaseName(trUtf8("test"));
    oraDb.setPassword(trUtf8("orajdbf"));
    oraDb.setUserName(trUtf8("orajdbf"));
    bool ok = oraDb.open();
    qDebug() << ok;
}
