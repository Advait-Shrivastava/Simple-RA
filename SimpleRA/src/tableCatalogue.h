#include "table.h"
#include "matrix.h"

/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
class TableCatalogue
{

    unordered_map<string, Table*> tables;
    unordered_map<string, Matrix*> matrices;  //This will store all the matrices loaded so far

public:
    TableCatalogue() {}
    void insertTable(Table* table);
    void deleteTable(string tableName);
    Table* getTable(string tableName);
    bool isTable(string tableName);
    bool isColumnFromTable(string columnName, string tableName);
    void print();
    ~TableCatalogue();

    void insertMatrix(Matrix* matrix);  // These are the new function we need to handle matrices
    void deleteMatrix(string matrix);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string tableName);
    void renameMatrix(string prevName,string updatedName);
};