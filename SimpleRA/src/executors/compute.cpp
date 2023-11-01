#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY matrixName
 */

// bool isRenameMatrixPresent = false;  //This varible is added as a flag to show whether the input is Table or matrix

bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");


    if (tokenizedQuery.size() != 2)
    {
        // isRenameMatrixPresent = false;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    // isRenameMatrixPresent = false;
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");
    
    if (!tableCatalogue.isMatrix(parsedQuery.computeRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    return true;    
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    
    Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.computeRelationName);

    // This function to be implemented on matrix.cpp file
    // Here we have to make another copy of out matrix and do the transpose of it, after that subtract these two matrices.
    
    Matrix* resultMatrix = matrix->computeMatrix();
    tableCatalogue.insertMatrix(resultMatrix);

    int blockRead= matrix->blockCount;
    int blockWritten = matrix->blockCount;
    cout<<"Number of blocks read: "<<blockRead<<endl;
    cout<<"Number of blocks written: "<<blockWritten<<endl;
    cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;

    return;
}