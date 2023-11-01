#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY matrixName
 */

// bool isRenameMatrixPresent = false;  //This varible is added as a flag to show whether the input is Table or matrix

bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");


    if (tokenizedQuery.size() != 2)
    {
        // isRenameMatrixPresent = false;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    // isRenameMatrixPresent = false;
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checkSymmetryRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    
    if (!tableCatalogue.isMatrix(parsedQuery.checkSymmetryRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    return true;    
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    
    Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.checkSymmetryRelationName);

    // This function to be implemented on matrix.cpp file
    bool result = matrix->checkSymmetry();
    
    if(result)
    {
        cout << "Matrix "<<parsedQuery.checkSymmetryRelationName<<" is Symmetric"<<endl;    
    }
    else
    {
        cout << "Matrix "<<parsedQuery.checkSymmetryRelationName<<" is not Symmetric"<<endl;
    }

    int blockRead= matrix->blockCount;
    int blockWritten = 0;
    cout<<"Number of blocks read: "<<blockRead<<endl;
    cout<<"Number of blocks written: "<<blockWritten<<endl;
    cout<<"Number of blocks accessed: "<<blockRead+blockWritten<<endl;

    return;
}