#ifndef RC2_H
#define RC2_H

#include <QByteArray>

/* Блочный шифр RC2 (RFC 2268): блок 8 байт, ключ 1–128 байт.
   В Windows CryptoAPI ему соответствует идентификатор CALG_RC2. */
class Rc2
{
public:
    static const int BlockSize = 8;

    // эффективная длина ключа принимается равной его фактической длине
    explicit Rc2(const QByteArray &key);
    ~Rc2();

    // шифрование одного блока в 8 байт
    void encryptBlock(const unsigned char in[BlockSize], unsigned char out[BlockSize]) const;

private:
    unsigned short m_k[64]; // расширенный ключ
};

/* Шифрование (decrypt=false) и расшифрование (decrypt=true) в режиме
   обратной связи по шифротексту (CFB, как CRYPT_MODE_CFB в CryptoAPI).
   Размер порции обратной связи — 8 бит, поэтому длина данных не меняется и
   дополнение до размера блока не требуется. Начальный вектор нулевой. */
QByteArray rc2Cfb(const Rc2 &cipher, const QByteArray &data, bool decrypt);

#endif // RC2_H
