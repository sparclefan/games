#pragma execution_character_set("utf-8")

/**
 * @brief convert sudoku to exact cover problem  ( 324 column )
 *  column  0 -- (0, 0) has number
 *          1 -- (0, 1) has number
 *          ......
 *          9 -- (1, 0) has number
 *          .....
 *          80 -- (8, 8) has number
 * --------------------------------------
 *          81 --  row 0 has number 1
 *          82 --  row 0 has number 2
 *          ....
 *          89 --  row 0 has number 9
 *          90 --  row 1 has number 1
 *          .....
 *          161 --  row 8 has number 9
 * --------------------------------------
 *          162 --  column 0 has number 1
 *          .....
 *          242 --  column 8 has number 9
 * -----------------------------------------
 *          243 --  block 0 has number 1
 *          .....
 *          323 --  block 8 has number 9
 * 
 *   If grid cell (i, j) set number k, it got a row { a, b, c, d}, where:
 *          a = i*9 + j
 *          b = i*9 + k + 80
 *          c = j*9 + k + 161
 *          d = (i//3*3 + j//3) * 9 + k + 242
 * 
 *   refrence:
 *          https://zhuanlan.zhihu.com/p/67447747
 *          https://www.iteye.com/blog/philoscience-1539970
 * 
 *  @author Sparcle F. 2021-11-26
 */

#ifndef __SUDOKU_H_Sparcle_2021_11_26
#define __SUDOKU_H_Sparcle_2021_11_26

#include <vector>
#include <map>
using namespace std;

struct SudokuUnit {
    int col;
    int row;
    int num;
};

class Sudoku
{

public:
    Sudoku();
    ~Sudoku();

    vector<SudokuUnit> generatorPuzzle(int maxEmpty);
    vector<SudokuUnit> randomSolution();
    bool solv(vector<SudokuUnit> puzzle, vector<SudokuUnit> &solution);

    static inline int blockIndex(int row, int col){ return (row/3)*3+(col/3); };

private:

    bool testSolv(map<int, vector<int>> puzzleRows, vector<SudokuUnit> &solution, int exceptLoc=-1);

    vector<int> sudukuToRowData(int col, int row, int num);
    SudokuUnit rowDataToSudukuUnit(vector<int> rowData);

    vector<SudokuUnit> createRandomUnits(int nCount);
    void creatDlxRows(map<int, vector<int>> initRows, int exceptRow);

    static vector<int> shuffleIndex(int n);

    vector<vector<int>> m_rowDatas;
};

bool unit_test_Sudoku();

#endif //__SUDOKU_H_Sparcle_2021_11_26