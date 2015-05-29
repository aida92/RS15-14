#ifndef BRAK_H
#define BRAK_H

#include<vector>
#include<QString>
#include<QDate>

class Brak
{
public:
    /*!
     * \brief Brak konstruktor bez argumenata, koji ne radi ništa
     */
    Brak();

    /*!
     * \brief Brak kreira objekat tipa Brak
     * \param sifraNase
     * \param sifraTudje
     * \param trivija
     */
    Brak(const short sifraNase, const short sifraTudje, const QString &trivija);

    Brak(const Brak& drugi);
    ~Brak();

    //----geteri i seteri------//

    short int Sifra();
    short SifraNase();
    short SifraTudje();
    const QString& Trivija() const;

    //----geteri i seteri------//
    /*!
     * \brief postaviSledecuSifru menja statički podatak o tome koja će sledeća šifra biti upotrebljena
     * \param sifra
     */
    static void postaviSledecuSifru(int sifra);

    //----metodi potrebni za citanje i pisanje----//
    friend QDataStream& operator<<(QDataStream &out,Brak& brak);
    friend QDataStream& operator>>(QDataStream &in,Brak& brak);
    //----metodi potrebni za citanje i pisanje----//

    short Nivo() const;
    void Nivo(short nivo);

private:
    static short int _sledecaSifra;
    short int _sifra;
    short _sifraNase;
    short _sifraTudje;
    QString _trivija;


    short _nivo = -1;
};

#endif // BRAK_H
