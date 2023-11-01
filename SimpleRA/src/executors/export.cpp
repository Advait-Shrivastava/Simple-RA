#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool isExportMatrixPresent = false;

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportRelationName = tokenizedQuery[1];
        return true;
        
    }
    else if(tokenizedQuery.size() == 3 && tokenizedQuery[1]=="MATRIX"){
        isExportMatrixPresent = true;
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportRelationName = tokenizedQuery[2];
        return true;
    }

    isExportMatrixPresent = false;
    cout << "SYNTAX ERROR" << endl;
    return false;
    
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT"); 

    if(isExportMatrixPresent)
    {   
        if (tableCatalogue.isMatrix(parsedQuery.exportRelationName))
        return true;
    }

    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{

    logger.log("executeEXPORT");

    if(isExportMatrixPresent)
    {
        Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.exportRelationName);
        matrix->makePermanent();
        cout<<""<<endl;
        int blockRead= matrix->blockCount;
        int blockWritten = 0;
        cout<<"Number of blocks read: "<<blockRead<<endl;
        cout<<"Number of blocks written: "<<blockWritten<<endl;
        cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;
        return;
    }

    
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}