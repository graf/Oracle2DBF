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
    return true;
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
    QDate tillDate = ui->tillDateEdit->date();
    int good = 0;
    int bad = 0;
    int unknown = 0;
    myCallsDbfTable.seek(0);
    while (myCallsDbfTable.next()) {
        ui->progressBar->setValue(progress++);
        QDbf::QDbfRecord currentRecord = myCallsDbfTable.record();
        const QDate date = currentRecord.value(trUtf8("DATE")).toDate();
        if (date < fromDate || date > tillDate)
            continue;
        const QString rawNumber = currentRecord.value(trUtf8("NUMBER")).toString();
        QString number = formatPhone(rawNumber);
        if (number.isEmpty() || number.length() < 7) {
            bad++;
            continue;
        }
        QString name = myPhoneBook[number].join(trUtf8("; "));
        if (name.isEmpty()) {
            unknown++;
            continue;
        }

        good++;
        int accountCode = myExtnamesList.indexOf(name);
        if (accountCode == -1) {
            if (!myExtnamesDbfTable.addRecord())
                continue;
            //myExtnamesDbfTable.next();
            QDbf::QDbfRecord newRecord = myExtnamesDbfTable.record();
            accountCode = myExtnamesList.count();
            newRecord.setValue(trUtf8("ACCNTCODE"), accountCode);
            newRecord.setValue(trUtf8("NAME"), name);
            myExtnamesList.append(name);
        }
        currentRecord.setValue(trUtf8("ACCOUNT"), accountCode);
    }
    QMessageBox::information(NULL,
                             trUtf8("Завершено"),
                             trUtf8("Обработка завершена. Обработано номеров:\n"
                                    "\tизвестных - %1\n"
                                    "\tнеизвестных - %2\n"
                                    "\tневерный формат - %3")
                             .arg(QString::number(good))
                             .arg(QString::number(bad))
                             .arg(QString::number(unknown)));
}

bool MainWindow::readPhoneBook()
{
    QSettings mySettings(trUtf8("orajdbf.ini"), QSettings::IniFormat);
    QString oraDnsName = mySettings.value(trUtf8("ORACLE/dnsname"), trUtf8("orajdbf")).toString();
    QString oraUser = mySettings.value(trUtf8("ORACLE/user"), trUtf8("orajdbf")).toString();
    QString oraPassword = mySettings.value(trUtf8("ORACLE/password"), trUtf8("orajdbf")).toString();
    QString agentsTableName = mySettings.value(trUtf8("ORACLE/agents"), trUtf8("AGNLIST")).toString();
    QString pAgentsTableNames = mySettings.value(trUtf8("ORACLE/pagents"), trUtf8("PAGNLIST")).toString();
    QString agentNameField = mySettings.value(trUtf8("ORACLE/agentNameField"), trUtf8("NAME")).toString();
    QString pAgentNameField = mySettings.value(trUtf8("ORACLE/pAgentNameField"), trUtf8("NAME")).toString();
    QStringList agentPhoneFields;
//    agentPhoneFields.append(trUtf8("PHONE2"));
//    agentPhoneFields.append(trUtf8("PHONE"));
//    agentPhoneFields.append(trUtf8("FOX"));
//    mySettings.setValue(trUtf8("ORACLE/agentPhoneFields"), agentPhoneFields);
    agentPhoneFields = mySettings.value(trUtf8("ORACLE/agentPhoneFields"), trUtf8("AGNNAME, PHONE, FAX").split(trUtf8(", "))).toStringList();
    QStringList pAgentPhoneFields = mySettings.value(trUtf8("ORACLE/pAgentPhoneFields"), trUtf8("PHONE")).toStringList();

    QSqlDatabase oraDb = QSqlDatabase::addDatabase(trUtf8("QODBC"));
    oraDb.setDatabaseName(oraDnsName);
    oraDb.setUserName(oraUser);
    oraDb.setPassword(oraPassword);
    if (!oraDb.open()) {
        QMessageBox::critical(NULL, trUtf8("Ошибка"), trUtf8("Ошибка соединения с ORACLE. ") + oraDb.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare(trUtf8("select * from ") + agentsTableName);
    if (!query.exec()) {
        oraDb.close();
        QMessageBox::critical(NULL, trUtf8("Ошибка"), trUtf8("Ошибка получения списка клиентов. ") + oraDb.lastError().text());
        return false;
    }

    while (query.next()) {
        QSqlRecord currentAgent = query.record();
        QString name = currentAgent.value(agentNameField).toString();
        foreach (QString field, agentPhoneFields) {
            const QString rawPhone = currentAgent.value(field).toString();
            QString phone = formatPhone(rawPhone);
            myPhoneBook[phone].append(name);
        }
    }

    query.prepare(trUtf8("select * from ") + pAgentsTableNames);
    if (!query.exec()) {
        oraDb.close();
        QMessageBox::critical(NULL, trUtf8("Ошибка"), oraDb.lastError().text());
        return false;
    }

    while (query.next()) {
        QSqlRecord currentAgent = query.record();
        QString name = trUtf8("(П) ") + currentAgent.value(pAgentNameField).toString();
        foreach (QString field, pAgentPhoneFields) {
            const QString rawPhone = currentAgent.value(field).toString();
            QString phone = formatPhone(rawPhone);
            myPhoneBook[phone].append(name);
        }
    }
}

QString MainWindow::formatPhone(const QString &number) const
{
    QString result = number;
    result.remove(QRegExp(trUtf8(",.*$")));
    result.remove(QRegExp(trUtf8("\\D")));
    result.remove(QRegExp(trUtf8("^([78](846)?)")));
    return result;
}

int MainWindow::fillTestDb()
{
    int progress = 1;
    QSettings mySettings(trUtf8("orajdbf.ini"), QSettings::IniFormat);
    QString oraDnsName = mySettings.value(trUtf8("ORACLE/dnsname"), trUtf8("orajdbf")).toString();
    QString oraUser = mySettings.value(trUtf8("ORACLE/user"), trUtf8("orajdbf")).toString();
    QString oraPassword = mySettings.value(trUtf8("ORACLE/password"), trUtf8("orajdbf")).toString();
    QString agentsTableName = mySettings.value(trUtf8("ORACLE/agents"), trUtf8("AGNLIST")).toString();
    QString pAgentsTableNames = mySettings.value(trUtf8("ORACLE/pagents"), trUtf8("PAGNLIST")).toString();
    QString agentNameField = mySettings.value(trUtf8("ORACLE/agentNameField"), trUtf8("NAME")).toString();
    QString pAgentNameField = mySettings.value(trUtf8("ORACLE/pAgentNameField"), trUtf8("NAME")).toString();
    QStringList agentsPhoneFields = mySettings.value(trUtf8("ORACLE/agentsPhoneFields"), trUtf8("AGNNAME,PHONE,FAX")).toStringList();
    QStringList pAgentPhoneFields = mySettings.value(trUtf8("ORACLE/pAgentPhoneFields"), trUtf8("PHONE")).toStringList();

    QSqlDatabase oraDb = QSqlDatabase::addDatabase(trUtf8("QODBC"));
    oraDb.setDatabaseName(oraDnsName);
    oraDb.setUserName(oraUser);
    oraDb.setPassword(oraPassword);
    if (!oraDb.open()) {
        QMessageBox::critical(NULL, trUtf8("Ошибка"), trUtf8("Ошибка соединения с ORACLE. ") + oraDb.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare(trUtf8("insert into ") + agentsTableName + trUtf8(" values (:NAME, :PHONE, :PHONE2, :FAX)"));
    while (myCallsDbfTable.next()) {
        ui->progressBar->setValue(progress++);
        QDbf::QDbfRecord currentRecord = myCallsDbfTable.record();
        const QString rawPhone = currentRecord.value(trUtf8("NUMBER")).toString();
        QString phone = formatPhone(rawPhone);
        if (phone.isEmpty() || phone.length() < 7)
            continue;
        if (!myCallsDbfTable.next())
            continue;
        currentRecord = myCallsDbfTable.record();
        const QString rawPhone2 = currentRecord.value(trUtf8("NUMBER")).toString();
        QString phone2 = formatPhone(rawPhone2);
        if (phone2.isEmpty() || phone2.length() < 7)
            continue;
        if (!myCallsDbfTable.next())
            continue;
        currentRecord = myCallsDbfTable.record();
        const QString rawFax = currentRecord.value(trUtf8("NUMBER")).toString();
        QString fax = formatPhone(rawFax);
        if (fax.isEmpty() || fax.length() < 7)
            continue;
        QString name = trUtf8("Клиент") + QString::number(progress);

        query.bindValue(trUtf8(":NAME"), name);
        query.bindValue(trUtf8(":PHONE"), phone);
        query.bindValue(trUtf8(":PHONE2"), phone2);
        query.bindValue(trUtf8(":FAX"), fax);
        if (!query.exec()) {
            oraDb.close();
            QMessageBox::critical(NULL, trUtf8("Ошибка"), trUtf8("Ошибка в запросе. ") + oraDb.lastError().text());
            return false;
        }
    }
    oraDb.close();
}
