#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    /*NOTE -
    get tables:- 
    ------------

        loads the table with the given table name from
        table catalogue. to table variable
    */
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);
    /*!SECTION:- Loading datapage or file with the page name we
        generated using pageIndex and tablename
    */
    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}
vector<vector<int>> Page::getRows()
{
    logger.log("Page::getRows");
    return this->rows;
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}
//-----------------------------------Our Code----------------------------------------
Page::Page(string matrixName, pair<int,int> pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = matrixName;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = this->rowCount;
    this->pageName = "../data/temp/"+this->tableName + "_matPage" + "_" + to_string(pageIndex.first) + "_" + to_string(pageIndex.second);
}


Page::Page(string matrixName, pair<int, int> pageIndex)
{
    logger.log("Page::Page");
    this->tableName = matrixName;
    this->pageIndex = pageIndex.first + "," + pageIndex.second;
    this->pageName = "../data/temp/" + this->tableName + "_matPage" + "_" + to_string(pageIndex.first) + "_" + to_string(pageIndex.second);
    // Matrix matrix = *tableCatalogue.getMatrix(matrixName);
    /*NOTE -
    get tables:- 
    ------------

        loads the table with the given table name from
        table catalogue. to table variable
    */
    this->columnCount = 15;
    this->rowCount = 15;
    // uint maxRowCount = matrix.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(15, row);

    /*!SECTION:- Loading datapage or file with the page name we
        generated using pageIndex and tablename
    */

    ifstream fin(pageName, ios::in);
    // this->rowCount = matrix.maxRowsPerBlock[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}
vector<vector<int>> Page::getMatrixRows(){
    return this->rows;
}
int Page::getRowCount(){
    return this->rowCount;
}