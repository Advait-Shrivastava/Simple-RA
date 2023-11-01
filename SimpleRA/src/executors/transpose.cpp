#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY matrixName
 */

bool isTransposeMatrixPresent = false;  //This varible is added as a flag to show whether the input is Table or matrix

bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");


    if (tokenizedQuery.size() == 3 && tokenizedQuery[1]=="MATRIX")
    {
        isTransposeMatrixPresent = true;
        parsedQuery.queryType = TRANSPOSE;
        parsedQuery.transposeRelationName = tokenizedQuery[2];
        return true;
        
    }

    isTransposeMatrixPresent = false;
    cout << "SYNTAX ERROR" << endl;
    return false;
}

bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");

    if(isTransposeMatrixPresent)
    {
        if (!tableCatalogue.isMatrix(parsedQuery.transposeRelationName))
        {
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
            return false;
        }
        return true;

    } 
    return false;    
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    
    Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.transposeRelationName);
    
    // This function to be implemented on matrix.cpp file
    matrix->transposeMatrix();
    int blockRead= matrix->blockCount;
    int blockWritten = matrix->blockCount;
    cout<<"Number of blocks read: "<<blockRead<<endl;
    cout<<"Number of blocks written: "<<blockWritten<<endl;
    cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;
    return;
}