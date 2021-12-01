#include "sudoku.h"
#include "dlx.h"
#include <assert.h>
#include <time.h>
#include <set>
#include <iostream>

Sudoku::Sudoku()
: m_rowDatas()
{

}

Sudoku::~Sudoku()
{

}

vector<int> Sudoku::sudukuToRowData(int col, int row, int num)
{
    vector<int> result;
    int a = row * 9 + col;
    int b = row * 9 + num + 80;
    int c = col * 9 + num + 161;
    int d = blockIndex(row, col) * 9 + num + 242;

    result.push_back(a);
    result.push_back(b);
    result.push_back(c);
    result.push_back(d);

    return result;
}

SudokuUnit Sudoku::rowDataToSudukuUnit(vector<int> rowData)
{
    assert(rowData.size()==4);
    
    int a = rowData[0];
    int b = rowData[1];
    int c = rowData[2];
    int d = rowData[3];

    int col = (a - b + c - 81)/10;
    int row = (a - col)/9;
    int num = b - 80 - row*9;

    SudokuUnit result = {col, row, num};

    return result;
}

vector<SudokuUnit> Sudoku::createRandomUnits(int nCount)
{
    srand((unsigned)time(NULL));

    vector<SudokuUnit> result;
    set<int> cols;
    while( result.size() < nCount )
    {
        int col = rand()%8;
        int row = rand()%8;
        int num = rand()%8 + 1;

        vector<int> rowdata = sudukuToRowData(col, row, num);
        if( cols.find(rowdata[0]) != cols.end() ) continue;
        if( cols.find(rowdata[1]) != cols.end() ) continue;
        if( cols.find(rowdata[2]) != cols.end() ) continue;
        if( cols.find(rowdata[3]) != cols.end() ) continue;

        for( auto col : rowdata){
            cols.insert(col);
        }

        SudokuUnit unit = {col, row, num};
        result.push_back(unit);
    }

    return result;
}

void Sudoku::creatDlxRows(map<int, vector<int>> initRows, int exceptRow)
{
    m_rowDatas.clear();

    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
    {
        int idx = row*9+col;
        if( initRows.find(idx) != initRows.end()){
            vector<int> rowdata = initRows[idx];

            if( idx == exceptRow )
            {
                SudokuUnit unit = rowDataToSudukuUnit(rowdata);
                for(int num=1; num<10; num++){
                    if( num != unit.num ){
                        vector<int> rd = sudukuToRowData(unit.col, unit.row, num);
                        m_rowDatas.push_back(rd);
                    }
                }

            }else{
                m_rowDatas.push_back(rowdata);
            }

        }else{
            for(int num=1; num<10; num++){
                vector<int> rowdata = sudukuToRowData(col, row, num);
                m_rowDatas.push_back(rowdata);
            }
        }

    }
}

bool Sudoku::solv(vector<SudokuUnit> puzzle, vector<SudokuUnit> &solution)
{
    map<int, vector<int>> puzzleRows;
    for(auto unit : puzzle){
        vector<int> rowData = sudukuToRowData(unit.col, unit.row, unit.num);
        int idx = unit.row * 9 + unit.col;
        puzzleRows[idx] = rowData;
    }
    return testSolv(puzzleRows, solution);
}

bool Sudoku::testSolv(map<int, vector<int>> puzzleRows, vector<SudokuUnit> &solution, int exceptLoc)
{
    creatDlxRows(puzzleRows, exceptLoc);

    DancingLinksX dlx(324);
    for(auto rowdata : m_rowDatas){
        dlx.addRow(rowdata);
    }
    if(dlx.solve())
    {
        vector<int> solutions = dlx.getSolution();
        for(auto rowid : solutions)
        {
            SudokuUnit unit = rowDataToSudukuUnit(m_rowDatas[rowid]);
            solution.push_back(unit);
        }
        return true;
    }

    return false;    
}

vector<SudokuUnit> Sudoku::randomSolution()
{
    vector<SudokuUnit> solution;
    vector<SudokuUnit> puzzle;

    do
    {
        puzzle = createRandomUnits(11);
    }while(!solv(puzzle, solution) );

    return solution;
}

vector<int> Sudoku::shuffleIndex(int n)
{
    vector<int> tmplist;
    vector<int> result;
    for(int i=0; i<n; i++)
        tmplist.push_back(i);

    while(tmplist.size()){
        int id = rand()%tmplist.size();
        result.push_back(tmplist[id]);
        tmplist.erase(tmplist.begin()+id);
    }
    return result;
}

vector<SudokuUnit> Sudoku::generatorPuzzle(int maxEmpty)
{
    vector<SudokuUnit> sol = randomSolution();

    vector<int> seq = shuffleIndex(81);
    int done = 0;
    int tries = 0;

    map<int, vector<int>> puzzleRows;
    for(auto unit : sol){
        vector<int> rowData = sudukuToRowData(unit.col, unit.row, unit.num);
        int idx = unit.row * 9 + unit.col;
        puzzleRows[idx] = rowData;
    }

    vector<SudokuUnit> tmpSol;
    while( done<maxEmpty && tries<81 )
    {
        int idx = seq[tries++];
        if( !testSolv(puzzleRows, tmpSol, idx) )
        {
            puzzleRows.erase(idx);
            done ++;
        }
    }

    vector<SudokuUnit> puzzle;
    for(auto it : puzzleRows)
    {
        SudokuUnit unit = rowDataToSudukuUnit(it.second);
        puzzle.push_back(unit);
    }

    return puzzle;

}

bool unit_test_Sudoku()
{
    Sudoku sudoku;

    // vector<SudokuUnit> sol = sudoku.randomSolution();
    vector<SudokuUnit> sol = sudoku.generatorPuzzle(20);

    int sd[9][9] = {{0}};
    for(auto unit : sol)
    {
        sd[unit.col][unit.row] = unit.num;
    }

    for(int row=0; row<9; row++)
    {
        for(int col=0; col<9; col ++)
        {
            cout << sd[col][row] << "  ";
        }
        cout << endl;
    }
    return true;
}