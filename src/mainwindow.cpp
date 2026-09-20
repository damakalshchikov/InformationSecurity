#include "mainwindow.h"

#include "changepassdialog.h"
#include "logindialog.h"
#include "newuserdialog.h"
#include "passwordrules.h"
#include "usersdialog.h"

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include <cstring>

namespace {

// данные об авторе и выданном индивидуальном задании
const char *kAuthor = "Дамакальщиков М. А.";
const char *kGroup = "ИДБ-23-14";
const char *kTeacher = "Юсеф Фарах";

// максимальное число попыток ввода пароля
const unsigned kMaxEnterCount = 3;

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // файл с учетными записями располагается рядом с исполняемым файлом
    , m_store(QCoreApplication::applicationDirPath() + QStringLiteral("/" SECFILE))
{
    setWindowTitle(QStringLiteral("Лабораторная работа №1"));
    resize(640, 400);

    createMenus();

    m_loginButton = new QPushButton(QStringLiteral("Вход в систему"), this);
    connect(m_loginButton, &QPushButton::clicked, this, &MainWindow::onLogin);

    // кнопка входа размещается по центру главной формы
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->addStretch();
    auto *row = new QHBoxLayout;
    row->addStretch();
    row->addWidget(m_loginButton);
    row->addStretch();
    layout->addLayout(row);
    layout->addStretch();
    setCentralWidget(central);

    /* если файл с учетными записями пользователей не существует
       (первый запуск программы), то он создается автоматически */
    if (!m_store.exists() && !m_store.createWithAdmin()) {
        QMessageBox::critical(this, windowTitle(),
                              QStringLiteral("Не удалось создать файл учетных записей:\n%1")
                                      .arg(m_store.fileName()));
    }

    applyPermissions();
}

void MainWindow::createMenus()
{
    auto *usersMenu = menuBar()->addMenu(QStringLiteral("&Пользователи"));

    m_changeAct = usersMenu->addAction(QStringLiteral("&Смена пароля"),
                                       this, &MainWindow::onChangePassword);
    m_newUserAct = usersMenu->addAction(QStringLiteral("&Новый пользователь"),
                                        this, &MainWindow::onNewUser);
    m_allUsersAct = usersMenu->addAction(QStringLiteral("&Все пользователи"),
                                         this, &MainWindow::onAllUsers);
    usersMenu->addSeparator();
    usersMenu->addAction(QStringLiteral("В&ыход"), this, &QWidget::close);

    auto *helpMenu = menuBar()->addMenu(QStringLiteral("&Справка"));
    helpMenu->addAction(QStringLiteral("&О программе"), this, &MainWindow::onAbout);
}

void MainWindow::applyPermissions()
{
    const bool isAdmin = m_loggedIn && accountName(m_current) == QStringLiteral(ADMINNAME);

    // смена пароля доступна любому вошедшему пользователю
    m_changeAct->setEnabled(m_loggedIn);
    // управление учетными записями доступно только администратору
    m_newUserAct->setEnabled(isAdmin);
    m_allUsersAct->setEnabled(isAdmin);

    m_loginButton->setVisible(!m_loggedIn);

    if (m_loggedIn) {
        statusBar()->showMessage(
                QStringLiteral("Пользователь: %1%2")
                        .arg(accountName(m_current),
                             isAdmin ? QStringLiteral(" (администратор)") : QString()));
    } else {
        statusBar()->showMessage(QStringLiteral("Вход в систему не выполнен"));
    }
}

void MainWindow::onLogin()
{
    LoginDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    AccountType acc;
    unsigned rec = 0;

    // если совпадения имен не найдено, то пользователь не зарегистрирован
    if (!m_store.find(dlg.userName(), acc, rec)) {
        QMessageBox::warning(this, QStringLiteral("Вход в систему"),
                             QStringLiteral("Вы не зарегистрированы!"));
        return;
    }

    if (acc.PassLen == 0) {
        /* пароль отсутствует (первый вход пользователя в программу):
           его необходимо установить с подтверждением повторным вводом */
        ChangePassDialog pwd(acc, this);
        if (pwd.exec() != QDialog::Accepted)
            return;

        setAccountPass(acc, pwd.newPassword());
        if (!m_store.update(rec, acc)) {
            QMessageBox::critical(this, QStringLiteral("Вход в систему"),
                                  QStringLiteral("Не удалось сохранить пароль!"));
            return;
        }
    } else if (accountPass(acc) != dlg.password()) {
        // пароли не совпадают
        if (++m_enterCount >= kMaxEnterCount) {
            QMessageBox::critical(this, QStringLiteral("Вход в систему"),
                                  QStringLiteral("Вход в программу невозможен!"));
            close();
            return;
        }
        QMessageBox::warning(this, QStringLiteral("Вход в систему"),
                             QStringLiteral("Неверный пароль!"));
        return;
    }

    // если учетная запись заблокирована администратором
    if (acc.Block) {
        QMessageBox::warning(this, QStringLiteral("Вход в систему"),
                             QStringLiteral("Вы заблокированы!"));
        return;
    }

    m_enterCount = 0;
    m_current = acc;
    m_currentRec = rec;
    m_loggedIn = true;
    applyPermissions();
}

void MainWindow::onChangePassword()
{
    /* учетная запись перечитывается из файла: администратор мог изменить
       признак ограничений на выбираемые пароли уже после входа */
    AccountType acc;
    unsigned rec = 0;
    if (!m_store.find(accountName(m_current), acc, rec)) {
        QMessageBox::critical(this, QStringLiteral("Смена пароля"),
                              QStringLiteral("Учетная запись не найдена!"));
        return;
    }

    ChangePassDialog dlg(acc, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    setAccountPass(acc, dlg.newPassword());
    if (!m_store.update(rec, acc)) {
        QMessageBox::critical(this, QStringLiteral("Смена пароля"),
                              QStringLiteral("Не удалось сохранить пароль!"));
        return;
    }

    m_current = acc;
    m_currentRec = rec;
    QMessageBox::information(this, QStringLiteral("Смена пароля"),
                             QStringLiteral("Пароль изменен."));
}

void MainWindow::onNewUser()
{
    NewUserDialog dlg(m_store, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    // новая учетная запись создается с пустым паролем
    AccountType acc;
    memset(&acc, 0, sizeof(acc));
    setAccountName(acc, dlg.userName());
    setAccountPass(acc, QString());
    acc.Block = false;
    acc.Restrict = true;

    if (!m_store.append(acc)) {
        QMessageBox::critical(this, QStringLiteral("Новый пользователь"),
                              QStringLiteral("Не удалось добавить пользователя!"));
        return;
    }

    QMessageBox::information(this, QStringLiteral("Новый пользователь"),
                             QStringLiteral("Пользователь %1 добавлен.").arg(dlg.userName()));
}

void MainWindow::onAllUsers()
{
    UsersDialog dlg(m_store, this);
    dlg.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::information(
            this, QStringLiteral("О программе"),
            QStringLiteral("Разработка программы разграничения полномочий пользователей\n"
                           "на основе парольной аутентификации\n\n"
                           "Автор: %1, группа %2\n"
                           "Преподаватель: %3\n\n"
                           "%4")
                    .arg(QString::fromUtf8(kAuthor), QString::fromUtf8(kGroup),
                         QString::fromUtf8(kTeacher), passwordRuleDescription()));
}
