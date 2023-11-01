#pragma once
#include "tableCatalogue.h"

using namespace std;
/*NOTE - 
    Basically this file contains declarations of the function defined in the syntacticParser.cpp
    and class ParsedQuery::definition
*/
enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED,
    CHECKSYMMETRY,   //This Query is added by us to CHECK the symmetry of matrix
    COMPUTE,         //This Query is added by us to COMPUTE the expression 
    TRANSPOSE,       //This Query is added by us to TRANPOSE the matrix
    ORDER,           //This Query is added by us to ORDER the table
    GROUP            //This Query is added by us to Group the table
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum Aggregate_Func
{
    MIN,
    MAX,
    SUM,
    COUNT,
    AVG,
    NO_AGGREGATE_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";
    string renameToRelationName = "";       // This variable is declared by us to do matrix rename
    string checkSymmetryRelationName = "";       // This variable is declared by us to do matrix rename
    string computeRelationName = "";       // This variable is declared by us to do matrix computation
    string transposeRelationName = "";       // This variable is declared by us to do matrix transpose



    Aggregate_Func aggregate_Func = NO_AGGREGATE_CLAUSE;
    Aggregate_Func havingAggregate_Func = NO_AGGREGATE_CLAUSE;
    BinaryOperator havingBinaryOperator = NO_BINOP_CLAUSE;
    string groupByResultRelationName = "";
    string groupByRelationName = "";
    string groupByGroupingColName = "";
    string groupByHavingColName = "";
    string groupByAggregateColName = "";
    double havingAttributeValue = 0;
    // string transposeRelationName = "";


    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";
    vector<string>columnNamesToSort;
    vector<SortingStrategy>colsortingTechniques;

    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();
bool syntacticParseTRANSPOSE();
bool syntacticParseORDER();
bool syntacticParseGROUP();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
