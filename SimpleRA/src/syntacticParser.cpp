#include "global.h"
/*      
                                            Import Dependency
                                            -----------------
        A <--- B
        A includes B
        -----------------------------------------------------------------------------------------------
        |global.h <--- executo.h <--- semanticparser.h <--- syntacticparser.h(not syntacticparser.cpp)|
        ----------------------------------------------------------------------------------------------
*/
bool syntacticParse()
{
    logger.log("syntacticParse");
    /* 
        code logic here basically
        is defines a single string as possible query type
        will have do some changes for LOAD MATRIX
    */
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD")
        return syntacticParseLOAD();
    else if (possibleQueryType == "PRINT")
        return syntacticParsePRINT();
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if(possibleQueryType == "EXPORT")
        return syntacticParseEXPORT();
    else if(possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if(possibleQueryType == "CHECKSYMMETRY")
        return syntacticParseCHECKSYMMETRY();
    else if(possibleQueryType == "COMPUTE")
        return syntacticParseCOMPUTE();  
    else if(possibleQueryType == "TRANSPOSE")
        return syntacticParseTRANSPOSE();
    else if(possibleQueryType == "SORT")
        return syntacticParseSORT();                
    else
    {
        /* 
        General Table Operations
            cross
            join
            select
            etc
        */
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else if (possibleQueryType == "SORT")
            return syntacticParseSORT();
        else if (possibleQueryType == "ORDER")
            return syntacticParseORDER();
        else if (possibleQueryType == "GROUP")
            return syntacticParseGROUP();    

        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";
    this->renameToRelationName = "";
    this->checkSymmetryRelationName = "";
    this->computeRelationName = "";
    this->transposeRelationName = "";


    this->aggregate_Func = NO_AGGREGATE_CLAUSE;
    this->havingAggregate_Func = NO_AGGREGATE_CLAUSE;
    this->havingBinaryOperator = NO_BINOP_CLAUSE;
    this->groupByResultRelationName = "";
    this->groupByRelationName = "";
    this->groupByGroupingColName = "";
    this->groupByHavingColName = "";
    this->groupByAggregateColName = "";
    this->havingAttributeValue = 0;


    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortColumnName = "";
    this->sortRelationName = "";
    this->columnNamesToSort.clear();
    this->colsortingTechniques.clear();

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
