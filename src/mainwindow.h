#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "account.h"

class QAction;
class QPushButton;

// главная форма программы
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AccountStore &store, QWidget *parent = nullptr);

protected:
    // при завершении работы учетные данные шифруются и записываются в файл
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLogin();          // кнопка «Вход в систему»
    void onChangePassword(); // команда меню «Смена пароля»
    void onNewUser();        // команда меню «Новый пользователь»
    void onAllUsers();       // команда меню «Все пользователи»
    void onAbout();          // команда меню «О программе»

private:
    void createMenus();
    // приведение доступности команд меню в соответствие с полномочиями
    void applyPermissions();

    AccountStore &m_store; // уже расшифрованные учетные записи
    AccountType m_current {};  // учетная запись вошедшего пользователя
    unsigned m_currentRec = 0; // номер его записи в файле
    bool m_loggedIn = false;
    unsigned m_enterCount = 0; // счетчик неверных попыток входа

    QPushButton *m_loginButton;
    QAction *m_changeAct;
    QAction *m_newUserAct;
    QAction *m_allUsersAct;
};

#endif // MAINWINDOW_H
