#pragma once

#include "AppCore.h"
#include "NavigationState.h"

#include <QMainWindow>


class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QSplitter;
class QListView;
class QPushButton;
class QLineEdit;
class FileListModel;



/// @brief Главное окно приложения
///@param m_centralWidget центральный виджет на нем будет располагаться все остальное
///@param m_mainLayout основной макет для центрального виджета
///@param m_splitter разделитель между левой и правой панелью
///@param m_sideBar левая панель на ней будут распологатся закрепленные(часто используемые) папки
///@param m_rightContainer правый контейнер нужен что бы поместить его в splitter
///@param m_rightLayout основной макет для правого контейнера хранится в m_rightContainer
///@param m_topBarLayout основной макет для верхней панели (стрелочек и адресная строка)
///@param m_backBtn кнопка назад
///@param m_forwardBtn кнопка вперед
///@param m_addressBar адресная строка
///@param m_fileView элемент где будут отображатся все файлы
///@param m_model модель списка файлов
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AppCore& core,QWidget* parent = nullptr);

    /// @brief обновляет информацию о файлах для отображения в интерфейс
    /// @param state текущее состояние навигации
    void updateView(const NavigationState& state);
private:
    AppCore& m_core;

    QWidget* m_centralWidget;            
    QVBoxLayout* m_mainLayout;
    QSplitter* m_splitter;
    QListView* m_sideBar;
    QWidget* m_rightContainer;
    QVBoxLayout* m_rightLayout;
    QHBoxLayout* m_topBarLayout;
    QPushButton* m_backBtn;
    QPushButton* m_forwardBtn;
    QLineEdit* m_addressBar;
    QListView* m_fileView;
    FileListModel* m_model;


    void setUI();

    private slots:

        /// @brief обработчик нажатия кнопки назад в родительскую директорию
        void onBackClicked();

        /// @brief обработчик нажатия кнопки вперед в дочернюю директорию
        void onAddressReturnPressed();

        /// @brief обработчик двойного нажатия на файл
        void onFileDoubleClicked(const QModelIndex& index); 


};