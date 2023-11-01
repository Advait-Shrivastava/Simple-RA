#include "global.h"
// using namespace std;
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeJOIN_Helper_Equal(Table *A, Table *B, vector<string> resultColumnName, BinaryOperator bin_Operator){

    Table* resultantTable = new Table(parsedQuery.joinResultRelationName, resultColumnName);

    // Step 1: Perform external sorting on both the files.
    // Sort R and S both in ascending order. 
    Table *R = new Table("R", A);
    Table *S = new Table("S", B);
    
    A->sorting("R",{{R->getColumnIndex(parsedQuery.joinFirstColumnName),1}},bufferManager);
    B->sorting("S",{{S->getColumnIndex(parsedQuery.joinSecondColumnName),1}},bufferManager);
    tableCatalogue.insertTable(R);
    tableCatalogue.insertTable(S);


    // Step 2: Read the entries in nested fashion. 
    Cursor r_Cursor = R->getCursor(); 
    Cursor s_Cursor = S->getCursor();

    Cursor temp_Cursor_Location = s_Cursor;

    vector<int> r_Row = r_Cursor.getNext();
    vector<int> s_Row = s_Cursor.getNext();

    int r_ColumnIndex = R->getColumnIndex(parsedQuery.joinFirstColumnName);
    int s_ColumnIndex = S->getColumnIndex(parsedQuery.joinSecondColumnName);

    int value1 = r_Row[r_ColumnIndex];
    int value2 = s_Row[s_ColumnIndex]; 

    int mark = -1; 
    
    while(!r_Row.empty() || !s_Row.empty()){ 
        
        if(mark == -1){ 

            while(value1 < value2){
                r_Row = r_Cursor.getNext();
                if(r_Row.empty()) break;
                value1 = r_Row[r_ColumnIndex];

                // cout << "Inside first while" << endl;
                // cout << "Value 1 : " << value1 << " " << "Value 2 : " << value2 << endl;
            }
            if(r_Row.empty()) break;

            while(value1 > value2){

                // Storing details of currently accessed page. 
                temp_Cursor_Location = s_Cursor;

                s_Row = s_Cursor.getNext();
                if(s_Row.empty()) 
                    break;
                value2 = s_Row[s_ColumnIndex]; 

                // cout << "Inside second while" << endl;
                // cout << "Value 1 : " << value1 << " " << "Value 2 : " << value2 << endl;
            }
            if(s_Row.empty()) break;


            // At this point both value1 and value 2 are equal and temp_Cursor_Location points to first location where they are equal. 
            // cout << "Value 1 : " << value1 << " " << "Value 2 : " << value2 << endl << endl;
            mark = 1; 

        }

        if(value1 == value2){

            vector<int> resultRow;

            for(auto entry : r_Row)
                resultRow.push_back(entry);

            for(auto entry : s_Row)
                resultRow.push_back(entry);

            // for(auto entry : resultRow)
            //     cout << entry << ", "; 
            // cout << endl;

            resultantTable->writeRow<int>(resultRow); 

            s_Row = s_Cursor.getNext();
            if(s_Row.empty()){

                // Performing s_Cursor = temp_Cursor_Location manually
                s_Cursor = temp_Cursor_Location;                
                
                s_Row = s_Cursor.getNext();
                if(s_Row.empty()) break;
                value2 = s_Row[s_ColumnIndex];

                r_Row = r_Cursor.getNext();
                if(r_Row.empty()) break;
                value1 = r_Row[r_ColumnIndex];
            }
            else{
                value2 = s_Row[s_ColumnIndex];
                // cout << "Getting new row in S" << endl;
                // cout << "Value 1 : " << value1 << " " << "Value 2 : " << value2 << endl << endl;
            }
                
        }

        else{

            // cout << "Values no longer equal. Reseting mark to -1 and moving r_Row" << endl << endl;

            s_Cursor = temp_Cursor_Location;

            s_Row = s_Cursor.getNext();
            if(s_Row.empty()) break;
            value2 = s_Row[s_ColumnIndex];

            r_Row = r_Cursor.getNext();
            if(r_Row.empty()) break;
            value1 = r_Row[r_ColumnIndex];

            // cout << "After reseting S to temp location" << endl;
            // cout << "Value 1 : " << value1 << " " << "Value 2 : " << value2 << endl << endl;

            mark = -1; 
        }      
    }

    // Check if there is any entry in result Table. If there is then create table or display that table is empty and delete the resultant table. 

    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }

    // resultantTable->makePermanent();

    if(R->deleteTable())
        tableCatalogue.deleteTable(R->tableName);

    if(S->deleteTable())
        tableCatalogue.deleteTable(S->tableName);

    bufferManager.clearBuffer();

    return; 
}


void executeJOIN_Helper_NOTEqual(Table *R, Table *S, vector<string> resultColumnName, BinaryOperator bin_Operator){

    Table* resultantTable = new Table(parsedQuery.joinResultRelationName, resultColumnName);

    // Read the entries in nested fashion. For every row in R, check every row in S
    Cursor r_Cursor = R->getCursor(); 
    vector<int> r_Row = r_Cursor.getNext();

    int r_ColumnIndex = R->getColumnIndex(parsedQuery.joinFirstColumnName);
    int s_ColumnIndex = S->getColumnIndex(parsedQuery.joinSecondColumnName);
    
    while(!r_Row.empty()){

        Cursor s_Cursor = S->getCursor();
        vector<int> s_Row = s_Cursor.getNext();

        int value1 = r_Row[r_ColumnIndex];
        int value2; 

        while(!s_Row.empty()){

            value2 = s_Row[s_ColumnIndex]; 

            if(evaluateBinOp(value1, value2, bin_Operator)){

                vector<int> resultRow;

                for(auto entry : r_Row)
                    resultRow.push_back(entry);

                for(auto entry : s_Row)
                    resultRow.push_back(entry);

                // for(auto entry : resultRow)
                //     cout << entry << ", "; 
                // cout << endl;

                resultantTable->writeRow<int>(resultRow);    
            } 

            s_Row = s_Cursor.getNext();

        }

        r_Row = r_Cursor.getNext();
    }

    // Check if there is any entry in result Table. If there is then create table or display that table is empty and delete the resultant table. 

    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }

    // resultantTable->makePermanent();

    bufferManager.clearBuffer(); 

    return; 
}


void executeJOIN_Helper_Other(Table *A, Table *B, vector<string> resultColumnName, BinaryOperator bin_Operator){

    Table* resultantTable = new Table(parsedQuery.joinResultRelationName, resultColumnName);

    // Step 1: Perform external sorting on both the files.
    /******* Sort R and S *******/

    Table *R = new Table("R", A);
    Table *S = new Table("S", B);

    if(bin_Operator == LESS_THAN || bin_Operator == LEQ){
        // Sort R is ascending order and S in descding order. 
        A->sorting("R", {{R->getColumnIndex(parsedQuery.joinFirstColumnName),1}},bufferManager);
        B->sorting("S", {{S->getColumnIndex(parsedQuery.joinSecondColumnName),0}},bufferManager);
    }
    else{
        // Sort R is descending order and S in ascending order 
        A->sorting("R", {{R->getColumnIndex(parsedQuery.joinFirstColumnName),0}},bufferManager);
        B->sorting("S", {{S->getColumnIndex(parsedQuery.joinSecondColumnName),1}},bufferManager);
    }

    tableCatalogue.insertTable(R);
    tableCatalogue.insertTable(S);

    // Step 2: Read the entries in nested fashion. 
    Cursor r_Cursor = R->getCursor(); 
    vector<int> r_Row = r_Cursor.getNext();

    int r_ColumnIndex = R->getColumnIndex(parsedQuery.joinFirstColumnName);
    int s_ColumnIndex = S->getColumnIndex(parsedQuery.joinSecondColumnName);
    
    while(!r_Row.empty()){

        Cursor s_Cursor = S->getCursor();
        vector<int> s_Row = s_Cursor.getNext();

        int value1 = r_Row[r_ColumnIndex];
        int value2; 

        while(!s_Row.empty()){

            value2 = s_Row[s_ColumnIndex]; 

            if(evaluateBinOp(value1, value2, bin_Operator)){

                vector<int> resultRow;

                for(auto entry : r_Row)
                    resultRow.push_back(entry);

                for(auto entry : s_Row)
                    resultRow.push_back(entry);

                // for(auto entry : resultRow)
                //     cout << entry << ", "; 
                // cout << endl;

                resultantTable->writeRow<int>(resultRow);   

                s_Row = s_Cursor.getNext(); 
            } 
            else
                break;
        }

        r_Row = r_Cursor.getNext();
    }

    // Check if there is any entry in result Table. If there is then create table or display that table is empty and delete the resultant table. 

    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }

    // resultantTable->makePermanent();

    if(R->deleteTable())
        tableCatalogue.deleteTable(R->tableName);
    
    if(S->deleteTable())
        tableCatalogue.deleteTable(S->tableName);
    
    bufferManager.clearBuffer();

    return; 
}

void executeJOIN()
{
    logger.log("executeJOIN");

    // Step 1: Preprocessing for executing join. 
    Table *R = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table *S = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    vector<string> resultColumnName;
    for(auto entry : R->columns)
        resultColumnName.push_back(entry);

    for(auto entry : S->columns)
        resultColumnName.push_back(entry);

    logger.log("Pre-Processing done");        
    
    if(parsedQuery.joinBinaryOperator == EQUAL)
        executeJOIN_Helper_Equal(R, S, resultColumnName, parsedQuery.joinBinaryOperator);
    else if (parsedQuery.joinBinaryOperator == NOT_EQUAL)
         executeJOIN_Helper_NOTEqual(R, S, resultColumnName, parsedQuery.joinBinaryOperator);
    else 
        executeJOIN_Helper_Other(R, S, resultColumnName, parsedQuery.joinBinaryOperator);

    return;
}