#ifndef DIALOGNOVAOSOBA_H
#define DIALOGNOVAOSOBA_H

#include <QDialog>
#include <QPushButton>
#include <QDate>
#include <QString>
#include <QChar>
#include <QTextEdit>
#include <QLabel>


namespace Ui {
class DialogNovaOsoba;
}

class DialogNovaOsoba : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNovaOsoba(QWidget *parent = 0);
    ~DialogNovaOsoba();

    bool popuniPodatke(QString &ime, QString &prezime, QChar &pol, QDate &rodjenje, QDate &smrt);

    void retranslate();
private Q_SLOTS:
    void promenaUnosa();

    void on_chkSmrt_stateChanged(int arg1);
    void on_chkNepoznatDR_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::DialogNovaOsoba *ui;

    QPushButton *ok, *cancel;

    void popuniDugmice();
    void postaviProvere();
};

#endif // DIALOGNOVAOSOBA_H
