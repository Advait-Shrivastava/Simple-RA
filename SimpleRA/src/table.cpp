#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}
Table ::Table(string tableName,Table* table)
{
    this->tableName = tableName;
    this->blockCount = table->blockCount;
    this->columnCount = table->columnCount;
    this->columns = table->columns;
    this->distinctValuesInColumns = table->distinctValuesInColumns;
    this->indexed = table->indexed;
    this ->indexedColumn = table->indexedColumn;
    this->indexingStrategy = table->indexingStrategy;
    this->maxRowsPerBlock = table->maxRowsPerBlock;
    this->rowCount = table->rowCount;
    this->rowsPerBlockCount = table->rowsPerBlockCount;
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        /*In essence, this line of code removes all whitespace
          characters from the string word by rearranging the 
          elements and then erasing the unnecessary elements 
          from the end.*/
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    /*NOTE - block size defined here*/
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    //this peice of code is initializing the distinctValuesInColumns
    //with length columnCount empty sets
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    // removing first line as it contains column names
    getline(fin, line);

    /*!SECTION
        writing the file content into pages 
    */
    while (getline(fin, line))
    {
        // converts the string into stream
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            /*!SECTION
                The code getline(s, word, ',') is used to extract data
                from a string s until a specified delimiter, in this
                case, a comma ,. Let's break down what each parameter does:
            */
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        /*NOTE
            first check's if for this columns if this value has occured already
            if not then it stores the new distict value and increments the 
            distinct value counter
        */
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

bool Table::sortDeleteFile(string filePath)
{
    if (remove(filePath.c_str()) != 0)
    {
        logger.log("Error deleting file: " + filePath);
        return false;
    }
    else
    {
        logger.log("File deleted successfully: " + filePath);
        return true;
    }
}

bool Table:: deleteTable()
{
    string dirPath = "../data/temp/";

    for(int i = 0;i<this->blockCount;i++)
    {
        string filePath = dirPath+this->tableName+"_Page"+to_string(i);
        sortDeleteFile(filePath);
    }
    return true;
}

bool Table::sortRenameFile(string &oldFilePath, const string &newFilePath)
{
    if (rename(oldFilePath.c_str(), newFilePath.c_str()) != 0)
    {
        logger.log("Error renaming file from: " + oldFilePath + " to: " + newFilePath);
        return false;
    }
    else
    {
        logger.log("File renamed successfully from: " + oldFilePath + " to: " + newFilePath);
        return true;
    }
}

vector<vector<int>> Table::sortReadCSV(string filename)
{
    vector<vector<int>> data;
    ifstream file(filename);

    if (!file.is_open())
    {
        logger.log("Error: Could not open the file " + filename);
        return data;
    }

    string line;
    while (getline(file, line))
    {
        vector<int> row;
        stringstream lineStream(line);
        string cell;

        while (getline(lineStream, cell, ' '))
        {
            try
            {
                int cellValue = stoi(cell);
                row.push_back(cellValue);
            }
            catch (const invalid_argument &e)
            {
                logger.log("Error: Invalid integer in CSV: " + cell);
            }
            catch (const out_of_range &e)
            {
                logger.log("Error: Integer out of range in CSV: " + cell);
            }
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

void Table:: sortWriteCSV(const string &filename, const vector<vector<int>> &data)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        logger.log("Error: Could not open the file " + filename + " for writing.");
        return;
    }

    for (const vector<int> &row : data)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            file << row[i];
            if (i < row.size() - 1)
            {
                file << " ";
            }
        }
        file << endl;
    }

    file.close();
    logger.log("CSV data written to " + filename + " successfully.");
}

struct CustomComparator
{
    vector<vector<int>> order;

    CustomComparator(const vector<vector<int>> &order) : order(order) {}

    bool operator()(const pair<vector<int>, int> &a1, const pair<vector<int>, int> &b1) const
    {
        vector<int> a = a1.first;
        vector<int> b = b1.first;
        for (int i = 0; i < order.size(); i++)
        {
            int index = order[i][0];
            // Check if both vectors have elements at the specified index

            int aValue = a[index];
            int bValue = b[index];

            if (order[i][1] == 1)
            {
                if (aValue > bValue)
                {
                    return true;
                }
                else if (aValue < bValue)
                {
                    return false;
                }
            }
            else
            {
                if (aValue < bValue)
                {
                    return true;
                }
                else if (aValue > bValue)
                {
                    return false;
                }
            }
        }
        // If all elements are equal based on the specified order, return false
        return a1.second > b1.second;
    }
    bool operator()(const vector<int> &a, const vector<int> &b) const
    {

        for (int i = 0; i < order.size(); ++i)
        {
            int index = order[i][0];
            // Check if both vectors have elements at the specified index

            int aValue = a[index];
            int bValue = b[index];

            if (order[i][1] == 1)
            {
                if (aValue < bValue)
                {
                    return true;
                }
                else if (aValue > bValue)
                {
                    return false;
                }
            }
            else
            {
                if (aValue > bValue)
                {
                    return true;
                }
                else if (aValue < bValue)
                {
                    return false;
                }
            }
        }
        // If all elements are equal based on the specified order, return false
        return false;
    }
};









void Table::initialSort(string newTableName,vector<vector<int>> order,BufferManager &bf)
{
    // cout << "Initial Sorting" << endl;
    CustomComparator customComparator(order);
    int total_page = this->blockCount;
    // cout << total_page << endl;
    for (int i = 0; i < total_page; i++)
    {
        // string pageName = tablename + to_string(i);
        // vector<vector<int>> curr = sortReadCSV(pageName);
        // stable_sort(curr.begin(), curr.end(), customComparator);
        // sortWriteCSV(pageName, curr);
        // cout << i << endl;
        Cursor wantPage = Cursor(this->tableName, i);
        vector<vector<int>> curr;
        while (true)
        {
            vector<int> rowFromCur = wantPage.getNextRow();
            if (rowFromCur.size() == 0)
                break;
            else
                curr.push_back(rowFromCur);
        }
        // cout << "" << curr.size() << endl;
        stable_sort(curr.begin(), curr.end(), customComparator);
        bf.writePage(newTableName, i, curr, curr.size());
    }
}



void Table::merge(vector<vector<int>> &rangeOfMerge, vector<vector<int>> &order,string tablename,string temptablename)
{
    // cout << "Merging:: For range " << rangeOfMerge[0][0] << "-" << rangeOfMerge[rangeOfMerge.size() - 1][1] << endl;
    logger.log("Merging:: For range " + to_string(rangeOfMerge[0][0]) + " - " + to_string(rangeOfMerge[rangeOfMerge.size() - 1][1]));
    
    int mxRows = this->maxRowsPerBlock;

    if (rangeOfMerge.size() < 2)
    {
        logger.log("Only single bucket:- " + to_string(rangeOfMerge[0][0]) + " " + to_string(rangeOfMerge[0][1]));
        logger.log("Returning call");
        return;
    }
    // cout << rangeOfMerge.size() << endl;
    if (rangeOfMerge.size() == 0)
        return;
    vector<int> ithBucketPage;
    vector<int> pagePointer;
    vector<string> tempfileName;
    vector<string> OrigfileName;
    CustomComparator customComparator(order);
    priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator> pq(customComparator);

    vector<vector<vector<int>>> kPages;
    vector<vector<int>> output;

    int blockNumber = rangeOfMerge[0][0];

    for (int i = 0; i < rangeOfMerge.size(); i++)
    {

        logger.log("{" + to_string(rangeOfMerge[i][0]) + "-" + to_string(rangeOfMerge[i][1] - 1) + "}");
        ithBucketPage.push_back(rangeOfMerge[i][0]);
        string currpageName = tablename + to_string(rangeOfMerge[i][0]);
        vector<vector<int>> curr = sortReadCSV(currpageName);
        sortDeleteFile(currpageName);
        // pushing first element of all
        pq.push({curr[0], i});
        kPages.push_back(curr);
        pagePointer.push_back(0);
    }
    // cout << "\n";

    while (!pq.empty())
    {
        pair<vector<int>, int> topMost = pq.top();
        pq.pop();
        output.push_back(topMost.first);
        int kth = topMost.second;
        pagePointer[kth]++;
        //  checking if all the data of that page is already done
        if (pagePointer[kth] == kPages[kth].size())
        {
            //  checking if any page is still remaing of the range
            ithBucketPage[kth]++;
            if (ithBucketPage[kth] < rangeOfMerge[kth][1])
            {
                string currpageName = tablename + to_string(ithBucketPage[kth]);
                pagePointer[kth] = 0;
                vector<vector<int>> curr = sortReadCSV(currpageName);
                sortDeleteFile(currpageName);
                pq.push({curr[0], kth});
                kPages[kth] = curr;
            }
        }
        else
        {
            pq.push({kPages[kth][pagePointer[kth]], kth});
        }
        // checking if the write-out block is full
        /*REVIEW - HARD CODE*/
        if (output.size() == mxRows)
        {
            string writeOutName = temptablename + to_string(blockNumber);
            tempfileName.push_back(writeOutName);
            OrigfileName.push_back(tablename + to_string(blockNumber));
            sortWriteCSV(writeOutName, output);
            blockNumber++;
            output.clear();
        }
    }

    if (output.size() > 0)
    {
        string writeOutName = temptablename + to_string(blockNumber);
        OrigfileName.push_back(tablename + to_string(blockNumber));
        tempfileName.push_back(writeOutName);
        sortWriteCSV(writeOutName, output);
        blockNumber++;
        output.clear();
    }
    for (int i = 0; i < tempfileName.size(); i++)
    {
        sortRenameFile(tempfileName[i], OrigfileName[i]);
    }
}



void Table::mergesort(int l, int r,string tablename,string temptablename,vector<vector<int>> &order)
{
    // Initialization
    int totalPage = r - l;
    int buffer_size = BLOCK_COUNT;
    vector<vector<int>> nextMergeLevel;
    int total_page = this->blockCount;
    if (total_page == 1)
        return;
    
    if (totalPage > (buffer_size - 1))
    {
        // Level 0 merge
        while (true)
        {
            nextMergeLevel.push_back({l, min(l + buffer_size - 1, r)});
            vector<vector<int>> currentMergeLevel;
            for (int i = l; i < min(l + buffer_size - 1, r); i++)
            {
                currentMergeLevel.push_back({i, i + 1});
            }

            merge(currentMergeLevel, order,tablename,temptablename);
            currentMergeLevel.clear();

            l = min(l + buffer_size - 1, r);
            if (l == r)
                break;
        }
        // Level 1 and above merge
        while (nextMergeLevel.size() > 1)
        {
            vector<vector<int>> currentMergeLevel = nextMergeLevel;
            nextMergeLevel.clear();
            int count = 0;
            int start = -1;
            vector<vector<int>> toMerge;
            int i = 0;
            while (i < currentMergeLevel.size())
            {
                if (start == -1)
                {
                    start = currentMergeLevel[i][0];
                }
                count++;
                toMerge.push_back(currentMergeLevel[i]);
                if (count == (buffer_size - 1))
                {
                    merge(toMerge, order,tablename,temptablename);
                    nextMergeLevel.push_back({start, currentMergeLevel[i][1]});
                    toMerge.clear();
                    start = -1;
                    count = 0;
                }
                else
                {
                    if (i + 1 == currentMergeLevel.size())
                    {
                        merge(toMerge, order,tablename,temptablename);
                        nextMergeLevel.push_back({start, currentMergeLevel[i][1]});
                        toMerge.clear();
                        start = -1;
                    }
                }
                i++;
            }
        }
    }
    else
    {

        vector<vector<int>> currentMergeLevel;
        for (int i = l; i < r; i++)
        {
            currentMergeLevel.push_back({i, i + 1});
        }
        merge(currentMergeLevel, order,tablename,temptablename);
        currentMergeLevel.clear();
    }
}




void Table::sorting(string newTableName, vector<vector<int>> sortOrder,BufferManager &bf)
{
    string dirPath = "../data/temp/";
    string tablename = dirPath + newTableName + "_Page";
    string temptablename = dirPath + "temp_" + newTableName + "_Page";
    // string tname = this->tableName;
    initialSort(newTableName,sortOrder,bf);
    mergesort(0,this->blockCount,tablename,temptablename,sortOrder);
}






// void Table::sortTheTable()
// {
//     int buffer_size;
//     // total page in the table
//     int total_page;
//     // order in which columns need to be sorted 
//     vector<vector<int>> order;
//     // Directory path to the folder where page are made 
//     string dirPath = "../data/temp/";

//     BufferManager sortBufferManager;

//     // name in the format eg. "../data/temp/A_Page"
//     string temptablename;
//     string tablename;



//     // just name without path eg. "A"
//     string tname;

//     // max number of rows in a page
//     int mxRows;

// }
