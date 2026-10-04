#ifndef PASSPHRASEDIALOG_H
#define PASSPHRASEDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;

/* окно запроса парольной фразы для расшифрования файла с учетными данными.
   Не является автоматически создаваемой формой: объект создается непосредственно
   перед запросом и уничтожается сразу после него, чтобы введенная фраза не
   оставалась в памяти. */
class PassphraseDialog : public QDialog
{
    Q_OBJECT

public:
    // confirm = true: фраза задается впервые и вводится дважды
    explicit PassphraseDialog(bool confirm, QWidget *parent = nullptr);
    ~PassphraseDialog() override;

    QString passphrase() const;

protected:
    void accept() override;

private:
    bool m_confirm;
    QLineEdit *m_pass;
    QLineEdit *m_repeat = nullptr;
};

#endif // PASSPHRASEDIALOG_H
