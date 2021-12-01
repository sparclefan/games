#pragma execution_character_set("utf-8")

/**
 * @brief Dancing links X algorithm
 * 
 *   reference:  
 *          [Dancing Links] by Donald E. Knuth, Stanford University
 *          http://t.zoukankan.com/bifanwen-p-12666423.html
 *          https://www.cnblogs.com/SGCollin/p/9569807.html
 *          https://www.cnblogs.com/grenet/p/3145800.html
 *
 *      and here's a more concise solution:
 *          https://www.cs.mcgill.ca/~aassaf9/python/algorithm_x.html
 * 
 * @author Sparcle F. 2021-11-24
 */

#ifndef __DANCELINKX_H_Sparcle_2021_11_24
#define __DANCELINKX_H_Sparcle_2021_11_24

#include <vector>
#include <stack>

using namespace std;

class DancingLinksX
{
    struct CrossChainNode {
        CrossChainNode *left, *right, *up, *down;
        int row;
        int col;
        CrossChainNode() : left(this), right(this), up(this), down(this), row(-1), col(-1) {};
    };

public:
    DancingLinksX(int nCol);
    ~DancingLinksX();

    void addRow(const vector<int>& colIds );
    // void build(vector<vector<int>> data, int nCol);
    bool solve();
    vector<int> getSolution();
    void print();

private:    
    DancingLinksX();
    void clear();
    void addColumn();
    void addColumns(int nCol);
    void removeRow(CrossChainNode *pRow, vector<CrossChainNode *> &cache);
    void removeColumn(CrossChainNode *pCol, vector<CrossChainNode *> &cache);

    CrossChainNode *getColumn(int colId);
    int getColumnNodeCount(CrossChainNode *pCol);
    bool takenRow(CrossChainNode *node);
    void backTracking();

private:
    CrossChainNode *m_head;
    int m_rowCount;
    int m_colCount;

    vector<CrossChainNode *> m_takenRow;
    stack<vector<CrossChainNode *>> m_removeCache;

};

bool unit_test_DancingLinksX();

#endif //__DANCELINKX_H_Sparcle_2021_11_24