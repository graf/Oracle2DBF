#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Example {
namespace Internal {

class MainWindowPrivate;

} // namespace Internal


class MainWindowExample : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowExample(QWidget *parent = 0);
    ~MainWindowExample();

private:
    Internal::MainWindowPrivate *const d;

public slots:
    void openFile();

    friend class Internal::MainWindowPrivate;
};

} // namespace Example

#endif // MAINWINDOW_H
