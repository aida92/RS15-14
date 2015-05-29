#ifndef UREDJIVANJE_H
#define UREDJIVANJE_H
#include <vector>


class uredjivanje
{
public:
    uredjivanje(int sirina = 170);//ako je osoba sirine 100, damo joj jos 70
    ~uredjivanje();

    std::vector<int> * IzracunajSirinuCelije(std::vector<int> &m, int maxPoCeliji);

    int Sirina();


private:
    /*!
     * \brief _sirina neka osnovna sirina za jednu osobu
     */
    int _sirina;


    /*!
     * \brief IzracunajUkupnuSirinu za dat vektor iz engina,koji ima broj krvih srodnika po nivou, racuna sirinu potrebnu da se lepo smeste
     * \param m vektor sa brojevima krvnih srodnika po generaciji
     * \param maxPoCeliji maksimalan broj osoba koji se smešta u ćeliju (maks broj supružnika plus 1)
     * \return
     */
    int IzracunajUkupnuSirinu(std::vector<int> &m, int maxPoCeliji);

    /*!
     * \brief _ukupnaSirina
     */
    int _ukupnaSirina;
};
#endif // UREDJIVANJE_H
