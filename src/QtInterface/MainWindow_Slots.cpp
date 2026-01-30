#include "MainWindow.h"
#include "FileListModel.h"
#include "Types.h" 
#include "LinuxAppManager.h"
#include "OpenWithDialog.h"

#include <QMenu>
#include <QCursor>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFrame>
#include <QFormLayout>
#include <QStyle>
#include <QModelIndex>
#include <QListView>
#include <QEvent>
#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include <fstream>
#include <iostream>





static QPixmap colorizePixmap(const QString& path, const QColor& color)
{
    QPixmap pixmap(path);
    pixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return pixmap;
}



void MainWindow::onBackClicked()
{
    // m_core.GoUp();
    m_core.GoBack();
}


void MainWindow::onForwardClicked()
{
    m_core.GoForward();
}




void MainWindow::onAddressReturnPressed()
{
    std::string pathStr = m_addressBar->text().toStdString();
    m_core.Navigate(pathStr);
}




void MainWindow::onFileDoubleClicked(const QModelIndex& index)
{
    int coreIndex = getCoreIndex(index.row());
    if(coreIndex == -1)
    {
        return;
    }

    const FileEntry& file = m_core.GetState().GetCurrentFiles()[coreIndex];
    std::string filePath = file.GetPath().string();

    
    if(file.IsDirectory())
    {
        m_core.EnterDirectory(coreIndex);
        return;
    }

    if (m_core.TryOpenCustom(filePath)) 
    {
        #ifdef LOG_APP_CORE
        qDebug() << "Файл открыт через пользовательское правило:" << QString::fromStdString(filePath);
        #endif
        return; 
    }

    if(file.GetPath().extension() == ".txt")
    {
        m_core.ExecuteFile(coreIndex);
        return;
    }

    QString qPath = QString::fromStdString(filePath);
    QUrl url = QUrl::fromLocalFile(qPath); 

    #ifdef LOG_APP_CORE
    QFileInfo checkFile(qPath);
    if (!checkFile.exists()) {
        qDebug() << "Ошибка: Файл не существует";
    } else if (!checkFile.isReadable()) {
        qDebug() << "Ошибка: Нет прав на чтение";
    }
    #endif
    
    bool success = QDesktopServices::openUrl(url);
    
    if (!success) {
        qDebug() << "ERROR: Не удалось открыть файл системным методом:" << qPath;
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл. Проверьте ассоциации в системе.");
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
        int coreIndex = getCoreIndex(index.row());
        
        if (coreIndex != -1) {
            backendIndices.push_back(coreIndex);
        }
    }

    m_core.SetSelection(backendIndices);
    
    #ifdef LOG_APP_CORE
    qDebug() << "Выделено файлов:" << backendIndices.size();
    
    for(string str : m_core.whoIsSelacted())
    {
        qDebug() << QString::fromStdString(str + " is Selected" );
    }
    #endif
}








void MainWindow::onContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_fileView->indexAt(pos);

    QMenu *menu = new QMenu(this);
    

    if (index.isValid()) { //Окно при нажатии на файл

        QAction *openAction = menu->addAction("Открыть");
        connect(openAction, &QAction::triggered, this, [this, index](){
            onFileDoubleClicked(index); 
        });


        QAction *openWithAction = menu->addAction("Открыть с помощью...");
        connect(openWithAction, &QAction::triggered, this, [this, index](){
            
            int coreIndex = getCoreIndex(index.row());        //настоящий индекс
            if (coreIndex == -1) return;
            
            std::string filePath = m_core.GetState().GetCurrentFiles()[coreIndex].GetPath().string();       //получаем путь к файлу с помощью индекса

            std::vector<AppInfo> apps = LinuxAppManager::GetAppsForFile(filePath);                          //Получаем список программ

            OpenWithDialog dialog(filePath, apps, this);
            
            if (dialog.exec() == QDialog::Accepted) {
                
                std::string cmd = dialog.getFinalCommand();
                
                if (!cmd.empty()) {
                    QProcess::startDetached("/bin/sh", QStringList() << "-c" << QString::fromStdString(cmd));       //запуск выбранной программы
                    
                    #ifdef LOG_APP_CORE
                    qDebug() << "Custom Launch:" << QString::fromStdString(cmd);
                    #endif
                }
            }
        });

        menu->addSeparator();

        menu->addAction(m_cutAction);
        

        menu->addAction(m_copyAction);


        QAction* renameFile = menu->addAction("Переименовать");
        connect(renameFile, &QAction::triggered, this, [this, index](){
            int coreIndex = getCoreIndex(index.row());
            m_core.OnRenameRequest(getUserInput(QString::fromStdString(m_core.getNameOnIndex(coreIndex))).toStdString()); 
            updateView(m_core.GetState());
        });
        


        QAction* createArchive = menu->addAction("Создать архив");
        connect(createArchive, &QAction::triggered, this, [this](){
            
            std::vector<filesystem::path> selectedFiles = m_core.whoIsSelactedPath();
            if (selectedFiles.empty()) return;

            QString newName = getUserInput("archive.zip");
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

            int coreIndex = getCoreIndex(index.row());

            if (coreIndex == -1) return;

            string path = m_core.GetState().GetCurrentFiles()[coreIndex].GetPath().string();
            
            QIcon currentIcon = index.data(Qt::DecorationRole).value<QIcon>();

            showPropertiesDialog(path, currentIcon);
        });
    } 
    else { //Окно при нажатии на пустое место
        

        QAction *createFolder = menu->addAction("Создать папку");
        connect(createFolder, &QAction::triggered, this, [this](){
            m_core.CreateFolder(getUserInput("New Folder").toStdString());
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

            filesystem::path path = m_core.GetState().GetCurrentPath() / getUserInput("newFile.txt").toStdString();
            ofstream file(path);
            if(file.is_open())
            {
                file.close();
                #ifdef LOG_ENABLED
                cout<<"Файл успешно создан: "<<path<<'\n';
                #endif
            }

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







void MainWindow::updateView(const NavigationState& state)
{
    m_addressBar->setText(QString::fromStdString(state.GetCurrentPath().string()));

    if(m_model)
    {

        std::vector<FileEntry> allFiles = state.GetCurrentFiles();

        if(!m_showHiddenFiles)
        {
            std::vector<FileEntry> filteredFiles;

            filteredFiles.reserve(allFiles.size());         //бронирует память в куче

            for(const FileEntry& file : allFiles)
            {
                if(file.GetName().size() > 0 && file.GetName()[0] != '.')
                {
                    filteredFiles.push_back(file);
                }
            }

            m_model->updateData(filteredFiles);
        }else
        {
            m_model->updateData(allFiles);
        }


        
    }

    if(m_backBtn)
    {
        m_backBtn->setEnabled(state.CanGoBack());
    }
    
    if(m_forwardBtn)
    {
        m_forwardBtn->setEnabled(state.CanGoForward());
    }

}







QString MainWindow::getUserInput(QString defaultText)
{

    QDialog dialog(this);
            
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);          //Qt::FramelessWindowHint полностью удаляет стандартную рамку ОС, Qt::Popup превращает окно во всплывающее
            
    dialog.setAttribute(Qt::WA_TranslucentBackground); //делает фон окна полностью прозрачным


    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.setSpacing(0);

    QLineEdit nameEdit;
    nameEdit.setText(defaultText);
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









void MainWindow::showPropertiesDialog(const std::string& path, const QIcon& icon)
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

    dialog.setStyleSheet(styleSheet());

    dialogLayout->addWidget(backgroundFrame);

    QVBoxLayout* mainLayout = new QVBoxLayout(backgroundFrame);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // ШАПКА (Header)
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    QPushButton* starBtn = new QPushButton();
    starBtn->setObjectName("headerBtn");
    if(isSystemThemeDark()) 
    {
        starBtn->setIcon(QIcon(":/res/star_no_active_dark.svg"));
    }else
    {
        starBtn->setIcon(QIcon(":/res/star_no_active.svg"));
    } 
    starBtn->setToolTip("Добавить в избранное");
    
    QPushButton* closeBtn = new QPushButton();
    closeBtn->setObjectName("headerBtn");
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

    // ИКОНКИ
    // QLabel* iconLabel = new QLabel();

    // QString iconPath = FileListModel::getIconPath(props.name, props.isDirectory);

    // iconLabel->setPixmap(QIcon(iconPath).pixmap(64, 64));
    // iconLabel->setAlignment(Qt::AlignCenter);
    // iconLabel->setFixedSize(64, 64); //  размер лейбла = размер картинки

    QLabel* iconLabel = new QLabel();
    
    QPixmap finalPixmap;

    if (!icon.isNull()) {
        finalPixmap = icon.pixmap(256, 256).scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } 
    else {
        QString iconPath = FileListModel::getIconPath(props.name, props.isDirectory);
        finalPixmap = colorizePixmap(iconPath, isSystemThemeDark() ? Qt::white : Qt::black);
        finalPixmap = finalPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    iconLabel->setPixmap(finalPixmap);
    iconLabel->setAlignment(Qt::AlignCenter);
    
    iconLabel->setMinimumSize(64, 64);


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


void MainWindow::changeIconSize(int size)
{
    int newSize = m_currentIconSize + size;

    if(newSize < 80)
    {
        newSize = 80;
    }
    if(newSize > 200)
    {
        newSize = 200;
    }

    if(newSize != m_currentIconSize)
    {
        m_currentIconSize = newSize;

        #ifdef LOG_ENABLED
        cout << "Изменение размера иконок: " << m_currentIconSize << endl;
        #endif

        m_fileView->setGridSize(QSize(m_currentIconSize, m_currentIconSize));

        m_fileView->setIconSize(QSize(m_currentIconSize - 20, m_currentIconSize - 40));

        QFont font = m_fileView->font();
        
        int fontSize = 9 + (m_currentIconSize - 50) / 20;

        if (fontSize > 16) fontSize = 16; 

        font.setPointSize(fontSize);
        m_fileView->setFont(font);

    }

}

int MainWindow::getCoreIndex(int uiIndex)
{
    if(!m_model)
    {
        return -1;
    }

    if(uiIndex < 0 || uiIndex >= m_model->rowCount())
    {
        return -1;
    }

    const FileEntry& visibleFile = m_model->getFile(uiIndex);
    string visiblePath = visibleFile.GetPath().string();

    const vector<FileEntry>& allFiles = m_core.GetState().GetCurrentFiles();

    for(long long i = 0; i < allFiles.size();++i)
    {
        if(allFiles[i].GetPath().string() == visiblePath)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}




void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ThemeChange) {
        
        bool currentSystemDark = isSystemThemeDark();
        
        if (currentSystemDark != m_isDark) {
            m_isDark = currentSystemDark;
            setStyleSheetsForMainWindow();
            updateIcons();
            
            if (m_model) {
                if(m_model != nullptr)
                {
                    m_model->setupIcons();
                    QTimer::singleShot(0, m_model, &FileListModel::refresh);       //перекладывает отрисовку иконок в новой теме на следующий кадр
                }
            }
        }
    }
}