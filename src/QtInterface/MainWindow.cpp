#include "MainWindow.h"
#include "FileListModel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QSplitter>
#include <QListView>
#include <QStringListModel>
#include <QDesktopServices>


MainWindow::MainWindow(AppCore& core,QWidget* parent) :
    QMainWindow(parent),
    m_core(core)
{
    setUI();

    connect(m_backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_addressBar, &QLineEdit::returnPressed,this, &MainWindow::onAddressReturnPressed);
    connect(m_fileView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
}

void MainWindow::updateView(const NavigationState& state)
{
    m_addressBar->setText(QString::fromStdString(state.GetCurrentPath().string()));

    if(m_model)
    {
        m_model->updateData(state.GetCurrentFiles());
    }

}

void MainWindow::onBackClicked()
{
    m_core.GoUp();
}

void MainWindow::onAddressReturnPressed()
{
    std::string pathStr = m_addressBar->text().toStdString();
    m_core.Navigate(pathStr);
}

void MainWindow::onFileDoubleClicked(const QModelIndex& index)
{
    if(m_core.GetState().GetCurrentFiles()[index.row()].IsDirectory())
    {
        m_core.EnterDirectory(index.row());
    }else if(m_core.GetState().GetCurrentFiles()[index.row()].GetPath().extension() == ".txt")
    {
        m_core.ExecuteFile(index.row());
    }else
    {
        QString qPath = QString::fromStdString(m_core.GetState().GetCurrentFiles()[index.row()].GetPath().string());
        
        QDesktopServices::openUrl(QUrl::fromLocalFile(qPath));
    
    }
    
}




void MainWindow::setUI()
{
    setWindowTitle("FileManager");
    resize(1080, 550);

    // Центральный виджет
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);


    // Главный лайаут
    QVBoxLayout* m_mainLayout = new QVBoxLayout(centralWidget);
    

    // Убираем отступы
    m_mainLayout->setContentsMargins(0, 0, 0, 0); 


    //  Сплиттер(Разделитель)
    m_splitter = new QSplitter(Qt::Horizontal, centralWidget);
    

    // Убираем зазор между панелями (тонкая линия)
    m_splitter->setHandleWidth(1); // Тонкая линия в 1 пиксель
    m_splitter->setStyleSheet("QSplitter::handle { background-color: #cccccc; }");
    

    // Добавляем Сплиттер в главный лайаут
    m_mainLayout->addWidget(m_splitter);


    // ЛЕВАЯ ЧАСТЬ (Сайдбар)
    m_sideBar = new QListView(); 
    m_sideBar->setFrameShape(QFrame::NoFrame); // Убираем наружную рамку
    m_splitter->addWidget(m_sideBar);


    // ПРАВАЯ ЧАСТЬ 
    m_rightContainer = new QWidget();
    m_rightLayout = new QVBoxLayout(m_rightContainer);
    

    // Убираем отступы 
    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->setSpacing(0); //Растояние между элементами 0 пикселей


    // ВЕРХНЯЯ ПАНЕЛЬ (Toolbar)
    QWidget* topBarContainer = new QWidget();
    topBarContainer->setStyleSheet("background-color:  white; border-bottom: 1px solid #ddd;");
    topBarContainer->setFixedHeight(50); // Фиксированная высота


    QHBoxLayout* m_topBarLayout = new QHBoxLayout(topBarContainer);
    m_topBarLayout->setContentsMargins(10, 5, 10, 5); //внутренние отступы
    m_topBarLayout->setSpacing(10); // Расстояние между кнопками 10 пикселей


    m_backBtn = new QPushButton();
    m_backBtn->setIcon(QIcon(":/res/back.svg"));
    m_backBtn->setFlat(true); // Плоская кнопка без рамок
    m_topBarLayout->addWidget(m_backBtn);


    m_forwardBtn = new QPushButton();
    m_forwardBtn->setIcon(QIcon(":/res/forward.svg"));
    m_forwardBtn->setFlat(true);
    m_topBarLayout->addWidget(m_forwardBtn);


    m_addressBar = new QLineEdit();
    m_addressBar->setStyleSheet("background: #f5f5f5; border: 1px solid #ccc; border-radius: 4px; padding: 4px;");
    m_topBarLayout->addWidget(m_addressBar);


    m_rightLayout->addWidget(topBarContainer);

    // СПИСОК ФАЙЛОВ 
    m_fileView = new QListView();
    m_fileView->setFrameShape(QFrame::NoFrame);
    m_fileView->setViewMode(QListView::IconMode);
    m_fileView->setResizeMode(QListView::Adjust);
    m_fileView->setGridSize(QSize(100, 100));
    m_fileView->setWordWrap(true);

    m_rightLayout->addWidget(m_fileView);

    m_model = new FileListModel(this);
    m_fileView->setModel(m_model);



    m_splitter->addWidget(m_rightContainer);

    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setCollapsible(0, false); //Нельзя схлопнуть полностью
    m_splitter->setCollapsible(1, false);
    m_splitter->setSizes(QList<int>() << 220 << 860); //задаем начальные размеры







    
}