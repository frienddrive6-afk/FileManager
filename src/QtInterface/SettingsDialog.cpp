#include "SettingsDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>

SettingsDialog::SettingsDialog(AppCore& core, int currentSize, int currentSpacing, QWidget *parent) : 
    QDialog(parent),
    m_core(core),
    m_initialSize(currentSize),
    m_initialSpacing(currentSpacing)
{
    setupUI();

    m_core.OnAssociationsChanged = [this]()
    {
        this->refreshRulesTable();
    };

    refreshRulesTable();

    setupConnections();
}

void SettingsDialog::setupUI()
{
    setWindowTitle("Настройки");
    resize(700, 500); 

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // --- ЛЕВАЯ ПАНЕЛЬ ---
    m_tabsList = new QListWidget(this);
    m_tabsList->setFixedWidth(150); 
    m_tabsList->addItem("Общие");
    m_tabsList->addItem("Типы файлов"); 
    m_tabsList->addItem("Рамеры");
    
    // --- ПРАВАЯ ПАНЕЛЬ ---
    m_pagesStack = new QStackedWidget(this);
    
    m_pagesStack->addWidget(createGeneralPage());      // Индекс 0
    m_pagesStack->addWidget(createAssociationsPage()); // Индекс 1
    m_pagesStack->addWidget(createSizePage());         // Индекс 2

    mainLayout->addWidget(m_tabsList);
    mainLayout->addWidget(m_pagesStack);
}

void SettingsDialog::setupConnections()
{
    
    connect(m_tabsList, &QListWidget::currentRowChanged, m_pagesStack, &QStackedWidget::setCurrentIndex);           //Переключение страниц
    connect(m_addBtn, &QPushButton::clicked, this, &SettingsDialog::onAddClicked);
    
    m_tabsList->setCurrentRow(0);             //Вкладка по умолчанию "Общие"
}

void SettingsDialog::onAddClicked()
{
    QString ext = m_extInput->text().trimmed(); 
    QString cmd = m_cmdInput->text().trimmed();
    bool term = m_termCheck->isChecked();

    
    if (ext.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите расширение (например .png)");
        return;
    }
    if (!ext.startsWith(".")) {
        ext = "." + ext; 
    }

    if (cmd.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите команду запуска");
        return;
    }
    if (!cmd.contains("%path%")) {
        QMessageBox::warning(this, "Внимание", 
            "В команде нет '%path%'. Программа откроется, но файл передан не будет.");
    }

    FileAssociation rule;
    rule.extension = ext.toStdString();
    rule.command = cmd.toStdString();
    rule.runInTerminal = term;

    m_core.AddAssociation(rule); 

    m_extInput->clear();
    m_cmdInput->clear();
    m_termCheck->setChecked(false);
}


void SettingsDialog::refreshRulesTable()
{
    m_rulesTable->setRowCount(0); // Удаляет все строки
    
    const vector<FileAssociation>& rules = m_core.GetAssociations();

    for (const FileAssociation& rule : rules) {
        int row = m_rulesTable->rowCount();
        m_rulesTable->insertRow(row);

        m_rulesTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(rule.extension)));
        
        m_rulesTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rule.command)));
        
        QString termText = rule.runInTerminal ? "Да" : "Нет";
        m_rulesTable->setItem(row, 2, new QTableWidgetItem(termText));

        QPushButton* delBtn = new QPushButton("X");
        delBtn->setStyleSheet("color: red; font-weight: bold;");
        delBtn->setFixedSize(30, 25);
        
        std::string extToDelete = rule.extension;
        
        connect(delBtn, &QPushButton::clicked, [this, extToDelete](){
            m_core.RemoveAssociation(extToDelete);
        });

        QWidget* container = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(container);

        layout->addWidget(delBtn);            
        layout->setAlignment(Qt::AlignCenter); 
        layout->setContentsMargins(0, 0, 0, 0); 

        m_rulesTable->setCellWidget(row, 3, container);
    }
}




QWidget* SettingsDialog::createGeneralPage()
{
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(new QLabel("Тут будут настройки потом"));
    
    return page;
}

QWidget* SettingsDialog::createAssociationsPage()
{
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);

    // добавления нового правила
    QFrame* addFrame = new QFrame();
    addFrame->setStyleSheet("background: #f0f0f0; border-radius: 5px;");
    QHBoxLayout* addLayout = new QHBoxLayout(addFrame);

    m_extInput = new QLineEdit();
    m_extInput->setPlaceholderText(".ext");
    m_extInput->setFixedWidth(60);

    m_cmdInput = new QLineEdit();
    m_cmdInput->setPlaceholderText("Команда (например: gimp %path%)");

    m_termCheck = new QCheckBox("Терминал");
    
    m_addBtn = new QPushButton("Добавить");

    addLayout->addWidget(m_extInput);
    addLayout->addWidget(m_cmdInput);
    addLayout->addWidget(m_termCheck);
    addLayout->addWidget(m_addBtn);

    layout->addWidget(addFrame);

    // Таблица существующих правил
    m_rulesTable = new QTableWidget();
    m_rulesTable->setColumnCount(4);
    m_rulesTable->setHorizontalHeaderLabels({"Расширение", "Команда", "Терминал", ""});
    m_rulesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Команда растягивается
    
    layout->addWidget(m_rulesTable);

    return page;
}




QWidget* SettingsDialog::createSizePage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setSpacing(20); 

    // Блок размера иконок
    layout->addWidget(new QLabel("Размер иконок:"));
    QSlider* s_size = new QSlider(Qt::Horizontal);
    s_size->setRange(48, 256);
    s_size->setValue(m_initialSize);
    layout->addWidget(s_size);

    // Блок отступов
    layout->addWidget(new QLabel("Отступы между иконками:"));
    QSlider* s_spacing = new QSlider(Qt::Horizontal);
    s_spacing->setRange(0, 50);
    s_spacing->setValue(m_initialSpacing);
    layout->addWidget(s_spacing);

    auto emitChange = [this, s_size, s_spacing]() {
        #ifdef LOG_APP_CORE
        qDebug() << "[Settings] Слайдер переменен. Новые значения -> Size:" << s_size->value() << "Spacing:" << s_spacing->value();
        #endif
        emit iconConfigChanged(s_size->value(), s_spacing->value());
    };

    connect(s_size, &QSlider::valueChanged, emitChange);
    connect(s_spacing, &QSlider::valueChanged, emitChange);

    layout->addStretch();
    return page;
}
