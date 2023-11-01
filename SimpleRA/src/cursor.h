#pragma once
#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
// enum IndexingStrategy
// {
//     BTREE,
//     HASH,
//     NOTHING
// };
class Cursor{
    public:
    Page page;
    int pageIndex;
    string tableName;
    int pagePointer;

    public:
    Cursor(string tableName, int pageIndex);
    vector<int> getNext();
    vector<int> getNextRow();
    vector<vector<int>> getAllRows();
    void nextPage(int pageIndex);

    //--------------------------Our Code----------------------------------------
    //-------Matrix
    Cursor(string MatrixName, pair<int,int> pageIndex);
    Cursor();
};