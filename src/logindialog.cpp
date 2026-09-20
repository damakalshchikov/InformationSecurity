#include "logindialog.h"

#include "account.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Вход в систему"));

    m_login = new QLineEdit(this);
    // длина имени ограничена размером поля учетной записи
    m_login->setMaxLength(MAXNAME - 1);

    m_password = new QLineEdit(this);
    // замена на экране символом «*» символов вводимого пароля
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setMaxLength(MAXPASS - 1);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Введите имя:"), m_login);
    form->addRow(QStringLiteral("Введите пароль:"), m_password);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Ok"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &LoginDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &LoginDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    // установка фокуса ввода на редактор для ввода имени учетной записи
    m_login->setFocus();
}

QString LoginDialog::userName() const
{
    return m_login->text();
}

QString LoginDialog::password() const
{
    return m_password->text();
}

void LoginDialog::accept()
{
    if (m_login->text().isEmpty()) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Введите имя учетной записи!"));
        m_login->setFocus();
        return;
    }
    QDialog::accept();
}
