#pragma once
#include "cursor.h"

/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a matrix object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT).
 *
 */
class Matrix
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    // this gives the Name of .csv file that contains the data for the matrix
    string sourceFileName = "";

    /*ANCHOR - */
    int blockMatSize;
    // Matrix Name of this matrix
    string matrixName = "";
    // // Column Names of this matrix
    // vector<string> columns;
    // How many distinct values in a column
    vector<uint> distinctValuesPerColumnCount;
    
    // No. of columns
    uint columnCount = 0;
    // Stores total row count
    long long int rowCount = 0;
    // keeps the count of total blocks used to store the matrix
    uint blockCount = 0;

    uint maxRowsPerBlock = 0;

    vector<uint> rowsPerBlockCount;

    unordered_map<string, pair<int, int>> rangeOfPage;

    bool blockify();
    void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    Matrix(string matrixName, vector<string> columns);
    bool load();

    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    void transposeMatrix();
    bool checkSymmetry();
    void unload();
    void renameMatrix(string toMatrixName);
    Matrix* computeMatrix();

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
        logger.log("Matrix::printRow");
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
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};