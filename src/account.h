#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QByteArray>
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

/* Файл учетных записей: зашифрованная последовательность структур AccountType
   фиксированного размера. Номера записей нумеруются с нуля и совпадают с их
   порядком в файле.

   Расшифрованное содержимое хранится только в оперативной памяти: на диск оно
   не попадает. Все операции над записями выполняются над этим буфером, а на
   диск он возвращается в зашифрованном виде методом save(). */
class AccountStore
{
public:
    enum OpenResult {
        Opened,          // файл расшифрован, учетная запись администратора найдена
        IoError,         // файл не удалось прочитать
        WrongPassphrase  // парольная фраза неверна (или файл поврежден)
    };

    explicit AccountStore(const QString &fileName);
    ~AccountStore();

    const QString &fileName() const { return m_fileName; }

    // существует ли файл с учетными записями
    bool exists() const;
    /* создание записей с единственной учетной записью администратора
       (пустой пароль, без блокировки, с ограничениями на пароли) под
       парольной фразой passphrase; на диск они попадают после save() */
    void createWithAdmin(const QString &passphrase);

    /* чтение и расшифрование файла; правильность парольной фразы определяется
       по наличию в расшифрованных данных учетной записи администратора */
    OpenResult open(const QString &passphrase);
    /* шифрование данных на новом случайном значении и запись в файл; прежнее
       содержимое файла перед этим затирается */
    bool save();

    // поиск учетной записи по имени; rec получает номер найденной записи
    bool find(const QString &name, AccountType &acc, unsigned &rec) const;
    // чтение всех учетных записей в порядке их следования в файле
    std::vector<AccountType> readAll() const;

    // перезапись учетной записи с номером rec
    bool update(unsigned rec, const AccountType &acc);
    // добавление учетной записи в конец файла
    bool append(const AccountType &acc);

private:
    // число записей в расшифрованных данных
    unsigned count() const { return static_cast<unsigned>(m_data.size() / sizeof(AccountType)); }

    QString m_fileName;
    QByteArray m_data;       // расшифрованные учетные записи
    QByteArray m_passphrase; // парольная фраза, нужная для повторного шифрования
};

#endif // ACCOUNT_H
