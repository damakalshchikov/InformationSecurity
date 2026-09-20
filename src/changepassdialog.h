#ifndef CHANGEPASSDIALOG_H
#define CHANGEPASSDIALOG_H

#include <QDialog>

#include "account.h"

class QLabel;
class QLineEdit;

/* Окно установки (смены) пароля. Старый пароль запрашивается только у тех
   учетных записей, у которых он уже задан: при первом входе пароля еще нет. */
class ChangePassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePassDialog(const AccountType &acc, QWidget *parent = nullptr);

    QString newPassword() const;

protected:
    void accept() override;

private:
    AccountType m_acc;
    QLabel *m_oldLabel;
    QLineEdit *m_oldPass;
    QLineEdit *m_newPass;
    QLineEdit *m_confirmPass;
};

#endif // CHANGEPASSDIALOG_H
