#include "mainwindow.h"

#include "qdbftablemodel.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>

const char * const SELECT_FILE_TEXT = "Select dbf file by clicking on the button ----->";

namespace Example {
namespace Internal {

class MainWindowPrivate
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindowPrivate();
    ~MainWindowPrivate();

    void init();
    void openFile();

    MainWindowExample *q;
    QWidget *const m_centralWidget;
    QVBoxLayout *const m_baseLayout;
    QHBoxLayout *const m_fileLocationLayout;
    QLineEdit *const m_fileLocationEditor;
    QPushButton *const m_selectFileButton;
    QTableView *const m_tableView;
    QDir m_dir;
    QDbf::QDbfTableModel *const m_model;
};

} // namespace Internal
} // namespace Example

using namespace Example;
using namespace Example::Internal;

MainWindowPrivate::MainWindowPrivate() :
    q(0),
    m_centralWidget(new QWidget()),
    m_baseLayout(new QVBoxLayout(m_centralWidget)),
    m_fileLocationLayout(new QHBoxLayout()),
    m_fileLocationEditor(new QLineEdit()),
    m_selectFileButton(new QPushButton(QString::fromLatin1("..."), 0)),
    m_tableView(new QTableView()),
    m_dir(QCoreApplication::applicationDirPath()),
    m_model(new QDbf::QDbfTableModel())
{
}

MainWindowPrivate::~MainWindowPrivate()
{
    delete m_model;
    delete m_tableView;
    delete m_selectFileButton;
    delete m_fileLocationEditor;
    delete m_fileLocationLayout;
    delete m_baseLayout;
    delete m_centralWidget;
}

void MainWindowPrivate::init()
{
    q->setCentralWidget(m_centralWidget);

    m_baseLayout->setContentsMargins(0, 0, 0, 0);

    m_fileLocationLayout->setContentsMargins(2, 2, 2, 2);
    m_baseLayout->addLayout(m_fileLocationLayout);

    m_fileLocationEditor->setReadOnly(true);
    m_fileLocationEditor->setText(trUtf8(SELECT_FILE_TEXT));
    m_fileLocationLayout->addWidget(m_fileLocationEditor);

    m_selectFileButton->setFixedWidth(30);
    m_selectFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QObject::connect(m_selectFileButton, SIGNAL(clicked()), q, SLOT(openFile()));
    m_fileLocationLayout->addWidget(m_selectFileButton);

    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_baseLayout->addWidget(m_tableView);
}

void MainWindowPrivate::openFile()
{
    const QString caption = QLatin1String("Select file");
    const QString filter = QLatin1String("xBase files (*.dbf)");
    const QString filePath = QFileDialog::getOpenFileName(q, caption, m_dir.absolutePath(), filter);

    if (filePath.isNull()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    m_dir = fileInfo.dir();
    m_fileLocationEditor->setText(filePath);

    m_tableView->setModel(0);

    if (!m_model->open(filePath)) {
        const QString title = QLatin1String("Open file error");
        const QString text = QString(QLatin1String("Can not open file %1")).arg(filePath);
        QMessageBox::warning(q, title, text, QMessageBox::Ok);
        m_fileLocationEditor->setText(QLatin1String(SELECT_FILE_TEXT));
        return;
    }

    m_tableView->setModel(m_model);
}

MainWindowExample::MainWindowExample(QWidget *parent) :
    QMainWindow(parent),
    d(new MainWindowPrivate())
{
    d->q = this;
    d->init();
}

MainWindowExample::~MainWindowExample()
{
    delete d;
}

void MainWindowExample::openFile()
{
    d->openFile();
}
