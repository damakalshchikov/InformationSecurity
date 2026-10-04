#ifndef CRYPTOFILE_H
#define CRYPTOFILE_H

#include <QByteArray>

/* Защита файла с учетными записями (вариант №5): блочный шифр RC2 в режиме
   обратной связи по шифротексту (CFB), хеширование парольной фразы по MD2,
   к ключу добавляется случайное значение (соль).

   Формат зашифрованных данных:
       соль (11 байт) | шифротекст (длина равна длине открытых данных)
   Соль не секретна и хранится открыто, при каждой записи она новая. */

// длина случайного значения, добавляемого к ключу
const int SaltSize = 11;

/* получение сеансового ключа (128 бит) из парольной фразы:
   MD2(соль + парольная фраза) */
QByteArray deriveKey(const QByteArray &passphrase, const QByteArray &salt);

// шифрование данных на ключе, выработанном из парольной фразы
QByteArray encryptData(const QByteArray &plain, const QByteArray &passphrase);
// расшифрование; false, если данные слишком коротки для заголовка с солью
bool decryptData(const QByteArray &blob, const QByteArray &passphrase, QByteArray &plain);

// затирание содержимого буфера перед освобождением (секретные данные)
void wipe(QByteArray &data);

#endif // CRYPTOFILE_H
