#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
    this->blockMatSize= sqrt((BLOCK_SIZE * 1000) / int(sizeof(int)));
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->blockMatSize= sqrt((BLOCK_SIZE * 1000) / int(sizeof(int)));
}

/**
 * @brief Construct a new Matrix:: Matrix object used when an assignment command
 * is encountered. To create the matrix object both the matrix name and the
 * columns the matrix holds should be specified.
 *
 * @param matrixName
 * @param columns
 */
// Matrix::Matrix(string matrixName, vector<string> columns)
// {
//     logger.log("Matrix::Matrix");
//     this->sourceFileName = "../data/temp/" + matrixName + ".csv";
//     this->matrixName = matrixName;
//     // this->columns = columns;
//     this->columnCount = columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
//     this->writeRow<string>(columns);
// }

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load");

    string line;
    if (this->blockify())
        return true;
    return false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    logger.log("Matrix::blockify::blockMatsize " + to_string(blockMatSize));
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<vector<vector<int>>> storage;

    int r_counter = 0;
    while (getline(fin, line))
    {
        /*DEBUG: logger.log("Matrix::blockify::ReadingLine Line#"+to_string(r_counter)); */

        // converts the string into stream
        stringstream s(line);

        int c_counter = 0;

        while (getline(s, word, ','))
        {
            /*DEBUG: logger.log("Matrix::blockify::ReadingWord word#"+to_string(c_counter)); */
            if (c_counter % blockMatSize == 0)
            {
                /*DEBUG: logger.log("Matrix::blockify::shiftingToNextBlock(widthoverflow)); */
                logger.log("Matrix::blockify::shiftingToNextBlock(widthoverflow)");
                if (storage.size() <= (c_counter / blockMatSize))
                {
                    /*DEBUG: logger.log("Matrix::blockify::makingANewBlock(widthoverflow)); */
                    vector<vector<int>> smallMat(blockMatSize, vector<int>(blockMatSize, 0));
                    storage.push_back(smallMat);
                }
            }
            /*DEBUG: logger.log("Matrix::blockify::storingAnElement"); */
            storage[c_counter / blockMatSize][r_counter % blockMatSize][c_counter % blockMatSize] = stoi(word);
            /*DEBUG: logger.log("Matrix::blockify::makingName"); */
            string nameOfPage = this->matrixName + "_matPage" + "_" + to_string((r_counter / blockMatSize)) + "_" + to_string((c_counter / blockMatSize));
            /*DEBUG: logger.log("Matrix::blockify::updatingRange"); */
            this->rangeOfPage[nameOfPage] = {r_counter % blockMatSize, c_counter % blockMatSize};
            c_counter++;
        }
        if (this->columnCount == 0)
        {
            logger.log("Matrix::blockify::storingFinalCountValue");
            this->columnCount = c_counter;
        }
        r_counter++;
        if ((r_counter) % blockMatSize == 0)
        {
            logger.log("Matrix::blockify::savingBufferToPage");

            int pageToSave = ceil((double)c_counter / blockMatSize);
            logger.log("Matrix::blockify::savingBufferToPage::PageCount " + to_string(pageToSave) + " CurrentRowCount" + to_string(r_counter) + " CurrentColumnCount" + to_string(c_counter));
            for (int i = 0; i < pageToSave; i++)
            {
                blockCount++;
                bufferManager.writePageMatrix(this->matrixName, {r_counter / blockMatSize - 1, i}, storage[i], blockMatSize);
            }
            storage.clear();
        }
    }
    if (storage.size() > 0)
    {
        logger.log("Matrix::blockify::savingBufferToPage::Outer");
        int pageToSave = ceil((double)columnCount / blockMatSize);
        logger.log("Matrix::blockify::savingBufferToPage::PageCount " + to_string(pageToSave) + " CurrentRowCount" + to_string(r_counter));
        for (int i = 0; i < pageToSave; i++)
        {
            blockCount++;
            bufferManager.writePageMatrix(this->matrixName, {r_counter / blockMatSize, i}, storage[i], blockMatSize);
        }
        storage.clear();
    }
    // for(auto it : rangeOfPage)
    // {
    //     cout<<it.first<<" "<<it.second.first<<" "<<it.second.second<<endl;
    // }
    this->rowCount = r_counter;

    if (this->rowCount == 0)
        return false;
    // this->distinctValuesInColumns.clear();
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
void Matrix::updateStatistics(vector<int> row)
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
 * @brief Function prints the first few rows of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");

    uint count = min((long long)PRINT_COUNT, this->rowCount);
    uint noOfPageRowsNeededToBeAccessed = count / this->blockMatSize;
    uint noOfPageColumnsNeededToBeAccessed = count / this->blockMatSize;

    vector<vector<int>> matrixElements(count, vector<int>(count, 0));

    for (int row = 0; row <= noOfPageRowsNeededToBeAccessed; row++)
    {
        for (int coln = 0; coln <= noOfPageColumnsNeededToBeAccessed; coln++)
        {

            Cursor cursor(this->matrixName, {row, coln});
            vector<int> singleRow;
            for (int rowCounter = 0; rowCounter < this->blockMatSize; rowCounter++)
            {
                singleRow = cursor.getNext();

                for (int colnCounter = 0; colnCounter < singleRow.size(); colnCounter++)
                    if ((row * this->blockMatSize + rowCounter) < count && (coln * this->blockMatSize + colnCounter) < count)
                        matrixElements[row * this->blockMatSize + rowCounter][coln * this->blockMatSize + colnCounter] = singleRow[colnCounter];
            }
        }
    }

    for (int row = 0; row < count; row++)
        this->writeRow(matrixElements[row], cout);

    printRowCount(count); 
}

/**
 * @brief This function returns one row of the matrix using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";

    uint count = this->rowCount;
    uint noOfPageRowsNeededToBeAccessed = count / this->blockMatSize;
    uint noOfPageColumnsNeededToBeAccessed = count / this->blockMatSize;

    vector<vector<int>> matrixElements(count, vector<int>(count, 0));

    for (int row = 0; row <= noOfPageRowsNeededToBeAccessed; row++)
    {
        for (int coln = 0; coln <= noOfPageColumnsNeededToBeAccessed; coln++)
        {

            Cursor cursor(this->matrixName, {row, coln});
            vector<int> singleRow;
            for (int rowCounter = 0; rowCounter < this->blockMatSize; rowCounter++)
            {
                singleRow = cursor.getNext();

                for (int colnCounter = 0; colnCounter < singleRow.size(); colnCounter++)
                    if ((row * this->blockMatSize + rowCounter) < count && (coln * this->blockMatSize + colnCounter) < count)
                        matrixElements[row * this->blockMatSize + rowCounter][coln * this->blockMatSize + colnCounter] = singleRow[colnCounter];
            }
        }
    }

    ofstream fout(newSourceFile, ios::out);
    for (int row = 0; row < count; row++)
        this->writeRow(matrixElements[row], fout);
    fout.close(); 
}

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload()
{
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this matrix
 *
 * @return Cursor
 */
Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
// int Matrix::getColumnIndex(string columnName)
// {
//     logger.log("Matrix::getColumnIndex");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (this->columns[columnCounter] == columnName)
//             return columnCounter;
//     }
// }

void Matrix::renameMatrix(string toMatrixName)
{
    logger.log("Matrix::renameMatrix");
    /*NOTE - ChangingNamesOFThePages*/
    logger.log("TableCatalogue::renameMatrix::ChangingNamesofPage");
    string folderPath = "../data/temp/";
    /*Taken entry by reference so as to update the Pagename in unordered map too*/
    unordered_map<string, pair<int, int>> newRangeOfPage;
    int tempValue=sqrt(this->blockCount);
    for(int i=0;i<tempValue;i++){
        for(int j=0;j<tempValue;j++)
        {
            string oldPageName = this->matrixName+"_matPage_"+to_string(i)+"_"+to_string(j);
            string newPageName = toMatrixName+"_matPage_"+to_string(i)+"_"+to_string(j);
            filesystem::rename(folderPath + oldPageName, folderPath + newPageName);
            
        }
    }
    this->sourceFileName = toMatrixName;
    this->matrixName = toMatrixName;
    // for (auto &entry : this->rangeOfPage)
    // {
    //     string oldPageName = entry.first;
    //     string newPageName;
    //     int countUnderscore = 0;
    //     /*Logic to take out the suffix name from oldPath name
    //         PageName=matrixName_Page_i_j
    //         *here i,j are the matrix indices
    //     */
    //     for (int i = oldPageName.size() - 1; i >= 0; i--)
    //     {
    //         if (oldPageName[i] == '_')
    //         {
    //             countUnderscore++;
    //             if (countUnderscore == 3)
    //             {
    //                 newPageName = oldPageName.substr(i);
    //                 break;
    //             }
    //         }
    //     }
    //     newPageName = toMatrixName + newPageName;
    //     /*Add entry for newPage name and Delete Previous*/
    //     newRangeOfPage[newPageName] = entry.second;
    //     filesystem::rename(folderPath + oldPageName, folderPath + newPageName);
    // }
    logger.log("TableCatalogue::renameMatrix::ChangingNamesofPage::Done");
    return;
}

vector<vector<int>> transposeOfaPage(Page &page)
{
    logger.log("transposeOfaPage");
    vector<vector<int>> matrix = page.getMatrixRows();

    for (int i = 0; i < matrix.size(); i++)
    {
        for (int j = i + 1; j < matrix[0].size(); j++)
        {
            swap(matrix[i][j], matrix[j][i]);
        }
    }
    return matrix;
}

bool checkSymmetryOfDiagonal(Page &page)
{
    logger.log("checkSymmetryOfDiagonal");
    vector<vector<int>> matrix = page.getMatrixRows();
    for (int i = 0; i < matrix.size(); i++)
    {
        for (int j = i + 1; j < matrix[0].size(); j++)
        {
            if (matrix[i][j] != matrix[j][i])
            {
                return false;
            }
        }
    }
    return true;
}

bool checkSymmetryOfNonDiagonal(vector<vector<int>> matrix1, vector<vector<int>> matrix2)
{
    logger.log("checkSymmetryOfNonDiagonal");

    for (int i = 0; i < matrix1.size(); i++)
    {
        for (int j = 0; j < matrix1[0].size(); j++)
        {
            if (matrix1[i][j] != matrix2[j][i])
            {
                return false;
            }
        }
    }
    return true;
}

void Matrix::transposeMatrix()
{
    logger.log("Matrix::transposeMatrix");
    int blockInRow = sqrt(blockCount);

    for (int i = 0; i < blockInRow; i++)
    {
        for (int j = i; j < blockInRow; j++)
        {
            if (i == j)
            {
                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                logger.log("Matrix::transposeMatrix::FetchedPages::"+page1.pageName);
                Page transposedPage=Page(this->matrixName,{i,i},transposeOfaPage(page1),page1.getRowCount());
                // logger.log("Matrix::transposeMatrix::Transposed::"+page1.pageName);
                transposedPage.writePage();
                // logger.log("Matrix::transposeMatrix::PageWritten::"+page1.pageName);
                bufferManager.deleteFromPool(page1.pageName);
                logger.log("Matrix::transposeMatrix::PageDeleted::"+page1.pageName);
            }
            else
            {
                /*TODO - Get page calls*/
                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                Page page2 = bufferManager.getPageMatrix(this->matrixName, {j, i});
                logger.log("Matrix::transposeMatrix::FetchedPages");
                Page transposedPage1=Page(this->matrixName,{j,i},transposeOfaPage(page1),page1.getRowCount());
                Page transposedPage2=Page(this->matrixName,{i,j},transposeOfaPage(page2),page2.getRowCount());
                transposedPage1.writePage();
                transposedPage2.writePage();
                // logger.log("Matrix::transposeMatrix::TransposeDone");
                /*Deleting pages from the bufferManager*/
                logger.log("Matrix::transposeMatrix::PageDeleted");
                bufferManager.deleteFromPool(page1.pageName);
                bufferManager.deleteFromPool(page2.pageName);
            }
        }
    }
}

bool Matrix::checkSymmetry()
{
    int blockInRow = sqrt(blockCount);
    logger.log("Matrix::checkSymmetry");
    for (int i = 0; i < blockInRow; i++)
    {
        for (int j = i; j < blockInRow; j++)
        {
            if (i == j)
            {
                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                if (checkSymmetryOfDiagonal(page1) == false)
                {
                    return false;
                }
            }
            else
            {
                /*TODO - Get page calls*/
                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                Page page2 = bufferManager.getPageMatrix(this->matrixName, {j, i});

                if (checkSymmetryOfNonDiagonal(page1.getMatrixRows(), page2.getMatrixRows()) == false)
                {
                    return false;
                }
            }
        }
    }
    return true;
}
vector<vector<int>> differenceOfTwoMatrixDiagonal(Page &page1)
{
    logger.log("differenceOfTwoMatrixDiagonal");
    vector<vector<int>> v1=page1.getMatrixRows();
    for (int i = 0; i < v1.size(); i++)
    {
        for (int j = i; j < v1.size(); j++)
        {
            v1[i][j] -= v1[j][i];
            v1[j][i]=(-1*v1[i][j]);
        }
    }
    return v1;
}
pair<vector<vector<int>>,vector<vector<int>>> differenceOfTwoMatrixNonDiagonal(Page &page1, Page &page2)
{
    logger.log("differenceOfTwoMatrixNonDiagonal");
    vector<vector<int>> v1=page1.getMatrixRows();
    vector<vector<int>> v2=page2.getMatrixRows();
    for (int i = 0; i < v1.size(); i++)
    {
        for (int j = 0; j < v1.size(); j++)
        {
            v1[i][j] -= v2[j][i];
            v2[j][i] = -1 * v1[i][j];
        }
    }
    return {v1,v2};
}

Matrix *Matrix ::computeMatrix()
{
    logger.log("Matrix::computeMatrix::PreprocessingResultMatrix");
    string resultMatrixName = this->matrixName + "_RESULT";
    Matrix *resultMatrix = new Matrix(resultMatrixName);
    resultMatrix->blockCount = this->blockCount;
    resultMatrix->columnCount = this->columnCount;
    resultMatrix->rowCount = this->rowCount;
    int blockInRow = sqrt(blockCount);

    for (int i = 0; i < blockInRow; i++)
    {
        for (int j = i; j < blockInRow; j++)
        {
            if (i == j)
            {
                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                vector<vector<int>> resultCalculated=differenceOfTwoMatrixDiagonal(page1);
                // string pageName = resultMatrixName + "_matPage_" + to_string(i) + "_" + to_string(j);
                // resultMatrix->rangeOfPage[pageName] = this->rangeOfPage[page1.pageName];
                Page newPage=Page(resultMatrixName,{i,j},resultCalculated,resultCalculated.size());
                newPage.writePage();
            }
            else
            {

                Page page1 = bufferManager.getPageMatrix(this->matrixName, {i, j});
                Page page2 = bufferManager.getPageMatrix(this->matrixName, {j, i});
                pair<int, int> rangeValuePage1 = this->rangeOfPage[this->matrixName + "_matPage_" + to_string(i) + "_" + to_string(j)];
                pair<int, int> rangeValuePage2 = this->rangeOfPage[this->matrixName + "_matPage_" + to_string(j) + "_" + to_string(i)];
                pair<vector<vector<int>>,vector<vector<int>>> resultCalculated=differenceOfTwoMatrixNonDiagonal(page1, page2);
                // page1.pageName = resultMatrixName + "_matPage_" + to_string(i) + "_" + to_string(j);
                // page2.pageName = resultMatrixName + "_matPage_" + to_string(j) + "_" + to_string(i);
                // resultMatrix->rangeOfPage[page1.pageName] = rangeValuePage1;
                // resultMatrix->rangeOfPage[page2.pageName] = rangeValuePage2;
                Page newPage1=Page(resultMatrixName,{i,j},resultCalculated.first,resultCalculated.first.size());
                Page newPage2=Page(resultMatrixName,{j,i},resultCalculated.second,resultCalculated.first.size());
                // cout<<newPage1.pageName<<endl;
                // cout<<newPage2.pageName<<endl;
                newPage1.writePage();
                newPage2.writePage();
            }
        }
    }
    return resultMatrix;

    // Matrix()
}