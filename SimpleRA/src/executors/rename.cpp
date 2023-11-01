#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */

bool isRenameMatrixPresent = false;  //This varible is added as a flag to show whether the input is Table or matrix

bool syntacticParseRENAME()
{
    logger.log("syntacticParseRENAME");
    
    if(tokenizedQuery.size()==4 && tokenizedQuery[1]=="MATRIX")
    {
        isRenameMatrixPresent = true;
        parsedQuery.queryType = RENAME;
        parsedQuery.renameRelationName = tokenizedQuery[2];
        parsedQuery.renameToRelationName = tokenizedQuery[3];
        return true;
    }


    if (tokenizedQuery.size() != 6 || tokenizedQuery[2] != "TO" || tokenizedQuery[4] != "FROM")
    {
        isRenameMatrixPresent = false;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    isRenameMatrixPresent = false;
    parsedQuery.queryType = RENAME;
    parsedQuery.renameFromColumnName = tokenizedQuery[1];
    parsedQuery.renameToColumnName = tokenizedQuery[3];
    parsedQuery.renameRelationName = tokenizedQuery[5];
    return true;
}

bool semanticParseRENAME()
{
    logger.log("semanticParseRENAME");
    
    if(isRenameMatrixPresent)
    {
        if (!tableCatalogue.isMatrix(parsedQuery.renameRelationName))
        {
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
            return false;
        }

        if (tableCatalogue.isMatrix(parsedQuery.renameToRelationName))
        {
            cout << "SEMANTIC ERROR: New matrix name already exists" << endl;
            return false;
        }

        return true;

    }
    

    if (!tableCatalogue.isTable(parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.renameFromColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (tableCatalogue.isColumnFromTable(parsedQuery.renameToColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column with name already exists" << endl;
        return false;
    }
    return true;
}

void executeRENAME()
{
    logger.log("executeRENAME");
    
    if(isRenameMatrixPresent)
    {
        Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.renameRelationName);

        // This function to be implemented on matrix.cpp file
        matrix->renameMatrix(parsedQuery.renameToRelationName);
        //Updating the Matrix catalogue
        tableCatalogue.renameMatrix(parsedQuery.renameRelationName,parsedQuery.renameToRelationName);
        cout << "Matrix Renamed : From "<<parsedQuery.renameRelationName<<" TO "<<parsedQuery.renameToRelationName<<endl;
        return;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.renameRelationName);
    table->renameColumn(parsedQuery.renameFromColumnName, parsedQuery.renameToColumnName);
    return;
}