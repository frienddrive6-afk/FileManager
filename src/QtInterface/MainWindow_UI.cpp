#include "MainWindow.h"
#include "FileListModel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAction>
#include <QDir>
#include <QStandardPaths>
#include <QMenu>
#include <QLabel>
#include <QWidgetAction>



void MainWindow::setUI()
{
    setStyleSheetsForMainWindow();

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
    QWidget* leftSide = new QWidget(this);

    QVBoxLayout* sideBarLayout = new QVBoxLayout();
    sideBarLayout->setContentsMargins(0, 0, 0, 0);
    sideBarLayout->setSpacing(0);
    leftSide->setLayout(sideBarLayout);


    // Панель опций НАЧАЛО    ==========================================================
    QWidget* optionsContainer = new QWidget();
    optionsContainer->setObjectName("optionsPanel"); // имя для QSS
    optionsContainer->setAttribute(Qt::WA_StyledBackground, true);
    optionsContainer->setFixedHeight(30);

    QHBoxLayout* optionsLayout = new QHBoxLayout(optionsContainer);
    sideBarLayout->addLayout(optionsLayout);

    optionsLayout->setContentsMargins(0, 0, 0, 0);
    optionsLayout->setSpacing(0);
    optionsLayout->addStretch(1);

    sideBarLayout->addWidget(optionsContainer);

    QPushButton* menuBtn = new QPushButton();
    menuBtn->setIcon(QIcon(":/res/3_lines.svg"));
    menuBtn->setFixedWidth(50);
    menuBtn->setStyleSheet("background-color: transparent;");
    menuBtn->setFlat(true);
    optionsLayout->addWidget(menuBtn);

    QMenu* settingsMenu = new QMenu(this);

    menuBtn->setMenu(settingsMenu);

    QAction* showHiddenAction = new QAction("Показать скрытые файлы", this);
    showHiddenAction->setCheckable(true);
    showHiddenAction->setChecked(m_showHiddenFiles);

    connect(showHiddenAction, &QAction::toggled, this, [this](bool checked) {
        
        m_showHiddenFiles = checked;

        #ifdef LOG_APP_CORE
        cout << "Показать скрытые файлы: " << m_showHiddenFiles << endl;
        #endif

        updateView(m_core.GetState());
    });

    settingsMenu->addAction(showHiddenAction);

    settingsMenu->addSeparator();


    QWidget* zoomWidget = new QWidget();
    zoomWidget->setObjectName("zoomWidget");


    QHBoxLayout* zoomLayout = new QHBoxLayout(zoomWidget);
    zoomLayout->setContentsMargins(0, 0, 0, 0);
    zoomLayout->setSpacing(2);

    QLabel* zoomLabel = new QLabel("Размер значьков: ");
    zoomLabel->setObjectName("zoomLabel");

    QPushButton* btnMinus = new QPushButton("-");
    btnMinus->setObjectName("zoomBtn");

    QPushButton* btnPlus = new QPushButton("+");
    btnPlus->setObjectName("zoomBtn");

    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addStretch();
    zoomLayout->addWidget(btnMinus);
    zoomLayout->addWidget(btnPlus);

    connect(btnMinus, &QPushButton::clicked, this, [this](){
        changeIconSize(-20);

    });

    connect(btnPlus, &QPushButton::clicked, this, [this](){
        changeIconSize(20);
    });

    QWidgetAction* zoomAction = new QWidgetAction(settingsMenu);
    zoomAction->setDefaultWidget(zoomWidget);

    settingsMenu->addAction(zoomAction);
    settingsMenu->addSeparator();

    //панель опций КОНЕЦ      ==========================================================



    m_sideBar = new QListView(); 
    m_sideBar->setFrameShape(QFrame::NoFrame); // Убираем наружную рамку
    m_sideBar->setViewMode(QListView::ListMode); 
    m_sideBar->setIconSize(QSize(24, 24));       
    m_sideBar->setSpacing(5);                    
    m_sideBar->setEditTriggers(QAbstractItemView::NoEditTriggers);

    sideBarLayout->addWidget(m_sideBar);

    // m_splitter->addWidget(m_sideBar);
    m_splitter->addWidget(leftSide);
    

    m_sideBarModel = new QStandardItemModel(this);

    QList<QStandardItem*> items;

    QStandardItem* homeItem = new QStandardItem("Home");
    homeItem->setIcon(QIcon(":/res/folder.svg")); 
    homeItem->setData(QDir::homePath(), Qt::UserRole);
    items.append(homeItem);

    QStandardItem* docItem = new QStandardItem("Documents");
    docItem->setIcon(QIcon(":/res/folder.svg"));
    docItem->setData(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), Qt::UserRole);
    items.append(docItem);

    QStandardItem* downItem = new QStandardItem("Downloads");
    downItem->setIcon(QIcon(":/res/folder.svg")); 
    downItem->setData(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), Qt::UserRole);
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
    m_fileView->setContextMenuPolicy(Qt::CustomContextMenu);                    //позволяет вызывать контекстное меню по правой клавише мыши

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



QAction* MainWindow::createAction(QString text, QKeySequence shortcut, std::function<void()> slot)
{
    QAction* action = new QAction(text, this);
    action->setShortcut(shortcut);
    this->addAction(action);

    connect(action, &QAction::triggered, this, slot);
    return action;
}






void MainWindow::setupActions()
{
    // КОПИРОВАНИЕ (Ctrl + C)
    m_copyAction = createAction("Копировать", QKeySequence::Copy, [this](){
        m_core.OnCopyRequest();
        updateView(m_core.GetState());
    });

    // ВЫРЕЗАНИЕ (Ctrl + X)
    m_cutAction = createAction("Вырезать", QKeySequence::Cut, [this](){
        m_core.OnCutRequest();
        updateView(m_core.GetState());
    });


    // ВСТАВИТЬ (Ctrl + V)
    m_pasteAction = createAction("Вставить", QKeySequence::Paste, [this](){
        m_core.OnPasteRequest();
        updateView(m_core.GetState());
    });

    // УДАЛЕНИЕ (Del)
    m_deleteAction = createAction("Удалить", QKeySequence::Delete, [this](){
        m_core.OnDeleteRequest();
        updateView(m_core.GetState());
    });


    //ВЫДЕЛИТЬ ВСЕ (Ctrl + A)
    m_secectAll = createAction("Выделить все", QKeySequence::SelectAll, [this](){
        m_fileView->selectAll();
    });

}








void MainWindow::setStyleSheetsForMainWindow()
{
    this->setStyleSheet(
        "QMainWindow { background-color: #f3f3f3; }"
        "QLineEdit { background: white; border: 1px solid #ccc; border-radius: 4px; padding: 4px; }"
        "QDialog {"
                "   border: 2px solid #0078d7;"  
                "   background-color: white;"    
                "   border-radius: 0px;"         
                "}"

        "QLineEdit[class = 'foldername'] { "
        "   background: white;"        
        "   color: black;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "   selection-background-color: #0078d7;"
        "   selection-color: white;" 
        "}"

        
        "QMenu { background-color: white; border: 1px solid gray; }"
        "QMenu::item { padding: 5px 20px; }"
        "QMenu::item:selected { background-color: #0078d7; color: white; }"

        "QWidget#optionsPanel {"
        "    background-color: #ffffff;"
        "    border-bottom: 1px solid #d0d0d0;"
        "}"


        "QWidget#zoomWidget {"
        "    background: transparent;"
        "}"

        // 2. Кнопки зума (обращаемся по имени #zoomBtn)
        "QPushButton#zoomBtn {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #dcdcdc;"
        "    border-radius: 4px;"
        "    min-width: 30px; max-width: 30px;"
        "    min-height: 25px; max-height: 25px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "}"
        
        // Эффекты при наведении и нажатии
        "QPushButton#zoomBtn:hover { background-color: #e0e0e0; }"
        "QPushButton#zoomBtn:pressed { background-color: #d0d0d0; }"

        // 3. Текст "Размер значков"
        "QLabel#zoomLabel {"
        "    font-size: 14px;"
        "    color: black;"
        "    background: transparent;"
        "    border: none;"
        "}"

    );

}