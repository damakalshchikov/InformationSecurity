#include "account.h"
#include "mainwindow.h"
#include "passphrasedialog.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>

namespace {

const char *kTitle = "Лабораторная работа №3";

/* Подготовка файла учетных записей к работе: запрос парольной фразы и
   расшифрование (при первом запуске — задание фразы и создание файла).
   При неверной фразе или отказе от ее ввода работа программы завершается. */
bool unlockStore(AccountStore &store)
{
    const QString title = QString::fromUtf8(kTitle);

    if (!store.exists()) {
        PassphraseDialog dlg(true);
        if (dlg.exec() != QDialog::Accepted)
            return false;
        store.createWithAdmin(dlg.passphrase());
        if (!store.save()) {
            QMessageBox::critical(nullptr, title,
                                  QStringLiteral("Не удалось создать файл учетных записей:\n%1")
                                          .arg(store.fileName()));
            return false;
        }
        return true;
    }

    AccountStore::OpenResult res;
    {
        PassphraseDialog dlg(false);
        if (dlg.exec() != QDialog::Accepted) {
            QMessageBox::warning(nullptr, title,
                                 QStringLiteral("Парольная фраза не введена.\n"
                                                "Работа программы завершается."));
            return false;
        }
        res = store.open(dlg.passphrase());
    } // диалог (и введенная в нем фраза) уничтожается сразу после расшифрования

    if (res == AccountStore::IoError) {
        QMessageBox::critical(nullptr, title,
                              QStringLiteral("Не удалось прочитать файл учетных записей:\n%1")
                                      .arg(store.fileName()));
        return false;
    }
    if (res == AccountStore::WrongPassphrase) {
        QMessageBox::critical(nullptr, title,
                              QStringLiteral("Неверная парольная фраза!\n"
                                             "Работа программы завершается."));
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8(kTitle));

    // файл с учетными записями располагается рядом с исполняемым файлом
    AccountStore store(QCoreApplication::applicationDirPath() + QStringLiteral("/" SECFILE));
    if (!unlockStore(store))
        return 1;

    MainWindow window(store);
    window.show();

    return app.exec();
}
