#include "newuserdialog.h"

#include "account.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

NewUserDialog::NewUserDialog(const AccountStore &store, QWidget *parent)
    : QDialog(parent)
    , m_store(store)
{
    setWindowTitle(QStringLiteral("Добавление пользователя"));

    m_userName = new QLineEdit(this);
    m_userName->setMaxLength(MAXNAME - 1);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Имя нового пользователя"), m_userName);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Ok"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &NewUserDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &NewUserDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    // установка фокуса ввода на редактор для ввода имени пользователя
    m_userName->setFocus();
}

QString NewUserDialog::userName() const
{
    return m_userName->text();
}

void NewUserDialog::accept()
{
    const QString name = m_userName->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Введите имя нового пользователя!"));
        m_userName->setFocus();
        return;
    }

    if (!nameFits(name)) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Имя слишком длинное для учетной записи!"));
        m_userName->setFocus();
        m_userName->selectAll();
        return;
    }

    // проверка уникальности введенного имени
    AccountType existing;
    unsigned rec = 0;
    if (m_store.find(name, existing, rec)) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Пользователь %1\nуже зарегистрирован!").arg(name));
        m_userName->setFocus();
        m_userName->selectAll();
        return;
    }

    QDialog::accept();
}
