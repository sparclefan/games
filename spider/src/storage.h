#ifndef __storage_h_Sparcle_2021_05_21
#define __storage_h_Sparcle_2021_05_21

#include <vector>
#include <fstream>
#include "gamewin.h"

using namespace std;

typedef struct {
    char verinfo[32];
    int suitnum;
    int score;
} stGameInfo;


class GameStorage {

public:
    GameStorage(GameWin *pGameWin);
    ~GameStorage();

    bool Save(string filepath);
    bool Load(string filepath);

private:

    int GetPileIndex(Pile *pile);
    Pile *GetPile(int index);

    void SaveGameInfo(ofstream &of);
    bool SaveOp(ofstream &of, Op *op);
    bool SaveOpList(ofstream &of,  vector<Op *> *opList);
    void SaveCardList( ofstream &of,  const vector<Card *> &cardlist );
    bool SaveUndoList(ofstream &of);

    bool LoadGameInfo(ifstream &ifile, stGameInfo &ginfo );
    bool LoadCardList( ifstream &ifile, vector<Card *> &cardlist);
    Op *LoadOp(ifstream &ifile);
    vector<Op *> *LoadOpList( ifstream &ifile );
    bool LoadUndoList( ifstream &ifile);

    GameWin *m_pGameWin;
};

#endif //__storage_h_Sparcle_2021_05_21