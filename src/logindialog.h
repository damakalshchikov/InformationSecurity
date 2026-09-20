#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;

// окно входа в программу: запрос имени учетной записи и пароля
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString userName() const;
    QString password() const;

protected:
    // окно закрывается по кнопке «Ok», только если введено имя учетной записи
    void accept() override;

private:
    QLineEdit *m_login;
    QLineEdit *m_password;
};

#endif // LOGINDIALOG_H
