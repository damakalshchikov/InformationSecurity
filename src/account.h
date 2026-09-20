#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <vector>

// максимальная длина имени учетной записи (с учетом завершающего нуля)
#define MAXNAME 20
// максимальная длина пароля (с учетом завершающего нуля)
#define MAXPASS 10

// имя файла с учетными записями пользователей
#define SECFILE "security.db"

// имя учетной записи администратора
#define ADMINNAME "ADMIN"

// структурный тип для хранения учетной записи
struct AccountType
{
    char UserName[MAXNAME]; // имя
    int PassLen;            // длина пароля
    char UserPass[MAXPASS]; // пароль
    bool Block;             // признак блокировки учетной записи администратором
    bool Restrict;          // признак включения ограничений на выбираемые пароли
};

// извлечение имени и пароля из учетной записи
QString accountName(const AccountType &acc);
QString accountPass(const AccountType &acc);

/* проверка того, что имя (пароль) помещается в поле учетной записи;
   длина считается в байтах, так как кириллица в UTF-8 занимает по два байта */
bool nameFits(const QString &name);
bool passFits(const QString &pass);

// запись имени и пароля в учетную запись (строка обрезается, если не помещается)
void setAccountName(AccountType &acc, const QString &name);
void setAccountPass(AccountType &acc, const QString &pass);

/* Файл учетных записей: последовательность структур AccountType фиксированного
   размера. Номера записей нумеруются с нуля и совпадают с их порядком в файле. */
class AccountStore
{
public:
    explicit AccountStore(const QString &fileName);

    const QString &fileName() const { return m_fileName; }

    // существует ли файл с учетными записями
    bool exists() const;
    /* создание файла с единственной учетной записью администратора
       (пустой пароль, без блокировки, с ограничениями на пароли) */
    bool createWithAdmin();

    // поиск учетной записи по имени; rec получает номер найденной записи
    bool find(const QString &name, AccountType &acc, unsigned &rec) const;
    // чтение всех учетных записей в порядке их следования в файле
    std::vector<AccountType> readAll() const;

    // перезапись учетной записи с номером rec
    bool update(unsigned rec, const AccountType &acc);
    // добавление учетной записи в конец файла
    bool append(const AccountType &acc);

private:
    QString m_fileName;
};

#endif // ACCOUNT_H
