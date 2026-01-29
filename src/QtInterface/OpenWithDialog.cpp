#include "OpenWithDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QIcon>
#include <QFileInfo>

OpenWithDialog::OpenWithDialog(const std::string& filePath, const std::vector<AppInfo>& apps, QWidget *parent) : 
    QDialog(parent), 
    m_filePath(filePath), 
    m_apps(apps)
{
    setupUI();
    setupConnections();
}

void OpenWithDialog::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 500); 

    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* bgFrame = new QFrame();
    bgFrame->setObjectName("MainBackground");


    bgFrame->setStyleSheet(
        "QFrame#MainBackground { background-color: #f6f6f6; border: 1px solid #aaa; border-radius: 10px; }"
        "QLabel { color: #333; font-size: 14px; }"
        "QListWidget {"
        "    border: 1px solid #ccc;"
        "    border-radius: 6px;"
        "    background: white;"
        "    outline: none;" 
        "}"
        "QListWidget::item {"
        "    padding: 10px;"
        "    color: black;"
        "    border: none;" 
        "    margin: 2px;"  
        "    border-radius: 4px;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #ff9f43;" 
        "    color: white;"
        "}"
        "QListWidget::item:selected:hover {"
        "    background-color: #ffb773;" /* Чуть светлее оранжевый */
        "}"
        "QPushButton { padding: 8px 15px; border-radius: 5px; font-weight: bold; }"
        "QPushButton#OpenBtn { background-color: #ff9f43; color: white; border: none; }" /* Оранжевая кнопка */
        "QPushButton#OpenBtn:disabled { background-color: #cccccc; }"
        "QPushButton#OpenBtn:hover { background-color: #ffb773; }"
        "QPushButton#CancelBtn { background-color: white; border: 1px solid #ccc; color: #333; }"
        "QPushButton#CancelBtn:hover { background-color: #eee; }"
    );
    dialogLayout->addWidget(bgFrame);

    // Лайаут внутри рамки
    QVBoxLayout* mainLayout = new QVBoxLayout(bgFrame);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Заголовок
    QFileInfo fi(QString::fromStdString(m_filePath));
    QLabel* titleLabel = new QLabel("Открыть файл: <b>" + fi.fileName() + "</b>");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Список приложений
    m_appList = new QListWidget();
    m_appList->setIconSize(QSize(40, 40)); 
    m_appList->setUniformItemSizes(true);  
    

    for (const auto& app : m_apps) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString::fromStdString(app.name));
        
        
        QString iconString = QString::fromStdString(app.iconName);
        QIcon icon;

        if (iconString.startsWith("/")) {
            icon = QIcon(iconString);
        } 
        else {
            icon = QIcon::fromTheme(iconString);
            
        }

        if (icon.isNull()) {
            // icon = QIcon(":/res/file.svg"); 
            
            icon = QIcon::fromTheme("application-x-executable", QIcon(":/res/file.svg"));
        }

        item->setIcon(icon);
        
        m_appList->addItem(item);
    }
    
    mainLayout->addWidget(m_appList);

    // Кнопки (Отмена - Пружина - Открыть)
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    m_cancelBtn = new QPushButton("Отмена");
    m_cancelBtn->setObjectName("CancelBtn");
    
    m_openBtn = new QPushButton("Открыть");
    m_openBtn->setObjectName("OpenBtn");
    m_openBtn->setEnabled(false);

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_openBtn);

    mainLayout->addLayout(btnLayout);
}

void OpenWithDialog::setupConnections()
{
    // Отмена
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    // Клик по списку -> Включить кнопку "Открыть" и запомнить выбор
    connect(m_appList, &QListWidget::currentRowChanged, [this](int currentRow){
        if (currentRow >= 0 && currentRow < m_apps.size()) {
            m_selectedAppIndex = currentRow;
            m_openBtn->setEnabled(true);
        } else {
            m_selectedAppIndex = -1;
            m_openBtn->setEnabled(false);
        }
    });

    // Двойной клик 
    connect(m_appList, &QListWidget::itemDoubleClicked, this, &QDialog::accept);

    // Кнопка Открыть -> Закрыть с успехом
    connect(m_openBtn, &QPushButton::clicked, this, &QDialog::accept);



    m_appList->clearSelection();
    m_appList->setCurrentItem(nullptr);
    m_openBtn->setEnabled(false); 
    m_selectedAppIndex = -1;

}

QString OpenWithDialog::processExecCommand(const std::string& rawCmd, const std::string& path)
{
    QString cmd = QString::fromStdString(rawCmd);
    QString filePath = QString::fromStdString(path);
    
    QString safePath = "\"" + filePath + "\"";

    QStringList placeholders = {"%f", "%F", "%u", "%U"};
    
    bool replaced = false;
    for (const QString& ph : placeholders) {
        if (cmd.contains(ph)) {
            cmd.replace(ph, safePath);
            replaced = true;
        }
    }

    if (!replaced) {
        cmd += " " + safePath;
    }
    
    return cmd;
}

std::string OpenWithDialog::getFinalCommand() const
{
    if (m_selectedAppIndex < 0 || m_selectedAppIndex >= m_apps.size()) {
        return "";
    }

    const AppInfo& app = m_apps[m_selectedAppIndex];
    
    QString fullCmd = const_cast<OpenWithDialog*>(this)->processExecCommand(app.execCommand, m_filePath);
    
    return fullCmd.toStdString();
}