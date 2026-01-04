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
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMenu>
#include <QAction>
#include <QCursor> 
#include <QWidgetAction>


MainWindow::MainWindow(AppCore& core,QWidget* parent) :
    QMainWindow(parent),
    m_core(core)
{
    setUI();

    connect(m_backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_addressBar, &QLineEdit::returnPressed,this, &MainWindow::onAddressReturnPressed);
    connect(m_fileView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
    connect(m_sideBar, &QListView::clicked, this, &MainWindow::goToDirectory);
    connect(m_fileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_fileView, &QListView::customContextMenuRequested, this, &MainWindow::onContextMenuRequested);

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

void MainWindow::goToDirectory(const QModelIndex& index)
{
    QString path = index.data(Qt::UserRole).toString();

    if(!path.isEmpty())
    {
        m_core.Navigate(path.toStdString());
    }

}


void MainWindow::onSelectionChanged()
{
    QList<QModelIndex> qtSelection = m_fileView->selectionModel()->selectedIndexes();
    

    std::vector<int> backendIndices;

    for (const QModelIndex& index : qtSelection) {
        backendIndices.push_back(index.row());
    }

    m_core.SetSelection(backendIndices);
    
    #ifdef LOG_APP_CORE
    qDebug() << "Выделено файлов:" << backendIndices.size();
    
    for(string str : m_core.whoIsSelacted())
    {
        qDebug() << QString::fromStdString(str);
    }
    #endif
}

void MainWindow::onContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_fileView->indexAt(pos);

    QMenu *menu = new QMenu(this);
    
    menu->setStyleSheet(
        "QMenu { background-color: white; border: 1px solid gray; }"
        "QMenu::item { padding: 5px 20px; }"
        "QMenu::item:selected { background-color: #0078d7; color: white; }"
    );

    if (index.isValid()) { //Окно при нажатии на файл

        
        QAction *openAction = menu->addAction("Открыть");
        connect(openAction, &QAction::triggered, this, [this, index](){
            m_core.EnterDirectory(index.row());
            #ifdef LOG_APP_CORE
                qDebug() << "Открыть" << QString::fromStdString(m_core.getNameOnIndex(index.row())) <<'\n';

            #endif
        });



        QAction* cutFile = menu->addAction("Вырезать");
        connect(cutFile, &QAction::triggered, this, [this](){
            m_core.OnCutRequest();
            updateView(m_core.GetState());
        });
        

        QAction* copyFilse = menu->addAction("Копировать");
        connect(copyFilse, &QAction::triggered, this, [this](){
            m_core.OnCopyRequest();
            updateView(m_core.GetState());
        });
        

        QAction *deleteAction = menu->addAction("Удалить");
        connect(deleteAction, &QAction::triggered, this, [this](){
            m_core.OnDeleteRequest(); 
            updateView(m_core.GetState());
            
            
        });

        menu->addSeparator();
        
        menu->addAction("Свойства"); // Пока пустышка
    } 
    else { //Окно при нажатии на пустое место
        
        QAction *createFolder = menu->addAction("Создать папку");
        connect(createFolder, &QAction::triggered, this, [this, pos](){

            QMenu* createFolderMenu = new QMenu(this);
            
            QLineEdit* folderNameEdit = new QLineEdit("New Folder");
            folderNameEdit->selectAll();

            QWidgetAction* editAction = new QWidgetAction(createFolderMenu);
            editAction->setDefaultWidget(folderNameEdit);

            createFolderMenu->addAction(editAction);

            

            connect(folderNameEdit, &QLineEdit::returnPressed, [=](){

                QString newName = folderNameEdit->text();
                if(!newName.isEmpty()) 
                {
                    m_core.CreateFolder(newName.toStdString());
                    updateView(m_core.GetState());
                }

                createFolderMenu->close();
            });

            folderNameEdit->setFocus();


            createFolderMenu->exec(m_fileView->mapToGlobal(pos));

            createFolderMenu->deleteLater();
        });

        menu->addSeparator();
        
        QAction* paste = menu->addAction("Вставить");
        connect(paste, &QAction::triggered, this, [this](){
            m_core.OnPasteRequest();
            updateView(m_core.GetState());
        });

        menu->addSeparator();

        QAction *updateItfo = menu->addAction("Обновить");
        connect(updateItfo, &QAction::triggered, this, [this](){
            m_core.Navigate(m_core.GetState().GetCurrentPath());
        });

        QAction *infoThisFolder = menu->addAction("Свойства");
        connect(infoThisFolder, &QAction::triggered, this, [this](){
            //добавить логику
        });

    }

    menu->exec(m_fileView->mapToGlobal(pos));

    menu->deleteLater(); //Удаляем меню из памяти то как каждый раз создается новое
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
    m_sideBar->setViewMode(QListView::ListMode); 
    m_sideBar->setIconSize(QSize(24, 24));       
    m_sideBar->setSpacing(5);                    
    m_sideBar->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_splitter->addWidget(m_sideBar);

    m_sideBarModel = new QStandardItemModel(this);

    QList<QStandardItem*> items;

    QStandardItem* homeItem = new QStandardItem("Home");
    homeItem->setIcon(QIcon(":/res/folder.svg")); 
    homeItem->setData("/home/nomantb", Qt::UserRole); 
    items.append(homeItem);

    QStandardItem* docItem = new QStandardItem("Documents");
    docItem->setIcon(QIcon(":/res/folder.svg"));
    docItem->setData("/home/nomantb/Документы", Qt::UserRole);
    items.append(docItem);

    QStandardItem* downItem = new QStandardItem("Downloads");
    downItem->setIcon(QIcon(":/res/folder.svg")); 
    downItem->setData("/home/nomantb/Загрузки", Qt::UserRole);
    items.append(downItem);

    QStandardItem* testFolder = new QStandardItem("test_file_Manedger");
    testFolder->setIcon(QIcon(":/res/folder.svg")); 
    testFolder->setData("/home/nomantb/Загрузки/test_file_Manedger", Qt::UserRole);
    items.append(testFolder);


    m_sideBarModel->appendColumn(items);

    m_sideBar->setModel(m_sideBarModel);




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
    m_fileView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileView->setContextMenuPolicy(Qt::CustomContextMenu);//позволяет вызывать контекстное меню по правой клавише мыши

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