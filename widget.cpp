#include "widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent): QWidget(parent)
{
    setMinimumSize(800, 300);
    qApp->setStyle(QStyleFactory::create("Fusion"));

       QPalette darkPalette;
       darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
       darkPalette.setColor(QPalette::WindowText, Qt::white);
       darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
       darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
       darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
       darkPalette.setColor(QPalette::ToolTipText, Qt::white);
       darkPalette.setColor(QPalette::Text, Qt::white);
       darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
       darkPalette.setColor(QPalette::ButtonText, Qt::white);
       darkPalette.setColor(QPalette::BrightText, Qt::red);
       darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
       darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
       darkPalette.setColor(QPalette::HighlightedText, Qt::white);
       qApp->setPalette(darkPalette);

    tableWidget = new QTableWidget(this);

    pickFiles = new QPushButton(this);
    pickFiles->setMinimumHeight(35);
    pickFiles->setMaximumWidth(200);
    pickFiles->setText("Pick Files");
    clearTable = new QPushButton(this);
    clearTable->setMinimumHeight(35);
    clearTable->setMaximumWidth(200);
    clearTable->setText("Clear table");
    exitButton = new QPushButton(this);
    exitButton->setMinimumHeight(35);
    exitButton->setMaximumWidth(200);
    exitButton->setText("Exit");
    QFont font;
    font.setBold(true);
    pickFiles->setFont(font);
    clearTable->setFont(font);
    exitButton->setFont(font);

    grid = new QGridLayout(this);
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(pickFiles);
    buttons->addWidget(clearTable);
    buttons->addWidget(exitButton);
    grid->addWidget(tableWidget, 0, 0, 3, 3);
    grid->addWidget(pickFiles, 3, 0);
    grid->addWidget(clearTable, 3, 1);
    grid->addWidget(exitButton, 3, 2);



    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    QHeaderView* header = tableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    connect(pickFiles, &QPushButton::clicked, this, &Widget::getFiles);
    connect(exitButton, &QPushButton::clicked, this, &Widget::exitApp);
    connect(clearTable, &QPushButton::clicked, this, &Widget::clearRow);



    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
    tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Extension"));
    tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Size (bits)"));
    tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Size (pixels)"));
    tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Deepth"));
    tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem("Resolution (dpi)"));
    tableWidget->setHorizontalHeaderItem(6, new QTableWidgetItem("Compression"));


    currentPath = "D:/";
    filters << "*.jpg" << "*.gif" << "*.tif" << "*.bmp" << "*.png" << "*.pcx" << "*.jpeg";
    dir.setNameFilters(filters);
}


void Widget::readAndSetInfo(QFileInfo fileInfo)
{
    QImageWriter inf;
    inf.setFileName(fileInfo.fileName());
    image.load(currentPath + "/" + fileInfo.fileName());

    QString resolution = QVariant(image.width()).toString()+ "x" +QVariant(image.height()).toString();
    tableWidget->insertRow(tableWidget->rowCount());
    tableWidget->setItem(tableWidget->rowCount() - 1, 0, new QTableWidgetItem(fileInfo.fileName()));
    tableWidget->setItem(tableWidget->rowCount() - 1, 1, new QTableWidgetItem(fileInfo.suffix().toUpper()));

    QTableWidgetItem *item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, fileInfo.size());
    tableWidget->setItem(tableWidget->rowCount() - 1, 2, item);
    tableWidget->setItem(tableWidget->rowCount() - 1, 3, new QTableWidgetItem(resolution));
    tableWidget->setItem(tableWidget->rowCount() - 1, 4, new QTableWidgetItem(QVariant(image.bitPlaneCount()).toString()));

    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, round(image.dotsPerMeterX()/39.37));
    tableWidget->setItem(tableWidget->rowCount() - 1, 5, item);
    tableWidget->setItem(tableWidget->rowCount() - 1, 5, item);
    tableWidget->setItem(tableWidget->rowCount() - 1,6,new QTableWidgetItem(QVariant(inf.compression()).toString()));


}

void Widget::clearRow()
{
    tableWidget->clear();
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
    tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Extension"));
    tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Size (bits)"));
    tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Size (pixels)"));
    tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Deepth"));
    tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem("Resolution (dpi)"));
    tableWidget->setHorizontalHeaderItem(6, new QTableWidgetItem("Compression"));
}

void Widget::exitApp()
{
    close();
}

void Widget::getFiles()
{
    QStringList fileName = QFileDialog ::getOpenFileNames(this, "Open Files", currentPath,
                                                          "Image(*jpg *gif *tif *bmp *png *pcx *jpeg);;Archieve(*zip)");
    if(fileName.isEmpty())
        return;

    tableWidget->setSortingEnabled(false);

    if (fileName[0].contains("zip"))
    {
        tempPath = QDir::currentPath() + "/temp";
        dir.setPath(tempPath);
        dir.mkdir(tempPath);

        fileInfo.setFile(fileName[0]);
        currentPath = fileInfo.path();

        QZipReader zipReader(fileName[0]);
        zipReader.extractAll(tempPath);

        QFileInfoList files = dir.entryInfoList();
        if (files.empty())
        {
            QMessageBox::information(this, "Information", "There are no files in the archive in the correct format");
            return;
        }

        fileInfo = files[0];
        QString saveCurrPath = currentPath;
        currentPath = fileInfo.path();

        for (int i = 0; i < files.size(); i++)
        {
            fileInfo = files[i];
            readAndSetInfo(fileInfo);
        }

        dir.removeRecursively();
        currentPath = saveCurrPath;
    }

    else
    {
        fileInfo.setFile(fileName[0]);
        currentPath = fileInfo.path();

        for (int i = 0; i < fileName.size(); i++)
        {
            fileInfo.setFile(fileName[i]);
            readAndSetInfo(fileInfo);
        }
    }

    tableWidget->sortByColumn(0,Qt::AscendingOrder);
    tableWidget->setSortingEnabled(true);
}

Widget::~Widget()
{
}
