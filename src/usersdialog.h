#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>

class AccountStore;
class QTableWidget;

/* Окно просмотра (редактирования) учетных записей. Весь список выводится
   целиком в одном окне; администратор может изменить признак блокировки и
   признак ограничений на выбираемые пароли для любой учетной записи. */
class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(AccountStore &store, QWidget *parent = nullptr);

private slots:
    void save();

private:
    void reload();

    AccountStore &m_store;
    QTableWidget *m_table;
};

#endif // USERSDIALOG_H
