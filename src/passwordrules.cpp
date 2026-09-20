#include "passwordrules.h"

#include <QStringList>

/* Множество знаков препинания взято из указаний по выполнению лабораторных
   работ (раздел Object Pascal). Функция ispunct() здесь не используется
   намеренно: она относит к знакам препинания и знаки арифметических операций
   (+ * / %), которые в задании выделены в отдельную категорию. */
static const QString kPunctuation = QStringLiteral(".,;:-!?()\"'");

QString punctuationChars()
{
    return kPunctuation;
}

bool checkPassword(const QString &pass)
{
    // признаки наличия в пароле требуемых групп символов
    bool digit = false;
    bool punct = false;

    for (const QChar ch : pass) {
        if (ch.isDigit())
            digit = true;
        else if (kPunctuation.contains(ch))
            punct = true;
    }

    // вариант №5: в пароле должны присутствовать обе группы символов
    return digit && punct;
}

QString passwordRuleDescription()
{
    QStringList shown;
    for (const QChar ch : kPunctuation)
        shown << QString(ch);

    return QStringLiteral("Вариант №5: пароль должен содержать цифры и знаки препинания.\n"
                          "Допустимые знаки препинания: %1")
            .arg(shown.join(QStringLiteral(" ")));
}
