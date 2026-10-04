#ifndef MD2_H
#define MD2_H

#include <QByteArray>

/* Хеш-функция MD2 (RFC 1319): 128-битное хеш-значение.
   В Windows CryptoAPI ей соответствует идентификатор CALG_MD2. */
QByteArray md2(const QByteArray &data);

#endif // MD2_H
