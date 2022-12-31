#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <qzipreader_p.h>
#include <qzipwriter_p.h>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    QTableWidget *tableWidget;
    QPushButton *pickFiles;
    QPushButton *clearTable;
    QPushButton *exitButton;
    QGridLayout *grid;
    QHBoxLayout *buttons;

    bool firstFile;
    QStringList suffixes;
    QString currentPath;

    QFileInfo fileInfo;
    QImage image;

    QDir dir;
    QString tempPath;
    QStringList filters;

    void setHeaders();
    void readAndSetInfo(QFileInfo fileInfo);

private slots:
    void getFiles();
    void exitApp();
    void clearRow();
};
#endif // WIDGET_H
