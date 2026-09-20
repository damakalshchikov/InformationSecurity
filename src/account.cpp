#include "account.h"

#include <QFile>

#include <cstring>
#include <fstream>

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

bool AccountStore::exists() const
{
    return QFile::exists(m_fileName);
}

bool AccountStore::createWithAdmin()
{
    std::ofstream file(m_fileName.toStdString(), std::ios::out | std::ios::binary);
    if (!file)
        return false;

    // подготовка учетной записи администратора
    AccountType admin;
    memset(&admin, 0, sizeof(admin));
    setAccountName(admin, QStringLiteral(ADMINNAME));
    setAccountPass(admin, QString());
    admin.Block = false;
    admin.Restrict = true;

    file.write(reinterpret_cast<const char *>(&admin), sizeof(admin));
    return file.good();
}

bool AccountStore::find(const QString &name, AccountType &acc, unsigned &rec) const
{
    std::ifstream file(m_fileName.toStdString(), std::ios::in | std::ios::binary);
    if (!file)
        return false;

    AccountType current;
    unsigned index = 0;
    // последовательное чтение учетных записей и сравнение имен с искомым
    while (file.read(reinterpret_cast<char *>(&current), sizeof(current))) {
        if (accountName(current) == name) {
            acc = current;
            rec = index;
            return true;
        }
        ++index;
    }
    return false;
}

std::vector<AccountType> AccountStore::readAll() const
{
    std::vector<AccountType> result;
    std::ifstream file(m_fileName.toStdString(), std::ios::in | std::ios::binary);
    if (!file)
        return result;

    AccountType current;
    while (file.read(reinterpret_cast<char *>(&current), sizeof(current)))
        result.push_back(current);
    return result;
}

bool AccountStore::update(unsigned rec, const AccountType &acc)
{
    std::fstream file(m_fileName.toStdString(),
                      std::ios::in | std::ios::out | std::ios::binary);
    if (!file)
        return false;

    // смещение к началу изменяемой учетной записи
    file.seekp(static_cast<std::streamoff>(rec) * sizeof(AccountType), std::ios::beg);
    file.write(reinterpret_cast<const char *>(&acc), sizeof(acc));
    return file.good();
}

bool AccountStore::append(const AccountType &acc)
{
    std::ofstream file(m_fileName.toStdString(),
                       std::ios::out | std::ios::app | std::ios::binary);
    if (!file)
        return false;

    file.write(reinterpret_cast<const char *>(&acc), sizeof(acc));
    return file.good();
}
