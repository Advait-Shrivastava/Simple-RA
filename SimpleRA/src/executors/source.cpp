#include "global.h"
#include <iostream>
#include <fstream>
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
using namespace std;
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void dothisCommand()
{
    logger.log("doCommand");
    if (syntacticParse() && semanticParse())
        executeCommand();
    return;
}


void executeSOURCE()
{
    logger.log("executeSOURCE");
    fstream query_file;

    query_file.open("../data/"+parsedQuery.sourceFileName+".ra",ios::in);

    if (query_file.is_open()){ 

        string this_command;
        logger.log("Reading Source File");

        while(getline(query_file, this_command)){

            regex delim("[^\\s,]+");
            tokenizedQuery.clear();
            parsedQuery.clear();

            logger.log("\nReading New Command: ");
            logger.log(this_command);

            auto words_begin = std::sregex_iterator(this_command.begin(), this_command.end(), delim);
            auto words_end = std::sregex_iterator();
            for (std::sregex_iterator i = words_begin; i != words_end; ++i)
                tokenizedQuery.emplace_back((*i).str());

            if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
            {
                break;
            }

            if (tokenizedQuery.empty())
            {
                continue;
            }

            if (tokenizedQuery.size() == 1)
            {
                cout << "SYNTAX ERROR" << endl;
                continue;
            }

            dothisCommand();
        }
            
    }
    else{
        cout<<"qf is closed"<<endl;
    }
    query_file.close(); 
    return;
}
