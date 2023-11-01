#pragma once
#include "global.h"
// #include "sort.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 *
 * sorting_order = ASC | DESC
 */
// vector<string> columnNameToSort;
// vector<string> sortingStrategy;

bool syntacticParseSORT()
{
    logger.log("syntacticParseSORT");
    // if(tokenizedQuery.size()!= 8 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN"){
    //     cout<<"SYNTAX ERROR"<<endl;
    //     return false;
    // }
    // parsedQuery.queryType = SORT;
    // parsedQuery.sortResultRelationName = tokenizedQuery[0];
    // parsedQuery.sortRelationName = tokenizedQuery[3];
    // parsedQuery.sortColumnName = tokenizedQuery[5];
    // string sortingStrategy = tokenizedQuery[7];
    // if(sortingStrategy == "ASC")
    //     parsedQuery.sortingStrategy = ASC;
    // else if(sortingStrategy == "DESC")
    //     parsedQuery.sortingStrategy = DESC;
    // else{
    //     cout<<"SYNTAX ERROR"<<endl;
    //     return false;
    // }
    // return true;

    int queryLength = tokenizedQuery.size();
    int positionOfIN = -1;

    for (int i = queryLength - 1; i >= 0; i--)
    {
        if (tokenizedQuery[i] == "IN")
        {
            positionOfIN = i;
            break;
        }
    }

    // cout<<queryLength<<" "<<tokenizedQuery[2]<<" "<<positionOfIN<<endl;

    if (queryLength < 6 || queryLength % 2 != 0 || tokenizedQuery[2] != "BY" || positionOfIN == -1 || (queryLength - positionOfIN) != (positionOfIN - 2))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];

    for (int i = 3; i < positionOfIN; i++)
    {
        parsedQuery.columnNamesToSort.push_back(tokenizedQuery[i]);
    }

    for (int i = positionOfIN + 1; i < queryLength; i++)
    {
        if (tokenizedQuery[i] != "ASC" && tokenizedQuery[i] != "DESC")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        else if(tokenizedQuery[i] == "ASC")
        {
            parsedQuery.colsortingTechniques.push_back(ASC);
        }
        else
        {
            parsedQuery.colsortingTechniques.push_back(DESC);
        }
    }
    return true;
}

bool semanticParseSORT()
{
    logger.log("semanticParseSORT");

    // if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
    //     cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
    //     return false;
    // }

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    // if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
    //     cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
    //     return false;
    // }

    for (auto colName : parsedQuery.columnNamesToSort)
    {
        // cout<<colName<<endl;
        if (!tableCatalogue.isColumnFromTable(colName, parsedQuery.sortRelationName))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }
    }

    return true;
}

void executeSORT()
{

    logger.log("executeSORT");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);

    vector<vector<int>> orderOfSorting;

    for (int i = 0; i < parsedQuery.columnNamesToSort.size(); i++)
    {
        orderOfSorting.push_back({table->getColumnIndex(parsedQuery.columnNamesToSort[i]),parsedQuery.colsortingTechniques[i]==ASC});
    }

    logger.log("executeSort::Print::orderOfSorting");
    for(auto i:orderOfSorting){
        logger.log(to_string(i[0]) + " " + to_string(i[1]));
    }
    // sorting(BLOCK_COUNT,table->maxRowsPerBlock, table->blockCount, table->tableName, table->tableName,orderOfSorting,bufferManager);
    table->sorting(table->tableName,orderOfSorting,bufferManager);
    bufferManager.clearBuffer();
    orderOfSorting.clear();
    table->makePermanent();
    return;
}