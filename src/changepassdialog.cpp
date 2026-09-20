#include "changepassdialog.h"

#include "passwordrules.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

ChangePassDialog::ChangePassDialog(const AccountType &acc, QWidget *parent)
    : QDialog(parent)
    , m_acc(acc)
{
    setWindowTitle(QStringLiteral("Смена пароля"));

    const bool hasPassword = m_acc.PassLen != 0;

    m_oldPass = new QLineEdit(this);
    m_oldPass->setEchoMode(QLineEdit::Password);
    m_oldPass->setMaxLength(MAXPASS - 1);

    m_newPass = new QLineEdit(this);
    m_newPass->setEchoMode(QLineEdit::Password);
    m_newPass->setMaxLength(MAXPASS - 1);

    m_confirmPass = new QLineEdit(this);
    m_confirmPass->setEchoMode(QLineEdit::Password);
    m_confirmPass->setMaxLength(MAXPASS - 1);

    m_oldLabel = new QLabel(QStringLiteral("Старый пароль"), this);

    auto *form = new QFormLayout;
    form->addRow(m_oldLabel, m_oldPass);
    form->addRow(QStringLiteral("Новый пароль"), m_newPass);
    form->addRow(QStringLiteral("Подтверждение"), m_confirmPass);

    /* если пароль еще не задан (первый вход в программу), то поле старого
       пароля не нужно */
    m_oldLabel->setVisible(hasPassword);
    m_oldPass->setVisible(hasPassword);

    auto *hint = new QLabel(this);
    hint->setWordWrap(true);
    if (m_acc.Restrict)
        hint->setText(passwordRuleDescription());
    else
        hint->setText(QStringLiteral("Ограничения на выбираемые пароли отключены."));

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Ok"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &ChangePassDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ChangePassDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(hint);
    layout->addWidget(buttons);

    // установка фокуса ввода на первый доступный редактор
    if (hasPassword)
        m_oldPass->setFocus();
    else
        m_newPass->setFocus();
}

QString ChangePassDialog::newPassword() const
{
    return m_newPass->text();
}

void ChangePassDialog::accept()
{
    // смена пароля возможна только при правильном вводе старого пароля
    if (m_acc.PassLen != 0 && m_oldPass->text() != accountPass(m_acc)) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Неверный старый пароль!"));
        m_oldPass->setFocus();
        m_oldPass->selectAll();
        return;
    }

    // пустой пароль означал бы, что пароль так и не был установлен
    if (m_newPass->text().isEmpty()) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Пароль не может быть пустым!"));
        m_newPass->setFocus();
        return;
    }

    // новый пароль должен совпадать с его подтверждением
    if (m_newPass->text() != m_confirmPass->text()) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Пароли должны совпадать!"));
        m_newPass->setFocus();
        m_newPass->selectAll();
        return;
    }

    if (!passFits(m_newPass->text())) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Пароль слишком длинный для учетной записи!"));
        m_newPass->setFocus();
        m_newPass->selectAll();
        return;
    }

    // проверка установленных администратором ограничений на выбираемые пароли
    if (m_acc.Restrict && !checkPassword(m_newPass->text())) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Пароль не соответствует ограничениям!\n\n%1")
                                     .arg(passwordRuleDescription()));
        m_newPass->setFocus();
        m_newPass->selectAll();
        return;
    }

    QDialog::accept();
}
