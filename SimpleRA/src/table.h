#pragma once
#include "cursor.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Table
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    // this gives the Name of .csv file that contains the data for the table
    string sourceFileName = "";
    // Table Name of this table
    string tableName = "";
    // Column Names of this table
    vector<string> columns;
    // How many distinct values in a column
    vector<uint> distinctValuesPerColumnCount;
    // No. of columns
    uint columnCount = 0;
    // Stores total row count
    long long int rowCount = 0;
    // keeps the count of total blocks used to store the table
    uint blockCount = 0;
    // Max No. of rows per block
    uint maxRowsPerBlock = 0;
    // Count of rows for every block
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;
    
    bool extractColumnNames(string firstLine);
    bool blockify();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(string tableName, vector<string> columns);
    Table(string tableName,Table* table);
    bool load();
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();

    // void sortTheTable();
    bool sortDeleteFile(const string);
    bool sortRenameFile(string &oldFilePath, const string &newFilePath);
    vector<vector<int>> sortReadCSV(string filename);
    void sortWriteCSV(const string &filename, const vector<vector<int>> &data);

    void initialSort(string tname,vector<vector<int>> order,BufferManager &bf);
    void merge(vector<vector<int>> &rangeOfMerge, vector<vector<int>> &order,string tablename,string temptablename);
    void mergesort(int l, int r,string tablename,string temptablename,vector<vector<int>> &order);
    void sorting(string newTableName, vector<vector<int>> sortOrder,BufferManager &bf);

    bool deleteTable();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Table::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Table::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}

};