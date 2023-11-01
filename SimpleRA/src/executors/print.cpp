#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

bool isPrintMatrixPresent = false;

bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() == 2)
    {
        
        parsedQuery.queryType = PRINT;
        parsedQuery.printRelationName = tokenizedQuery[1];
        return true;
    }

    else if (tokenizedQuery.size() == 3 && tokenizedQuery[1]=="MATRIX")
    {
        isPrintMatrixPresent = true;
        parsedQuery.queryType = PRINT;
        parsedQuery.printRelationName = tokenizedQuery[2];
        return true;
    }

    isPrintMatrixPresent = false;
    cout << "SYNTAX ERROR" << endl;
    return false;
    
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");

    if(isPrintMatrixPresent){
        if (!tableCatalogue.isMatrix(parsedQuery.printRelationName))
        {
            cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
            return false;
        }
        return true; 
    }
    else{
        if (!tableCatalogue.isTable(parsedQuery.printRelationName))
        {
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
            return false;
        }
        return true;
    }    
}

void executePRINT()
{

    logger.log("executePRINT");

    if(isPrintMatrixPresent)
    {
        Matrix *matrix = tableCatalogue.getMatrix(parsedQuery.printRelationName);
        matrix->print();
        cout<<""<<endl;
        int blockRead= matrix->blockCount;
        int blockWritten = 0;
        cout<<"Number of blocks read: "<<blockRead<<endl;
        cout<<"Number of blocks written: "<<blockWritten<<endl;
        cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;
        return; 
    }    

    
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
    return;
}
