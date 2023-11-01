#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */

bool isLoadMatrixPresent = false;

bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if(tokenizedQuery.size() == 2)
    {
        isLoadMatrixPresent = false;
        parsedQuery.queryType = LOAD;
        parsedQuery.loadRelationName = tokenizedQuery[1];
        return true;
    }

    else if (tokenizedQuery.size() == 3 && tokenizedQuery[1]=="MATRIX")
    {
        isLoadMatrixPresent = true;
        parsedQuery.queryType = LOAD;
        parsedQuery.loadRelationName = tokenizedQuery[2];
        return true;
    }

    isLoadMatrixPresent = false;
    cout << "SYNTAX ERROR" << endl;
    return false;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");

    if(isLoadMatrixPresent)
    {
        //Write the catalogue for matrices

        if (tableCatalogue.isMatrix(parsedQuery.loadRelationName))
        {
            cout << "SEMANTIC ERROR: Matrix already exists" << endl;
            return false;
        }

        

        if (!isFileExists(parsedQuery.loadRelationName))
        {
            cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
            return false;
        }
        return true;
    }

    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    if(isLoadMatrixPresent)
    {
        //Create the object of matrix class and call the load function to load csv file.
        Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
        if (matrix->load())
        {
            tableCatalogue.insertMatrix(matrix);
            cout << "Loaded Matrix. Column Count: " << matrix->columnCount << " Row Count: " << matrix->rowCount << endl;
            int blockRead= matrix->blockCount;
            int blockWritten = matrix->blockCount;
            cout<<"Number of blocks read: "<<blockRead<<endl;
            cout<<"Number of blocks written: "<<blockWritten<<endl;
            cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;
        }
        return;   
    }

    
    Table *table = new Table(parsedQuery.loadRelationName);
    // cout<<"The value of matrix is : "<<parsedQuery.loadRelationName<<endl;
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;

    }
    return;
    
}