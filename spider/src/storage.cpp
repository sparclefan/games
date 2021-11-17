#include "storage.h"
#include "ui_gamewin.h"

const char * Ver = "Spider1.0";

GameStorage::GameStorage(GameWin *pGameWin)
: m_pGameWin(pGameWin)
{

}

GameStorage::~GameStorage()
{

}

bool GameStorage::Save(string filepath)
{
    ofstream of = ofstream(filepath, ios::out | ios::binary);

    if( !of.is_open()) return false;

    SaveGameInfo(of);
    SaveCardList(of, m_pGameWin->m_cards);
    SaveUndoList(of);

    of.close();

    return true;
}

void GameStorage::SaveGameInfo(ofstream &of)
{
    stGameInfo ginfo = {0};
    strcpy( ginfo.verinfo, Ver);
    ginfo.suitnum = m_pGameWin->m_nOpSuit;
    ginfo.score = m_pGameWin->m_score;

    of.write((char *)&ginfo, sizeof ginfo);
}

bool GameStorage::LoadGameInfo(ifstream &ifile, stGameInfo &ginfo )
{
    ifile.read((char *)&ginfo, sizeof ginfo);

    if( strcmp(ginfo.verinfo, Ver) )
        return false;

    return true;
}

bool GameStorage::Load(string filepath)
{
    ifstream ifile = ifstream(filepath, ios::in | ios::binary);
    if( !ifile.is_open()) return false;

    stGameInfo ginfo;
    if( !LoadGameInfo(ifile, ginfo))
        return false;

    m_pGameWin->clearAll();
    LoadCardList(ifile, m_pGameWin->m_cards);
    LoadUndoList(ifile);
    ifile.close();

    return true;
}

void GameStorage::SaveCardList( ofstream &of,  const vector<Card *> &cardlist )
{
    int size = cardlist.size();
    of.write((char *)&size, sizeof size);
    for(auto card : cardlist)
    {
        int suit = static_cast<int>(card->suit());
        int rank = card->rank();
        of.write((char *)&suit, sizeof suit);
        of.write((char *)&rank, sizeof rank);
    }
}

bool GameStorage::LoadCardList( ifstream &ifile, vector<Card *> &cardlist)
{
    int n;
    ifile.read((char *)&n, sizeof n);

    for(int i=0; i<n; i++)
    {
        int suit;
        int rank;
        ifile.read((char *)&suit, sizeof suit);
        ifile.read((char *)&rank, sizeof rank);

        Card *card = new Card( static_cast<Suit>(suit), rank, m_pGameWin->ui->frame, FALSE );
        card->setId(i);
        cardlist.push_back(card);
    }
    return true;
}

int GameStorage::GetPileIndex(Pile *pile)
{
    if( pile == &m_pGameWin->m_orgPile )
        return 0;

    for(int i=0; i<10; i++)
    {
        if( pile == &(m_pGameWin->m_workPiles[i]) )
            return i+1;
    }

    for(int i=0; i<8; i++)
    {
        if( pile == &(m_pGameWin->m_sucessPile[i]) )
            return i+11;
    }

    return -1;
}

Pile *GameStorage::GetPile(int index)
{
    if( index < 0)
        return nullptr;
    else if( index == 0)
        return &m_pGameWin->m_orgPile;
    else if( index < 11)
        return &m_pGameWin->m_workPiles[index-1];
    else if( index < 19)
        return &m_pGameWin->m_sucessPile[index-11];
    else
        return nullptr;
    
}

bool GameStorage::SaveOp(ofstream &of, Op *op)
{
    int pilefrom = GetPileIndex(op->m_pilefrom);
    int pileto = GetPileIndex(op->m_pileto);

    if( pilefrom<0 || pileto<0 )
        return false;

    of.write((char *)&pilefrom, sizeof pilefrom);
    of.write((char *)&pileto, sizeof pileto);
    of.write((char *)&op->m_revlast, sizeof op->m_revlast);
    of.write((char *)&op->m_revOrder, sizeof op->m_revOrder);
//    SaveCardList(of, op->m_cards);

    int cardnum = op->m_cards.size();
    of.write((char *)&cardnum, sizeof cardnum);
    for(auto card : op->m_cards)
    {
        int id = card->id();
        of.write((char *)&id, sizeof id);
    }

    return true;
}

Op *GameStorage::LoadOp(ifstream &ifile)
{
    int pilefrom, pileto;
    BOOL revlast, revOrder;
    vector<Card *> cardlist;
    ifile.read((char *)&pilefrom, sizeof pilefrom);
    ifile.read((char *)&pileto, sizeof pileto);
    ifile.read((char *)&revlast, sizeof revlast);
    ifile.read((char *)&revOrder, sizeof revOrder);
 
 //   LoadCardList(ifile, cardlist);
    int cardnum =0;
    ifile.read((char *)&cardnum, sizeof cardnum);
    for(int i=0; i<cardnum; i++)
    {
        int id;
        ifile.read((char *)&id, sizeof id);
        Card *card = m_pGameWin->m_cards[id];
        cardlist.push_back(card);
    }

    Pile *pfrom = GetPile(pilefrom);
    Pile *pto = GetPile(pileto);

    if( pfrom==nullptr || pto==nullptr)
        return nullptr;

    Op *op = new Op(pfrom, pto, cardlist, revlast, revOrder);
    if(pilefrom == 0)
        op->setDeliver();

    return op;
}


bool GameStorage::SaveOpList(ofstream &of, vector<Op *> *opList)
{
    bool flag = true;

    int size = opList->size();
    of.write((char *)&size, sizeof size);
    for(auto op : *opList)
    {
       flag = (flag && SaveOp(of, op) );
    }

    return flag;
}

vector<Op *> *GameStorage::LoadOpList( ifstream &ifile )
{
    int size=0;
    ifile.read((char *)&size, sizeof size);
    if( size<=0) return nullptr;

    vector<Op *> *oplist = new vector<Op *>();
    for(int i=0; i<size; i++)
    {
        Op *op = LoadOp(ifile);
        oplist->push_back(op);
    }

    return oplist;
}

bool GameStorage::SaveUndoList(ofstream &of)
{
    bool flag = true;

    int size = m_pGameWin->m_undoList.size();
    of.write((char *)&size, sizeof size);

    for( auto opList : m_pGameWin->m_undoList)
    {
        flag = (flag && SaveOpList(of, opList) );
    }

    return flag;
}

bool GameStorage::LoadUndoList( ifstream &ifile)
{
    int size=0;
    ifile.read((char *)&size, sizeof size);
    if( size<=0 ) return false;

    for(int i=0; i<size; i++)
    {
        vector<Op *> *oplist = LoadOpList(ifile);
        if( oplist != nullptr){
            m_pGameWin->m_undoList.push_back(oplist);
        }
    }
    return true;
}