#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN 
 * <aggregate_func(attribute)>
 * 
 * sorting_order = ASC | DESC 
 */

bool isLastPartCorrect(string lastQuery)
{
    int functionLength;
    switch (lastQuery[1])
    {
    case 'I' :
        parsedQuery.aggregate_Func = MIN;
        functionLength = 3;
        break;

    case 'A' :
        parsedQuery.aggregate_Func = MAX;
        functionLength = 3;
        break;

    case 'U' :
        parsedQuery.aggregate_Func = SUM;
        functionLength = 3;
        break;

    case 'O' :
        parsedQuery.aggregate_Func = COUNT;
        functionLength = 5;
        break;            

    case 'V' :
        parsedQuery.aggregate_Func = AVG;
        functionLength = 3;
        break;
    
    default:
        return false;
    }

    if(lastQuery[functionLength]!='(' || lastQuery[lastQuery.size()-1]!=')')
    {
        cout<<lastQuery[functionLength]<<" "<<lastQuery[lastQuery.size()-1]<<endl;
        return false;
    }

    int colNameLength = lastQuery.size()-functionLength-2;
    parsedQuery.groupByAggregateColName = lastQuery.substr(functionLength+1,colNameLength);
    return true;

}

bool isHavingPartCorrect(string havingPart)
{
    int havingFunctionLength;

    switch (havingPart[1])
    {
    case 'I' :
        parsedQuery.havingAggregate_Func = MIN;
        havingFunctionLength = 3;
        break;

    case 'A' :
        parsedQuery.havingAggregate_Func = MAX;
        havingFunctionLength = 3;
        break;

    case 'U' :
        parsedQuery.havingAggregate_Func = SUM;
        havingFunctionLength = 3;
        break;

    case 'O' :
        parsedQuery.havingAggregate_Func = COUNT;
        havingFunctionLength = 5;
        break;            

    case 'V' :
        parsedQuery.havingAggregate_Func = AVG;
        havingFunctionLength = 3;
        break;
    
    default:
        return false;
    }

    if(havingPart[havingFunctionLength]!='(' || havingPart[havingPart.size()-1] !=')')
    {
        return false;
    }

    int havingcolNameLength = havingPart.size()-havingFunctionLength-2;
    parsedQuery.groupByHavingColName = havingPart.substr(havingFunctionLength+1,havingcolNameLength);
    return true;
}


bool isBinaryOperatorPartCorrect(string operatorType)
{
    if(operatorType == "<")
    {
        parsedQuery.havingBinaryOperator = LESS_THAN;
    }

    else if(operatorType == ">")
    {
        parsedQuery.havingBinaryOperator = GREATER_THAN;
    }

    else if(operatorType == "<=")
    {
        parsedQuery.havingBinaryOperator = LEQ;
    }

    else if(operatorType == ">=")
    {
        parsedQuery.havingBinaryOperator = GEQ;
    }

    else if(operatorType == "==")
    {
        parsedQuery.havingBinaryOperator = EQUAL;
    }

    else if(operatorType == "!=")
    {
        parsedQuery.havingBinaryOperator = NOT_EQUAL;
    }
    else
    {
        return false;
    }
    return true;
}

bool checkTheValueCorrect(string val)
{
    int dotCount = 0;

    for(auto it : val)
    {
        if(it=='.')
        {
            if(dotCount==1)
            {
                return false;
            }
            else
            {
                dotCount++;
            }
        }
        else if(isdigit(it))
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool syntacticParseGROUP(){
    logger.log("syntacticParseGROUP");
    if(tokenizedQuery.size()!= 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = GROUP;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByRelationName = tokenizedQuery[6];
    parsedQuery.groupByGroupingColName = tokenizedQuery[4];
    
    string lastQuery = tokenizedQuery[12];

    if(!isLastPartCorrect(lastQuery))
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }


    string havingPart = tokenizedQuery[8];

    if(!isHavingPartCorrect(havingPart))
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    string operatorType = tokenizedQuery[9];

    if(!isBinaryOperatorPartCorrect(operatorType))
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    if(!checkTheValueCorrect(tokenizedQuery[10]))
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    parsedQuery.havingAttributeValue = stod(tokenizedQuery[10]);
    return true;
}

bool semanticParseGROUP(){
    logger.log("semanticParseGROUP");

    if(tableCatalogue.isTable(parsedQuery.groupByResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.groupByRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByGroupingColName, parsedQuery.groupByRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregateColName, parsedQuery.groupByRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByHavingColName, parsedQuery.groupByRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void checkAndInsertToResult(int currGroupVal,int currGroupMemberCount,int currHavingVal,int currGroupID,Table* resultantTable)
{
    int valueToMatchInHavingCondition;
    int valueThatMightPresentInResulantTable;

    if(parsedQuery.aggregate_Func==MIN || parsedQuery.aggregate_Func==MAX || parsedQuery.aggregate_Func==SUM)
    {
        valueThatMightPresentInResulantTable = currGroupVal;
    }
    else if(parsedQuery.aggregate_Func==AVG)
    {
        valueThatMightPresentInResulantTable = currGroupVal/currGroupMemberCount;
    }
    else
    {
        valueThatMightPresentInResulantTable = currGroupMemberCount;
    }

    // cout<<"valueThatMightPresentInResulantTable : "<<valueThatMightPresentInResulantTable<<endl;
    // cout<<"currHavingVal : "<<currHavingVal<<endl;
    // cout<<"Member count : "<<currGroupMemberCount<<endl;


    if(parsedQuery.havingAggregate_Func==MIN || parsedQuery.havingAggregate_Func==MAX || parsedQuery.havingAggregate_Func==SUM)
    {
        valueToMatchInHavingCondition = currHavingVal;
    }
    else if(parsedQuery.havingAggregate_Func==AVG)
    {
        valueToMatchInHavingCondition = currHavingVal/currGroupMemberCount;
    }
    else
    {
        valueToMatchInHavingCondition = currGroupMemberCount;
    }

    vector<int> resultantRow = {currGroupID,valueThatMightPresentInResulantTable};

    if(parsedQuery.havingBinaryOperator == LESS_THAN && valueToMatchInHavingCondition<parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    else if(parsedQuery.havingBinaryOperator == GREATER_THAN && valueToMatchInHavingCondition>parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    else if(parsedQuery.havingBinaryOperator == LEQ && valueToMatchInHavingCondition<=parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    else if(parsedQuery.havingBinaryOperator == GEQ && valueToMatchInHavingCondition>=parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    else if(parsedQuery.havingBinaryOperator == NOT_EQUAL && valueToMatchInHavingCondition!=parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    else if(parsedQuery.havingBinaryOperator == EQUAL && valueToMatchInHavingCondition==parsedQuery.havingAttributeValue)
    {
        resultantTable->writeRow<int>(resultantRow);
        // rowsOfResultantTable.push_back({currGroupID,valueThatMightPresentInResulantTable});
    }
    return;
}

void executeGROUP(){
    logger.log("executeGROUP");


    Table *mainTable = tableCatalogue.getTable(parsedQuery.groupByRelationName);


    string aggFun = "";

    switch(parsedQuery.aggregate_Func)
	{
		case MIN:
			aggFun = "MIN" ; break ;
		case MAX:
			aggFun = "MAX" ; break ;
		case SUM:
			aggFun = "SUM" ; break ;
		case COUNT:
			aggFun = "COUNT" ; break ;
        case AVG:
			aggFun = "AVG" ; break ;    
	}

    vector<string> colNamesInResultantTable = {parsedQuery.groupByGroupingColName,aggFun+parsedQuery.groupByAggregateColName};
    Table* resultantTable = new Table(parsedQuery.groupByResultRelationName,colNamesInResultantTable);


    vector<string>projectionColumnList = {parsedQuery.groupByGroupingColName,parsedQuery.groupByAggregateColName,parsedQuery.groupByHavingColName};
    Table* projectTable = new Table("project"+mainTable->tableName,projectionColumnList);


    int groupBYColPosition = mainTable->getColumnIndex(parsedQuery.groupByGroupingColName);
    int aggColPosition = mainTable->getColumnIndex(parsedQuery.groupByAggregateColName);
    int havingColPosition = mainTable->getColumnIndex(parsedQuery.groupByHavingColName);

    vector<int> columnIndices;
    columnIndices.push_back(groupBYColPosition);
    columnIndices.push_back(aggColPosition);
    columnIndices.push_back(havingColPosition);


    Cursor projectCursor = mainTable->getCursor();
	vector<int> row = projectCursor.getNext();
    vector<int> resultantRow(columnIndices.size(), 0);

    while (!row.empty())
    {

        for (int columnCounter = 0; columnCounter < columnIndices.size(); columnCounter++)
        {
            resultantRow[columnCounter] = row[columnIndices[columnCounter]];
        }
        projectTable->writeRow<int>(resultantRow);
        row = projectCursor.getNext();
    }
    projectTable->blockify();
    tableCatalogue.insertTable(projectTable);
    // Project Operation completes here


    // Call Sort funnction here
    projectTable->sorting(projectTable->tableName,{{0,1}},bufferManager);
    // return;
    bufferManager.clearBuffer();


    Cursor cursor = projectTable->getCursor();
    row = cursor.getNext();

    // vector<vector<int>> rowsOfResultantTable;

    int currGroupID = row[0];
    int currGroupVal = row[1];
    int currHavingVal = row[2];
    int currGroupMemberCount = 1;

    row = cursor.getNext();
    // bool isLastIdSame = false;


    while(row.size()!=0)
    {
        int nextGroupID = row[0];
        int nextGroupVal = row[1];
        int nextHavingVal = row[2];

        // cout<<"Previous : "<<currGroupID<<" "<<currGroupVal<<" "<<currHavingVal<<currGroupMemberCount<<endl;
        // cout<<"Curr : "<<nextGroupID<<" "<<nextGroupVal<<" "<<nextHavingVal<<currGroupMemberCount<<endl;
        

        if(currGroupID==nextGroupID)
        {
            currGroupMemberCount++;
            if(parsedQuery.aggregate_Func==MIN)
            {
                currGroupVal = min(currGroupVal,nextGroupVal);
            }

            else if(parsedQuery.aggregate_Func==MAX)
            {
                currGroupVal = max(currGroupVal,nextGroupVal);
            }

            else if(parsedQuery.aggregate_Func==SUM || parsedQuery.aggregate_Func==AVG)
            {
                currGroupVal+=nextGroupVal;
            }
            
            // HAVING VALA PART
            if(parsedQuery.havingAggregate_Func==MIN)
            {
                currHavingVal = min(currHavingVal,nextHavingVal);
            }

            else if(parsedQuery.havingAggregate_Func==MAX)
            {
                currHavingVal = max(currHavingVal,nextHavingVal);
            }

            else if(parsedQuery.havingAggregate_Func==SUM || parsedQuery.havingAggregate_Func==AVG)
            {
                currHavingVal+=nextHavingVal;
            }
        }

        else
        {
            checkAndInsertToResult(currGroupVal,currGroupMemberCount,currHavingVal,currGroupID,resultantTable);
            currGroupID = nextGroupID;
            currGroupVal = nextGroupVal;
            currHavingVal = nextHavingVal;
            currGroupMemberCount = 1;
        }
        row = cursor.getNext();
    }

    checkAndInsertToResult(currGroupVal,currGroupMemberCount,currHavingVal,currGroupID,resultantTable);
    
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);

    if(projectTable->deleteTable())
    {
        tableCatalogue.deleteTable(projectTable->tableName);
    }
    return;
}