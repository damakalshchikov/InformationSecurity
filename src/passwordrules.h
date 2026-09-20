#ifndef PASSWORDRULES_H
#define PASSWORDRULES_H

#include <QString>

/* Индивидуальное задание, вариант №5: наличие цифр и знаков препинания.
   Проверка выполняется только для учетных записей, у которых администратор
   включил признак ограничений на выбираемые пароли. */
bool checkPassword(const QString &pass);

// множество знаков препинания, используемое при проверке
QString punctuationChars();

// краткое описание ограничения для сообщений об ошибке и окна «О программе»
QString passwordRuleDescription();

#endif // PASSWORDRULES_H
