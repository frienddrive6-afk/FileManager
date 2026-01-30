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
class QStandardItemModel;
class QAction;



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
///@param m_sideBarModel модель списка папок
///@param m_copyAction действие копирования ctrl + c
///@param m_cutAction действие вырезания ctrl + x
///@param m_pasteAction действие вставки ctrl + v
///@param m_deleteAction действие удаления del
///@param m_secectAll действие выделить все ctrl + a
///@param m_showHiddenFiles true - показывать все, false - скрывать файлы
///@param m_currentIconSize текущий размер иконок по умолчанию 100
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AppCore& core,QWidget* parent = nullptr);

    /// @brief обновляет информацию о файлах для отображения в интерфейс
    /// @param state текущее состояние навигации
    void updateView(const NavigationState& state);

    /// @brief проверяет является ли системная тема темной
    static bool isSystemThemeDark();
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
    QStandardItemModel* m_sideBarModel;
    QPushButton* m_menuBtn;
    // QPushButton* m_starBtn;

    QAction* m_copyAction;
    QAction* m_cutAction;
    QAction* m_pasteAction;
    QAction* m_deleteAction;
    QAction* m_secectAll;

    bool m_showHiddenFiles;
    int m_currentIconSize;
    
    bool m_isDark;


    /// @brief установка интерфейса
    void setUI();

    /// @brief установка стилей
    void setStyleSheetsForMainWindow();

    /// @brief установка действий
    void setupActions();

    /// @brief получает текст с клавиатуры в модальном окне
    QString getUserInput(QString defaultText);

    /// @brief упрощенная функция для создания QAction с shotcut
    /// @param text Названия действия
    /// @param shortcut к каким клавишам будет привязан
    /// @param slot функция которая будет вызываться при нажатии
    /// @return созданный QAction
    QAction* createAction(QString text, QKeySequence shortcut, std::function<void()> slot);

    /// @brief изменяет размер иконок
    /// @param delta прибавление или отнимание 
    void changeIconSize(int delta);

    int getCoreIndex(int uiIndex);

    

    void updateIcons();

    private slots:

        /// @brief обработчик нажатия кнопки назад в родительскую директорию
        void onBackClicked();

        /// @brief обработчик нажатия кнопки вперед
        void onForwardClicked();

        /// @brief обработчик нажатия кнопки вперед в дочернюю директорию
        void onAddressReturnPressed();

        /// @brief обработчик двойного нажатия на файл
        void onFileDoubleClicked(const QModelIndex& index);
        
        /// @brief переходит в указанную директорию
        /// @param index индекс директории типа QModelIndex
        void goToDirectory(const QModelIndex& index);

        /// @brief обработчик изменения выделения
        void onSelectionChanged();

        /// @brief обработчик контекстного меню
        /// @param pos позиция где создавать
        void onContextMenuRequested(const QPoint &pos);

        /// @brief показывает диалог свойств
        /// @param path путь к файлу
        /// @param icon иконка которая будет показана в свойствах
        void showPropertiesDialog(const std::string& path, const QIcon& icon = QIcon());




protected:

    /// @brief обработка события смены темы
    /// @param event событие
    void changeEvent(QEvent *event) override;

};