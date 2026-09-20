#include "usersdialog.h"

#include "account.h"

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

enum Column { ColName = 0, ColBlock = 1, ColRestrict = 2, ColCount = 3 };

// создание ячейки-флажка для признака учетной записи
QTableWidgetItem *makeCheckItem(bool checked, bool enabled)
{
    auto *item = new QTableWidgetItem;
    Qt::ItemFlags flags = Qt::ItemIsUserCheckable;
    if (enabled)
        flags |= Qt::ItemIsEnabled;
    item->setFlags(flags);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

} // namespace

UsersDialog::UsersDialog(AccountStore &store, QWidget *parent)
    : QDialog(parent)
    , m_store(store)
{
    setWindowTitle(QStringLiteral("Список пользователей"));
    resize(560, 320);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(ColCount);
    m_table->setHorizontalHeaderLabels({QStringLiteral("Имя пользователя"),
                                        QStringLiteral("Блокировка"),
                                        QStringLiteral("Парольное ограничение")});
    m_table->horizontalHeader()->setSectionResizeMode(ColName, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);

    auto *buttons = new QDialogButtonBox(this);
    auto *saveButton = buttons->addButton(QStringLiteral("Сохранить"),
                                          QDialogButtonBox::ApplyRole);
    buttons->addButton(QStringLiteral("Ok"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QStringLiteral("Отмена"), QDialogButtonBox::RejectRole);
    connect(saveButton, &QPushButton::clicked, this, &UsersDialog::save);
    connect(buttons, &QDialogButtonBox::accepted, this, &UsersDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &UsersDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_table);
    layout->addWidget(buttons);

    reload();
}

void UsersDialog::reload()
{
    const std::vector<AccountType> accounts = m_store.readAll();

    m_table->setRowCount(static_cast<int>(accounts.size()));
    for (int row = 0; row < static_cast<int>(accounts.size()); ++row) {
        const AccountType &acc = accounts[static_cast<size_t>(row)];
        const bool isAdmin = accountName(acc) == QStringLiteral(ADMINNAME);

        auto *nameItem = new QTableWidgetItem(accountName(acc));
        nameItem->setFlags(Qt::ItemIsEnabled);
        m_table->setItem(row, ColName, nameItem);

        /* учетную запись администратора блокировать нельзя: иначе система
           осталась бы без возможности администрирования */
        m_table->setItem(row, ColBlock, makeCheckItem(acc.Block, !isAdmin));
        m_table->setItem(row, ColRestrict, makeCheckItem(acc.Restrict, true));
    }
}

void UsersDialog::save()
{
    std::vector<AccountType> accounts = m_store.readAll();

    // сохранение в учетных записях сделанных администратором изменений
    for (int row = 0; row < static_cast<int>(accounts.size()); ++row) {
        AccountType acc = accounts[static_cast<size_t>(row)];
        acc.Block = m_table->item(row, ColBlock)->checkState() == Qt::Checked;
        acc.Restrict = m_table->item(row, ColRestrict)->checkState() == Qt::Checked;
        m_store.update(static_cast<unsigned>(row), acc);
    }

    QMessageBox::information(this, windowTitle(),
                             QStringLiteral("Параметры учетных записей сохранены."));
}
