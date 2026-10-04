#include "cryptofile.h"

#include "md2.h"
#include "rc2.h"

#include <QRandomGenerator>

QByteArray deriveKey(const QByteArray &passphrase, const QByteArray &salt)
{
    return md2(salt + passphrase);
}

QByteArray encryptData(const QByteArray &plain, const QByteArray &passphrase)
{
    // случайное значение берется из генератора операционной системы
    QByteArray salt(SaltSize, Qt::Uninitialized);
    for (int i = 0; i < SaltSize; ++i)
        salt[i] = static_cast<char>(QRandomGenerator::system()->bounded(256));

    QByteArray key = deriveKey(passphrase, salt);
    const Rc2 cipher(key);
    wipe(key);

    return salt + rc2Cfb(cipher, plain, false);
}

bool decryptData(const QByteArray &blob, const QByteArray &passphrase, QByteArray &plain)
{
    if (blob.size() < SaltSize)
        return false;

    QByteArray key = deriveKey(passphrase, blob.left(SaltSize));
    const Rc2 cipher(key);
    wipe(key);

    plain = rc2Cfb(cipher, blob.mid(SaltSize), true);
    return true;
}

void wipe(QByteArray &data)
{
    // detach() гарантирует, что затирается собственный буфер, а не общий
    data.detach();
    volatile char *p = data.data();
    for (int i = 0; i < data.size(); ++i)
        p[i] = 0;
}
