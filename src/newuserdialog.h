#ifndef NEWUSERDIALOG_H
#define NEWUSERDIALOG_H

#include <QDialog>

class AccountStore;
class QLineEdit;

// окно добавления нового пользователя
class NewUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewUserDialog(const AccountStore &store, QWidget *parent = nullptr);

    QString userName() const;

protected:
    // имя нового пользователя должно быть непустым и уникальным
    void accept() override;

private:
    const AccountStore &m_store;
    QLineEdit *m_userName;
};

#endif // NEWUSERDIALOG_H
