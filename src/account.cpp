#include "account.h"

#include "cryptofile.h"

#include <QFile>

#include <cstring>

QString accountName(const AccountType &acc)
{
    // имя всегда завершается нулем, поэтому strnlen дает его фактическую длину
    return QString::fromUtf8(acc.UserName, strnlen(acc.UserName, MAXNAME));
}

QString accountPass(const AccountType &acc)
{
    return QString::fromUtf8(acc.UserPass, strnlen(acc.UserPass, MAXPASS));
}

bool nameFits(const QString &name)
{
    return name.toUtf8().size() < MAXNAME;
}

bool passFits(const QString &pass)
{
    return pass.toUtf8().size() < MAXPASS;
}

void setAccountName(AccountType &acc, const QString &name)
{
    const QByteArray raw = name.toUtf8();
    memset(acc.UserName, 0, MAXNAME);
    // копируется не более MAXNAME-1 байт, последний байт остается нулевым
    memcpy(acc.UserName, raw.constData(), qMin<int>(raw.size(), MAXNAME - 1));
}

void setAccountPass(AccountType &acc, const QString &pass)
{
    const QByteArray raw = pass.toUtf8();
    memset(acc.UserPass, 0, MAXPASS);
    const int len = qMin<int>(raw.size(), MAXPASS - 1);
    memcpy(acc.UserPass, raw.constData(), len);
    acc.PassLen = len;
}

AccountStore::AccountStore(const QString &fileName)
    : m_fileName(fileName)
{
}

AccountStore::~AccountStore()
{
    // расшифрованные данные и парольная фраза не должны остаться в памяти
    wipe(m_data);
    wipe(m_passphrase);
}

bool AccountStore::exists() const
{
    return QFile::exists(m_fileName);
}

void AccountStore::createWithAdmin(const QString &passphrase)
{
    m_passphrase = passphrase.toUtf8();

    // подготовка учетной записи администратора
    AccountType admin;
    memset(&admin, 0, sizeof(admin));
    setAccountName(admin, QStringLiteral(ADMINNAME));
    setAccountPass(admin, QString());
    admin.Block = false;
    admin.Restrict = true;

    m_data = QByteArray(reinterpret_cast<const char *>(&admin), sizeof(admin));
}

AccountStore::OpenResult AccountStore::open(const QString &passphrase)
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly))
        return IoError;
    const QByteArray blob = file.readAll();
    file.close();

    const QByteArray pass = passphrase.toUtf8();
    QByteArray plain;
    if (!decryptData(blob, pass, plain) || plain.size() % sizeof(AccountType) != 0)
        return WrongPassphrase;

    /* при неверной парольной фразе получается набор случайных байт, поэтому
       правильность определяется по наличию учетной записи администратора */
    m_data = plain;
    AccountType admin;
    unsigned rec = 0;
    if (!find(QStringLiteral(ADMINNAME), admin, rec)) {
        wipe(plain);
        wipe(m_data);
        m_data.clear();
        return WrongPassphrase;
    }
    wipe(plain);

    m_passphrase = pass;
    return Opened;
}

bool AccountStore::save()
{
    const QByteArray blob = encryptData(m_data, m_passphrase);

    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadWrite))
        return false;

    // стирание прежнего содержимого файла
    const qint64 oldSize = file.size();
    if (oldSize > 0 && file.write(QByteArray(oldSize, '\0')) != oldSize)
        return false;
    file.flush();

    file.seek(0);
    file.resize(0);
    return file.write(blob) == blob.size() && file.flush();
}

bool AccountStore::find(const QString &name, AccountType &acc, unsigned &rec) const
{
    // последовательный просмотр учетных записей и сравнение имен с искомым
    for (unsigned i = 0; i < count(); ++i) {
        AccountType current;
        memcpy(&current, m_data.constData() + i * sizeof(AccountType), sizeof(current));
        if (accountName(current) == name) {
            acc = current;
            rec = i;
            return true;
        }
    }
    return false;
}

std::vector<AccountType> AccountStore::readAll() const
{
    std::vector<AccountType> result(count());
    if (!result.empty())
        memcpy(result.data(), m_data.constData(), result.size() * sizeof(AccountType));
    return result;
}

bool AccountStore::update(unsigned rec, const AccountType &acc)
{
    if (rec >= count())
        return false;
    // замена учетной записи с номером rec
    m_data.replace(rec * sizeof(AccountType), sizeof(acc),
                   reinterpret_cast<const char *>(&acc), sizeof(acc));
    return true;
}

bool AccountStore::append(const AccountType &acc)
{
    m_data.append(reinterpret_cast<const char *>(&acc), sizeof(acc));
    return true;
}
