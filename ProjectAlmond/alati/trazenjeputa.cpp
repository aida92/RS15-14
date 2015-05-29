#include "trazenjeputa.h"



TrazenjePuta::TrazenjePuta(PorodicnoStablo* stablo)
    :_stablo(stablo),_putevi(nullptr),_duzine(nullptr),_sifre(nullptr),_n(0)
{}
TrazenjePuta::~TrazenjePuta()
{
    if(_duzine!=nullptr){
        for(int i=0;i<_n;i++)
            delete[] _duzine[i];
        delete [] _duzine;
    }
    if(_putevi!=nullptr){
        for(int i=0;i<_n;i++)
            delete[] _putevi[i];
        delete [] _putevi;
    }
    if(_sifre!=nullptr)
        delete []_sifre;
}

std::vector<short>& TrazenjePuta::operator()(short sifraPocetne,short sifraTrazene)
{

    _put.clear();
    int tren=rBr(sifraTrazene),kraj=rBr(sifraPocetne);
    while(tren!=kraj)
    {
        _put.push_back(_sifre[tren]);
        tren=_putevi[kraj][tren];
    }
    _put.push_back(_sifre[kraj]);

    std::reverse(_put.begin(),_put.end());

    //std::cout<<sifraPocetne<<" --> "<<sifraTrazene<<std::endl;
   /* std::cout<<"Put izmedju trazenih osoba: ";
    for(int i=0;i<_put.size();i++)std::cout<<_put[i]<<' ';
    std::cout<<std::endl;
*/
    return _put;
}

QString TrazenjePuta::tipSrodstva(short sifraPocetne, short sifraTrazene)
{
    std::vector<short>& osobeIzmedju=operator()(sifraPocetne,sifraTrazene);
    int duzinaPuta=osobeIzmedju.size();
    if(duzinaPuta<2)return QString(tr("greska"));
    bool trazenaJeZensko=_stablo->NadjiOsobuSifrom(sifraTrazene)->Pol()!='M';
    //bool trazenaJeNasa=_stablo->NadjiOsobuSifrom(sifraTrazene)->JeKrvniSrodnik();
    std::vector<short>::const_iterator prva=osobeIzmedju.cbegin();
    std::vector<short>::const_iterator druga=osobeIzmedju.cbegin();
    std::vector<short>::const_iterator kraj=osobeIzmedju.cend();
    druga++;
    int razlika=0;

    for(;druga!=kraj;++prva,++druga)
    {
        if(_stablo->jeDeteOd(*prva,*druga))razlika++;
        else if(_stablo->jeRoditeljOd(*prva,*druga))razlika--;
    }

    switch(duzinaPuta)
    {
    case 2:
        if(_stablo->jeDeteOd(sifraPocetne,sifraTrazene))
            if(trazenaJeZensko)
                return QString(tr("cerka"));
            else
                return QString(tr("sin"));
        else if(_stablo->jeRoditeljOd(sifraPocetne,sifraTrazene))
            if(trazenaJeZensko)
                return QString(tr("majka"));
            else
                return QString(tr("otac"));


         else
            if(trazenaJeZensko)
                return QString(tr("zena"));
            else
                return QString(tr("muz"));
        break;
    case 3:
        if(_stablo->jeDeteOd(sifraPocetne,osobeIzmedju[1]) && _stablo->jeSupruznikOd(osobeIzmedju[1],sifraTrazene)){
            if(trazenaJeZensko)
                return QString(tr("snaja"));
            else
                return QString(tr("zet"));

        }
       else if(_stablo->jeSupruznikOd(sifraPocetne,osobeIzmedju[1]) && _stablo->jeRoditeljOd(osobeIzmedju[1],sifraTrazene)){
            if(_stablo->NadjiOsobuSifrom(osobeIzmedju[1])->Pol()=='M'){
                if(trazenaJeZensko)
                    return QString(tr("svekrva"));
                else
                    return QString(tr("svekar"));
            }
            else
            {
                if(trazenaJeZensko)
                    return QString(tr("tasta"));
                else
                    return QString(tr("tast"));
            }


        }
        else if(_stablo->jeRoditeljOd(sifraPocetne,osobeIzmedju[1]) && _stablo->jeBratSestraOd(osobeIzmedju[1],osobeIzmedju[2]))
        {
            if(_stablo->NadjiOsobuSifrom(osobeIzmedju[1])->Pol()!='M')
            {
                if(trazenaJeZensko)
                    return QString(tr("tetka"));
                else
                    return QString(tr("ujak"));
            }
            else
            {
                if(trazenaJeZensko)
                    return QString(tr("tetka"));
                else
                    return QString(tr("stric"));
            }

        }

        else if(razlika==-2)
        {
            if(trazenaJeZensko)
                return QString(tr("baba"));
            else
                return QString(tr("deda"));
        }
        else if(razlika==2)
        {
            if(trazenaJeZensko)
                return QString(tr("unuka"));
            else
                return QString(tr("unuk"));
        }
        else if(_stablo->jeBratSestraOd(sifraPocetne,sifraTrazene)){
            if(trazenaJeZensko)
                return QString(tr("sestra"));
            else
                return QString(tr("brat"));}
        else if(razlika==-1)
        {
            if(_stablo->NadjiOsobuSifrom(osobeIzmedju[1])->Pol()!='M')
            {
                if(trazenaJeZensko)
                    return QString(tr("ujna"));
                else
                    return QString(tr("teca"));
            }
            else
            {
                if(trazenaJeZensko)
                    return QString(tr("strina"));
                else
                    return QString(tr("teca"));
            }

        }

        break;
    case 4:
        if(razlika==-3){
            if(trazenaJeZensko)
                return QString(tr("prababa"));
            else
                return QString(tr("pradeda"));
        }
        else if(razlika==3){
            if(trazenaJeZensko)
                return QString(tr("praunuka"));
            else
                return QString(tr("praunuk"));
        }
        else if(_stablo->jeBratSestraOd(sifraPocetne,osobeIzmedju[2]) && _stablo->jeDeteOd(osobeIzmedju[2],sifraTrazene))
        {
            if(_stablo->NadjiOsobuSifrom(osobeIzmedju[2])->Pol()!='M')
            {
                if(trazenaJeZensko)
                    return QString(tr("sestricina"));
                else
                    return QString(tr("sestric"));
            }
            else
            {
                if(trazenaJeZensko)
                    return QString(tr("bratanica"));
                else
                    return QString(tr("bratanac"));
            }

        }
        else if(razlika==-1)
        {
            if(_stablo->NadjiOsobuSifrom(osobeIzmedju[1])->Pol()!='M')
            {
                if(trazenaJeZensko)
                    return QString(tr("ujna"));
                else
                    return QString(tr("teca"));
            }
            else
            {
                if(trazenaJeZensko)
                    return QString(tr("strina"));
                else
                    return QString(tr("teca"));
            }

        }

        break;
    case 5:
        if(razlika==4){
            if(trazenaJeZensko)
                return QString(tr("cukunbaba"));
            else
                return QString(tr("cukundeda"));
        }
        else if(razlika==-3){
            if(trazenaJeZensko)
                return QString(tr("cukununuka"));
            else
                return QString(tr("cukununuk"));
        }


        break;
    default:
        if(trazenaJeZensko)
            return QString(tr("daleka rodjaka"));
        else
            return QString(tr("daleki rodjak"));
        break;

    }



    return QString(tr("daleki rodjak"));
}

void TrazenjePuta::OsveziMatricuPuteva()
{
    int n=_stablo->Osobe().size();

    InicijalizujMatricu(&_duzine,n);
    InicijalizujMatricu(&_putevi,n);
    _n=n;
    _sifre=new short[n];

    auto it=_stablo->Osobe().cbegin();

    for(int i=0;i<n;++i,++it)
    {
        _sifre[i]=(*it).first;

    }


    for(int i=0;i<n;i++){


        _duzine[i][i]=0;
        _putevi[i][i]=_sifre[i];


    }
  // std::cout<<_stablo->Brakovi().size()<<std::endl;
    std::map<short, Brak*>::iterator brak=_stablo->Brakovi().begin();
    std::map<short, Brak*>::iterator krajBraka=_stablo->Brakovi().end();
    //postavljanje direktnih veza
    for(;brak!=krajBraka;++brak){
        short sNase=(*brak).second->SifraNase();
        short sTudje=(*brak).second->SifraTudje();


        int rb1=rBr(sNase);
        int rb2=rBr(sTudje);

        int rbd=-1;
        _duzine[rb1][rb2]=1;
        _duzine[rb2][rb1]=1;
        _putevi[rb1][rb2]=rb1;
        _putevi[rb2][rb1]=rb2;
       // std::cout<<"SIFRA BRAKA KOJI GLEDAM: "<<(*brak).first<<std::endl;
        std::pair<std::multimap<short,short>::iterator,std::multimap<short,short>::iterator> decaBraka=_stablo->BrakDeca().equal_range((*brak).first);
        std::multimap<short,short>::iterator i=decaBraka.first;
        for(;i!=decaBraka.second;++i){

            Osoba * osoba=_stablo->NadjiOsobuSifrom((*i).second);
            if(osoba!=nullptr){
                rbd=rBr(osoba->Sifra());
                if(rbd>0){
                    _duzine[rb1][rbd]=1;
                    _duzine[rb2][rbd]=1;
                    _duzine[rbd][rb1]=1;
                    _duzine[rbd][rb2]=1;
                    _putevi[rb1][rbd]=rb1;
                    _putevi[rb2][rbd]=rb2;
                    _putevi[rbd][rb1]=rbd;
                    _putevi[rbd][rb2]=rbd;}
            }}

    }

    for(int k=0;k<n;k++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(_duzine[i][k]!=-1 && _duzine[k][j]!=-1 && (_duzine[i][j]==-1 || _duzine[i][j]>_duzine[i][k]+_duzine[k][j]))
                {
                    _duzine[i][j]=_duzine[i][k]+_duzine[k][j];
                    _putevi[i][j]=k;
                }
            }
        }
    }

    /*
    for(int i=0;i<n;i++){

        for(int j=0;j<n;j++)
        {
            std::cout<<_putevi[i][j]<<",";
        }
        std::cout<<std::endl;
    }

    std::cout<<std::endl;
    std::cout<<std::endl;
    for(int i=0;i<n;i++){

        for(int j=0;j<n;j++)
        {
            std::cout<<_duzine[i][j]<<",";
        }
        std::cout<<std::endl;
    }

*/
    //ovime sam popunio matricu duzina i putanja, sada mogu da je koristim pozivima funkcija

    /*
    let dist be a |V| × |V| array of minimum distances initialized to ∞ (infinity)
    2 for each vertex v
    3    dist[v][v] ← 0
    4 for each edge (u,v)
    5    dist[u][v] ← w(u,v)  // the weight of the edge (u,v)
    6 for k from 1 to |V|
    7    for i from 1 to |V|
    8       for j from 1 to |V|
    9          if dist[i][j] > dist[i][k] + dist[k][j]
    10             dist[i][j] ← dist[i][k] + dist[k][j]
    11         end if

*/
}

void TrazenjePuta::InicijalizujMatricu(short ***m, int n)
{
    if((*m)!=nullptr&&_n!=n){
        for(int i=0;i<_n;i++)
            delete[] (*m)[i];
        delete [] (*m);
    }
    *m=new short*[n];
    for(int i=0;i<n;++i)
    {
        (*m)[i]=new short[n];
        for(int j=0;j<n;j++)
        {
            (*m)[i][j]=-1;
        }
    }
}

int TrazenjePuta::rBr( short sifra) const
{

    for(int i=0;i<_n;i++)
    {
        if(_sifre[i]==sifra)return i;

    }
    return -1;
}
