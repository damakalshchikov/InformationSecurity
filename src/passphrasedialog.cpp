#include "passphrasedialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

PassphraseDialog::PassphraseDialog(bool confirm, QWidget *parent)
    : QDialog(parent)
    , m_confirm(confirm)
{
    setWindowTitle(confirm ? QStringLiteral("Создание базы данных")
                           : QStringLiteral("Расшифрование базы данных"));
    setMinimumWidth(360);

    m_pass = new QLineEdit(this);
    // символы парольной фразы на экране заменяются на «*»
    m_pass->setEchoMode(QLineEdit::Password);

    auto *layout = new QVBoxLayout(this);
    if (confirm) {
        layout->addWidget(new QLabel(
                QStringLiteral("Файл учетных записей не найден. Задайте парольную фразу,\n"
                               "на основе которой он будет зашифрован:"), this));
        layout->addWidget(m_pass);
        m_repeat = new QLineEdit(this);
        m_repeat->setEchoMode(QLineEdit::Password);
        layout->addWidget(new QLabel(QStringLiteral("Повторите парольную фразу:"), this));
        layout->addWidget(m_repeat);
    } else {
        layout->addWidget(new QLabel(
                QStringLiteral("Пароль для расшифрования базы учетных записей:"), this));
        layout->addWidget(m_pass);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Ok"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &PassphraseDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &PassphraseDialog::reject);
    layout->addWidget(buttons);

    m_pass->setFocus();
}

PassphraseDialog::~PassphraseDialog()
{
    // введенная фраза затирается в полях ввода до их уничтожения
    m_pass->clear();
    if (m_repeat)
        m_repeat->clear();
}

QString PassphraseDialog::passphrase() const
{
    return m_pass->text();
}

void PassphraseDialog::accept()
{
    if (m_pass->text().isEmpty()) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Введите парольную фразу!"));
        m_pass->setFocus();
        return;
    }
    if (m_confirm && m_pass->text() != m_repeat->text()) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Парольные фразы не совпадают!"));
        m_repeat->clear();
        m_repeat->setFocus();
        return;
    }
    QDialog::accept();
}
