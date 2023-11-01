#include "global.h"

void TableCatalogue::insertTable(Table *table)
{
    logger.log("TableCatalogue::~insertTable");
    this->tables[table->tableName] = table;
}
void TableCatalogue::deleteTable(string tableName)
{
    logger.log("TableCatalogue::deleteTable");
    this->tables[tableName]->unload();
    delete this->tables[tableName];
    this->tables.erase(tableName);
}
Table *TableCatalogue::getTable(string tableName)
{
    logger.log("TableCatalogue::getTable");
    Table *table = this->tables[tableName];
    return table;
}
bool TableCatalogue::isTable(string tableName)
{
    logger.log("TableCatalogue::isTable");
    if (this->tables.count(tableName))
        return true;
    return false;
}

bool TableCatalogue::isColumnFromTable(string columnName, string tableName)
{
    logger.log("TableCatalogue::isColumnFromTable");
    if (this->isTable(tableName))
    {
        Table *table = this->getTable(tableName);
        if (table->isColumn(columnName))
            return true;
    }
    return false;
}

void TableCatalogue::print()
{
    logger.log("TableCatalogue::print");
    cout << "\nRELATIONS" << endl;

    int rowCount = 0;
    for (auto rel : this->tables)
    {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

TableCatalogue::~TableCatalogue()
{
    logger.log("TableCatalogue::~TableCatalogue");
    for (auto table : this->tables)
    {
        table.second->unload();
        delete table.second;
    }
}

// Below functions are defined by us to handle the matrices

void TableCatalogue::insertMatrix(Matrix *matrix)
{
    logger.log("TableCatalogue::~insertMatrix");
    this->matrices[matrix->matrixName] = matrix;
}

void TableCatalogue::deleteMatrix(string matrix)
{
    logger.log("TableCatalogue::deleteMatrix");
    this->matrices[matrix]->unload();
    delete this->matrices[matrix];
    this->matrices.erase(matrix);
}

Matrix *TableCatalogue::getMatrix(string matrixName)
{
    logger.log("TableCatalogue::getMatrix");
    Matrix *matrix = this->matrices[matrixName];
    return matrix;
}

bool TableCatalogue::isMatrix(string matrixName)
{
    logger.log("TableCatalogue::isMatrix");
    if (this->matrices.count(matrixName))
        return true;
    return false;
}

void TableCatalogue::renameMatrix(string prevName, string updatedName)
{
    logger.log("TableCatalogue::renameMatrix");
    // this->matrices[matrix]->unload();
    // delete this->matrices[matrix];
    Matrix *matrix = this->matrices[prevName];
    this->matrices.erase(prevName);
    this->matrices[updatedName] = matrix;
    return;
}
