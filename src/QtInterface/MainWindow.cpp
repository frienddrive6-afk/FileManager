#include "MainWindow.h"
#include "FileListModel.h"
#include "Types.h"

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
#include <QProcess>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QFormLayout>
#include <fstream>


MainWindow::MainWindow(AppCore& core,QWidget* parent) :
    QMainWindow(parent),
    m_core(core)
{
    setUI();
    setupActions();

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
        qDebug() << QString::fromStdString(str + "is Selected" );
    }
    #endif
}

void MainWindow::setupActions()
{
    // КОПИРОВАНИЕ (Ctrl + C)
    m_copyAction = new QAction("Копировать", this);
    m_copyAction->setShortcut(QKeySequence::Copy);      //QKeySequence::Copy это Ctrl+C
    this->addAction(m_copyAction); 
    connect(m_copyAction, &QAction::triggered, this, [this](){
        m_core.OnCopyRequest();
        updateView(m_core.GetState());
    });

    // ВЫРЕЗАНИЕ (Ctrl + X)
    m_cutAction = new QAction("Вырезать", this);
    m_cutAction->setShortcut(QKeySequence::Cut);         //Ctrl + X
    this->addAction(m_cutAction);
    connect(m_cutAction, &QAction::triggered, this, [this](){
        m_core.OnCutRequest();
        updateView(m_core.GetState());
    });

    // ВСТАВИТЬ (Ctrl + V)
    m_pasteAction = new QAction("Вставить", this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    this->addAction(m_pasteAction);
    connect(m_pasteAction, &QAction::triggered, this, [this](){
        m_core.OnPasteRequest();
        updateView(m_core.GetState());
    });

    // УДАЛЕНИЕ (Del)
    m_deleteAction = new QAction("Удалить", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);      // Del
    this->addAction(m_deleteAction);
    connect(m_deleteAction, &QAction::triggered, this, [this](){
        m_core.OnDeleteRequest();
        updateView(m_core.GetState());
    });


    //ВЫДЕЛИТЬ ВСЕ
    m_secectAll = new QAction("Выделить все", this);
    m_secectAll->setShortcut(QKeySequence::SelectAll);       //Ctrl + A
    this->addAction(m_secectAll);
    connect(m_secectAll, &QAction::triggered, this, [this](){
        m_core.OnSelectAllRequest();
        updateView(m_core.GetState());
    });

}

QString MainWindow::QLineEditAction(QString text)
{

    QDialog dialog(this);
            
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);          //Qt::FramelessWindowHint полностью удаляет стандартную рамку ОС, Qt::Popup превращает окно во всплывающее
            
    dialog.setAttribute(Qt::WA_TranslucentBackground); //делает фон окна полностью прозрачным


    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.setSpacing(0);

    QLineEdit nameEdit;
    nameEdit.setText(text);
    nameEdit.selectAll(); 
    nameEdit.setFrame(false);
            
    nameEdit.setProperty("class", "foldername");
            
    layout.addWidget(&nameEdit);

    connect(&nameEdit, &QLineEdit::returnPressed, &dialog, &QDialog::accept);

    dialog.setFixedWidth(200); 
            
    dialog.move(QCursor::pos() - QPoint(10, 15));

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = nameEdit.text();
        if (!newName.isEmpty()) {
            return newName;
        }
    }

    return QString{};

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



        menu->addAction(m_cutAction);
        

        menu->addAction(m_copyAction);


        QAction* renameFile = menu->addAction("Переименовать");
        connect(renameFile, &QAction::triggered, this, [this, index](){
            m_core.OnRenameRequest(QLineEditAction(QString::fromStdString(m_core.getNameOnIndex(index.row()))).toStdString()); 
            updateView(m_core.GetState());
        });
        


        QAction* createArchive = menu->addAction("Создать архив");
        connect(createArchive, &QAction::triggered, this, [this](){
            
            std::vector<filesystem::path> selectedFiles = m_core.whoIsSelactedPath();
            if (selectedFiles.empty()) return;

            QString newName = QLineEditAction("archive.zip");
            if(!newName.isEmpty())
            {
                QStringList arguments;
                arguments << "-r" << newName;        

                for(const filesystem::path& path : selectedFiles) {
                    arguments << QString::fromStdString(path.filename().string());
                }

                QString workingDir = QString::fromStdString(m_core.GetState().GetCurrentPath().string());

                bool isStarted = QProcess::startDetached("zip", arguments, workingDir);

                #ifdef LOG_APP_CORE
                if(isStarted) {
                    qDebug() << "Архивация запущена:" << arguments.join(" ");
                } else {
                    qDebug() << "Ошибка запуска";
                }
                #endif
            }

        });


        menu->addAction(m_deleteAction);

        menu->addSeparator();
        
        QAction *propertiesAction = menu->addAction("Свойства");
        connect(propertiesAction, &QAction::triggered, this, [this, index](){

            string path = m_core.GetState().GetCurrentFiles()[index.row()].GetPath().string();
            
            showPropertiesDialog(path);
        });
    } 
    else { //Окно при нажатии на пустое место
        

        QAction *createFolder = menu->addAction("Создать папку");
        connect(createFolder, &QAction::triggered, this, [this](){
            m_core.CreateFolder(QLineEditAction("New Folder").toStdString());
            updateView(m_core.GetState());
        });




        menu->addSeparator();
        
        menu->addAction(m_pasteAction); 
        menu->addAction(m_secectAll);

        menu->addSeparator();

        QAction *updateItfo = menu->addAction("Обновить");
        connect(updateItfo, &QAction::triggered, this, [this](){
            m_core.Navigate(m_core.GetState().GetCurrentPath());
        });

        menu->addSeparator();

        QAction* openInTerminal = menu->addAction("Открыть в терминале");
        connect(openInTerminal, &QAction::triggered, this, [this]() {
            QString workingDir = QString::fromStdWString(m_core.GetState().GetCurrentPath().wstring());

            bool success = QProcess::startDetached("x-terminal-emulator", {}, workingDir);

            #ifdef LOG_APP_CORE
            if (!success) {
                qDebug() << "Не удалось запустить терминал в" << workingDir;
            }else
            {
                qDebug() << "Терминал запущен в" << workingDir;
            }
            #endif
        });


        QAction* createTXT = menu->addAction("Создать текстовый документ");
        connect(createTXT, &QAction::triggered, this, [this](){
            // QString workingDir = QString::fromStdWString(m_core.GetState().GetCurrentPath().wstring());

            // bool success = QProcess::startDetached(">>", {QLineEditAction("newFile.txt")}, workingDir);

            filesystem::path path = m_core.GetState().GetCurrentPath() / QLineEditAction("newFile.txt").toStdString();
            ofstream file(path);
            if(file.is_open())
            {
                file.close();
                #ifdef LOG_ENABLED
                cout<<"Файл успешно создан: "<<path<<'\n';
                #endif
            }

            // m_core.CreateFile("newFile.txt");
            updateView(m_core.GetState());
        });

        menu->addSeparator();

        QAction *infoThisFolder = menu->addAction("Свойства");
        connect(infoThisFolder, &QAction::triggered, this, [this](){
            
            string path = m_core.GetState().GetCurrentPath().string();
        
            showPropertiesDialog(path);

        });

    }

    menu->exec(m_fileView->mapToGlobal(pos));

    menu->deleteLater(); //Удаляем меню из памяти то как каждый раз создается новое
}





void MainWindow::showPropertiesDialog(const std::string& path)
{
    FileProperties props = m_core.GetProperties(path);

    QDialog dialog(this);
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);   //удаляет внешнюю рамку операционной системы + окно является диалоговым
    dialog.setAttribute(Qt::WA_TranslucentBackground); // Делаем саму подложку окна прозрачной
    dialog.setFixedWidth(400); 

    QVBoxLayout* dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->setContentsMargins(0, 0, 0, 0);   //убрать отступы


    QFrame* backgroundFrame = new QFrame(); //фон окна
    backgroundFrame->setObjectName("MainBackground");

    dialog.setStyleSheet(
        "QFrame#MainBackground {"
        "   background-color: #f6f6f6;"   
        "   border-radius: 12px;"         
        "   border: 1px solid #dbdbdb;"   
        "}"
        "QLabel { color: #2e3436; font-size: 14px; }"
        "QLabel#Title { font-size: 18px; font-weight: bold; }"
        "QLabel#SubTitle { color: #5e5c64; font-size: 13px; }"
        
        "QFrame#InfoCard { background-color: #ffffff; border-radius: 8px; border: 1px solid #dedede; }"
        
        "QPushButton { border: none; border-radius: 4px; padding: 4px; }"
        "QPushButton:hover { background-color: #e8e8e8; }"
    );

    dialogLayout->addWidget(backgroundFrame);

    QVBoxLayout* mainLayout = new QVBoxLayout(backgroundFrame);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // ШАПКА (Header)
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    QPushButton* starBtn = new QPushButton();
    starBtn->setIcon(QIcon(":/res/star.svg")); 
    starBtn->setToolTip("Добавить в избранное");
    
    QPushButton* closeBtn = new QPushButton();
    closeBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    closeBtn->setToolTip("Закрыть");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    headerLayout->addWidget(starBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(closeBtn);

    mainLayout->addLayout(headerLayout);

    // ОСНОВНАЯ ИНФОРМАЦИЯ
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setAlignment(Qt::AlignCenter);

    // большаяконка
    QLabel* iconLabel = new QLabel();
    QString iconPath = props.isDirectory ? ":/res/folder.svg" : ":/res/file.svg"; 
    iconLabel->setPixmap(QIcon(iconPath).pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    // Имя
    QLabel* nameLabel = new QLabel(QString::fromStdString(props.name));
    nameLabel->setObjectName("Title");
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);  //Разрешить перенос

    // Детали
    QString detailsText;
    if (props.isDirectory) {
        detailsText = QString("%1 объектов").arg(props.fileCount + props.folderCount) + ",   " + QString::fromStdString( FileSystemManager::FormatSize(props.size));
    } else {
        // detailsText = QString("%1 байт").arg(props.size); 
        detailsText = QString::fromStdString( FileSystemManager::FormatSize(props.size));
    }
    QLabel* sizeLabel = new QLabel(detailsText);
    sizeLabel->setObjectName("SubTitle");
    sizeLabel->setAlignment(Qt::AlignCenter);

    infoLayout->addWidget(iconLabel);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(sizeLabel);

    mainLayout->addLayout(infoLayout);

    // КАРТОЧКА СВОЙСТВ
    QFrame* infoCard = new QFrame();
    infoCard->setObjectName("InfoCard");
    
    QFormLayout* formLayout = new QFormLayout(infoCard);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setVerticalSpacing(12);
    formLayout->setContentsMargins(15, 15, 15, 15);

    // Родительская папка
    QLabel* parentValue = new QLabel(QString::fromStdString(props.parentPath));
    parentValue->setWordWrap(true);
    formLayout->addRow("Родительская папка", parentValue);

    // Дата изменения
    QLabel* modifiedValue = new QLabel(QString::fromStdString(props.dateModified));
    formLayout->addRow("Последнее изменение", modifiedValue);

    // Дата создания
    QLabel* createdValue = new QLabel("Временно недоступно");
    createdValue->setStyleSheet("color: #999; font-style: italic;");
    formLayout->addRow("Создано", createdValue);

    mainLayout->addWidget(infoCard);

    
    // КАРТОЧКА РАЗРЕШЕНИЙ
    
    QFrame* permCard = new QFrame();
    permCard->setObjectName("InfoCard");
    QHBoxLayout* permLayout = new QHBoxLayout(permCard);
    
    QLabel* permTitle = new QLabel("Разрешения");
    QLabel* permLink = new QLabel("Создание и удаление файлов >");
    permLink->setObjectName("SubTitle");
    permLink->setAlignment(Qt::AlignRight);

    permLayout->addWidget(permTitle);
    permLayout->addWidget(permLink);

    mainLayout->addWidget(permCard);

    mainLayout->addStretch();

    
    dialog.exec();  //показать
}











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
    );

}