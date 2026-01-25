#pragma once
#include <QDialog>

#include "AppCore.h"

class QListWidget;
class QStackedWidget;
class QLineEdit;
class QCheckBox;
class QTableWidget; 


/// @brief класс диалога настроек
/// @param m_tabsList левая панель
/// @param m_pagesStack правая панель
/// @param m_extInput ввод расширения
/// @param m_cmdInput ввод команды
/// @param m_termCheck галочка терминала
/// @param m_rulesTable таблица правил
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(AppCore& core,QWidget *parent = nullptr);

private:
    AppCore& m_core;

    QListWidget* m_tabsList;      
    QStackedWidget* m_pagesStack; 

    QLineEdit* m_extInput;        
    QLineEdit* m_cmdInput;        
    QCheckBox* m_termCheck;       
    QTableWidget* m_rulesTable;

    QPushButton* m_addBtn;

    void setupUI();
    void setupConnections();
    
    // Метод для создания страницы "Ассоциации"
    QWidget* createAssociationsPage();

    // Метод для создания страницы "Основные/Темы"
    QWidget* createGeneralPage();


    void refreshRulesTable();


    private slots:
        void onAddClicked();        //кнопка добавить

};