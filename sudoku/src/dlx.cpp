#include "dlx.h"
#include <assert.h>
#include <iostream>

DancingLinksX::DancingLinksX()
: m_colCount(0), m_rowCount(0), m_takenRow(), m_removeCache()
{
    m_head = new CrossChainNode();
}

DancingLinksX::DancingLinksX(int nCol)
: DancingLinksX()
{
    addColumns(nCol);
}


DancingLinksX::~DancingLinksX()
{
    clear();
}

void DancingLinksX::clear()
{
    if( !m_head ) return;

    CrossChainNode *pCol = m_head;
    while( pCol != m_head)
    {
        CrossChainNode *node = pCol;
        while( node != pCol )
        {
            node = node->down;
            delete node->up;
        }
        pCol = pCol->right;
        delete pCol->left;
    }
    delete m_head;
    m_head = nullptr;
}

// void DancingLinksX::build(vector<vector<int>> data, int nCol)
// {
//     clear();
//     m_head = new CrossChainNode();

//     addColumns(nCol);
//     for(auto row : data){
//         addRow(row);
//     }

//     print();
// }


/**
 *   (newNode)<-head <--> node <--> ... <--> newNode->(head)
 * 
 */
void DancingLinksX::addColumn()
{
    CrossChainNode *newNode = new CrossChainNode();
    newNode->col = m_colCount++;

    CrossChainNode *right = m_head->left;
    m_head->left = newNode;
    newNode->right = m_head;
    right->right = newNode;
    newNode->left = right;
}

void DancingLinksX::addColumns(int nCol)
{
    for(int i=0; i<nCol; i++)
        addColumn();
}

DancingLinksX::CrossChainNode *DancingLinksX::getColumn(int colId)
{
    if( colId >= m_colCount )
        return nullptr;

    CrossChainNode *node = m_head;
    if( colId >= m_colCount/2){
        int col = m_colCount;
        while( (node = node->left) != m_head){
            if ( --col == colId) return node;
        }
    }else{
        int col = 0;
        while( (node = node->right) != m_head){
            if( col++ == colId) return node;
        }
    }

    return nullptr;
}

void DancingLinksX::addRow(const vector<int>& colIds )
{
    if( colIds.size() == 0)
        return;

    CrossChainNode *row = nullptr;
    for( auto col : colIds )
    {
        CrossChainNode *pCol = getColumn(col);
        CrossChainNode *newNode = new CrossChainNode();

        CrossChainNode *down = pCol->up;
        pCol->up = newNode;
        newNode->down = pCol;
        down->down = newNode;
        newNode->up = down;

        if(row){
            CrossChainNode *right = row->left;
            row->left = newNode;
            newNode->right = row;
            right->right = newNode;
            newNode->left = right;
        }else{
            row = newNode;
        }
        newNode->col = pCol->col;
        newNode->row = m_rowCount;
    }
    m_rowCount++;
}

void DancingLinksX::removeRow(CrossChainNode *pRow, vector<CrossChainNode *> &cache)
{
    assert(pRow->row >=0);

    CrossChainNode *node = pRow;
    while ( (node = node->right) != pRow)
    {
        node->up->down = node->down;
        node->down->up = node->up;
        cache.push_back(node);
    }
}

void DancingLinksX::removeColumn(CrossChainNode *pCol, vector<CrossChainNode *> &cache)
{
    assert(pCol->col >=0);

    CrossChainNode *node = pCol;
    while( (node = node->down) != pCol)
    {
        if( node->row >= 0)
            removeRow(node, cache);
        else{
            node->left->right = node->right;
            node->right->left = node->left;
            cache.push_back(node);
        }
    }
}

int DancingLinksX::getColumnNodeCount(CrossChainNode *pCol)
{
    int count=0;
    CrossChainNode *node = pCol;
    while( (node = node->down) != pCol) count++;
    return count;
}

bool DancingLinksX::takenRow(CrossChainNode *node)
{
    vector<CrossChainNode *> cache;

    m_takenRow.push_back(node);

    CrossChainNode *pCol = node;
    do {
        removeColumn(pCol, cache);
    }while( (pCol = pCol->right) != node);

    m_removeCache.push(cache);
    return true;
}

void DancingLinksX::backTracking()
{
    m_takenRow.erase(m_takenRow.end()-1);

    vector<CrossChainNode *> cache = m_removeCache.top();
    for(auto node : cache)
    {
        if( node->up->down != node)
            node->up->down = node;
        if( node->down->up != node)
            node->down->up = node;
        if( node->left->right != node)
            node->left->right = node;
        if( node->right->left != node)
            node->right->left = node;
    }
    cache.clear();
    m_removeCache.pop();
}

bool DancingLinksX::solve()
{
    if(!m_head) return false;

    if( m_head->right == m_head)
        return true;

    CrossChainNode *pCol = m_head;
    int nodeCount = -1;

    CrossChainNode *node = pCol->right;
    do{
        int count = getColumnNodeCount(node);
        if( nodeCount<0 || count< nodeCount){
            pCol = node;
            nodeCount = count;
            if( nodeCount == 1)
                break;
            else if( nodeCount == 0)
                return false;
        }
    } while ( (node=node->right) != m_head);

    node = pCol;
    while( (node = node->down) != pCol)
    {
        takenRow(node);
        // print();
        if( solve())
            return true;
        else
            backTracking();
        // print();
    }

    return m_head->left == m_head;

}

vector<int> DancingLinksX::getSolution()
{
    vector<int> result;
    for( auto node : m_takenRow )
    {
        result.push_back(node->row);
    }
    return result;
}

void DancingLinksX::print()
{
    bool **exactcover = new bool *[m_rowCount];
    for(int i=0; i<m_rowCount; i++)
    {
        exactcover[i] = new bool[m_colCount];
        for(int j=0; j<m_colCount; j++)
            exactcover[i][j] = false;
    }

    CrossChainNode *pCol = m_head;
    while( (pCol = pCol->right) != m_head)
    {
        CrossChainNode *node = pCol;
        while( (node=node->down) != pCol)
            exactcover[node->row][node->col] = true;
    }

    cout << "-------------------------------------" << endl;
    for(int i=0; i<m_rowCount; i++)
    {
        for(int j=0; j<m_colCount; j++)
        {
            cout << (exactcover[i][j] ? 1:0) <<"  ";
        }
        cout << endl;
    }
    cout << "-------------------------------------" << endl;
}


bool unit_test_DancingLinksX()
{
	DancingLinksX dlx(8);
	vector<vector<int>> data = {
		{ 0, 3, 6 },
		{ 0, 3, 7 },
		{ 3, 4, 6, 7 },
		{ 2, 4, 5 },
		{ 1, 2, 5, 6, 7 },
		{ 1, 6 }
	};

	for(auto row : data){
		dlx.addRow(row);
	}

    dlx.print();

	if( dlx.solve() )
	{
		vector<int> solution = dlx.getSolution();
		cout << "exact cover solution: " << endl;
		for( auto rowid : solution)
		{
            cout << rowid << ", ";
		}
        cout << endl;
	}else{
		cout << "no solution!" << endl;
	}

	return true;

}