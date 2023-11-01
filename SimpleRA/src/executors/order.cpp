#pragma once
#include"global.h"
// #include"sort.h"
// #ifndef SORT_H
// #define SORT_H
// #include "sort.h"
// #endif

/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseORDER(){
    logger.log("syntacticParseORDER");
    if(tokenizedQuery.size()!= 8 || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = ORDER;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[7];
    parsedQuery.sortColumnName = tokenizedQuery[4];
    string sortingStrategy = tokenizedQuery[5];

    if(sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    return true;
}

bool semanticParseORDER(){
    logger.log("semanticParseORDER");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void executeORDER(){
    logger.log("executeORDER");

    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    Table* resultTable = new Table(parsedQuery.sortResultRelationName, table);
 
    vector<vector<int>> orderOfSorting;
    orderOfSorting.push_back({table->getColumnIndex(parsedQuery.sortColumnName),parsedQuery.sortingStrategy==ASC});

    table->sorting(resultTable->tableName,orderOfSorting,bufferManager);
    bufferManager.clearBuffer();
    tableCatalogue.insertTable(resultTable);
    
    return;
}