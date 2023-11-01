#pragma once
// #include "cursor.h"
#include "executor.h" 
#include <bits/stdc++.h>
#include <iostream>
#include <cstdio>

int buffer_size;
// total page in the table
int total_page;
// order in which columns need to be sorted 
vector<vector<int>> order;
// Directory path to the folder where page are made 
string dirPath = "../data/temp/";

// BufferManager sortBufferManager;

// // name in the format eg. "../data/temp/A_Page"
// string temptablename;
// string tablename;



// // just name without path eg. "A"
// string tname;

// // max number of rows in a page
// int mxRows;

// bool sortDeleteFile(const string &filePath)
// {
//     if (remove(filePath.c_str()) != 0)
//     {
//         logger.log("Error deleting file: " + filePath);
//         return false;
//     }
//     else
//     {
//         logger.log("File deleted successfully: " + filePath);
//         return true;
//     }
// }

// bool sortRenameFile(const string &oldFilePath, const string &newFilePath)
// {
//     if (rename(oldFilePath.c_str(), newFilePath.c_str()) != 0)
//     {
//         logger.log("Error renaming file from: " + oldFilePath + " to: " + newFilePath);
//         return false;
//     }
//     else
//     {
//         logger.log("File renamed successfully from: " + oldFilePath + " to: " + newFilePath);
//         return true;
//     }
// }
// vector<vector<int>> sortReadCSV(string filename)
// {
//     vector<vector<int>> data;
//     ifstream file(filename);

//     if (!file.is_open())
//     {
//         logger.log("Error: Could not open the file " + filename);
//         return data;
//     }

//     string line;
//     while (getline(file, line))
//     {
//         vector<int> row;
//         stringstream lineStream(line);
//         string cell;

//         while (getline(lineStream, cell, ' '))
//         {
//             try
//             {
//                 int cellValue = stoi(cell);
//                 row.push_back(cellValue);
//             }
//             catch (const invalid_argument &e)
//             {
//                 logger.log("Error: Invalid integer in CSV: " + cell);
//             }
//             catch (const out_of_range &e)
//             {
//                 logger.log("Error: Integer out of range in CSV: " + cell);
//             }
//         }

//         data.push_back(row);
//     }

//     file.close();
//     return data;
// }

// void sortWriteCSV(const string &filename, const vector<vector<int>> &data)
// {
//     ofstream file(filename);

//     if (!file.is_open())
//     {
//         logger.log("Error: Could not open the file " + filename + " for writing.");
//         return;
//     }

//     for (const vector<int> &row : data)
//     {
//         for (size_t i = 0; i < row.size(); ++i)
//         {
//             file << row[i];
//             if (i < row.size() - 1)
//             {
//                 file << " ";
//             }
//         }
//         file << endl;
//     }

//     file.close();
//     logger.log("CSV data written to " + filename + " successfully.");
// }























// // range of merge will contain start-end page(both included) for a file
// struct CustomComparator
// {
//     const vector<vector<int>> &order;

//     CustomComparator(const vector<vector<int>> &order) : order(order) {}

//     bool operator()(const pair<vector<int>, int> &a1, const pair<vector<int>, int> &b1) const
//     {
//         vector<int> a = a1.first;
//         vector<int> b = b1.first;
//         for (int i = 0; i < order.size(); i++)
//         {
//             int index = order[i][0];
//             // Check if both vectors have elements at the specified index

//             int aValue = a[index];
//             int bValue = b[index];

//             if (order[i][1] == 1)
//             {
//                 if (aValue > bValue)
//                 {
//                     return true;
//                 }
//                 else if (aValue < bValue)
//                 {
//                     return false;
//                 }
//             }
//             else
//             {
//                 if (aValue < bValue)
//                 {
//                     return true;
//                 }
//                 else if (aValue > bValue)
//                 {
//                     return false;
//                 }
//             }
//         }
//         // If all elements are equal based on the specified order, return false
//         return a1.second > b1.second;
//     }
//     bool operator()(const vector<int> &a, const vector<int> &b) const
//     {

//         for (int i = 0; i < order.size(); ++i)
//         {
//             int index = order[i][0];
//             // Check if both vectors have elements at the specified index

//             int aValue = a[index];
//             int bValue = b[index];

//             if (order[i][1] == 1)
//             {
//                 if (aValue < bValue)
//                 {
//                     return true;
//                 }
//                 else if (aValue > bValue)
//                 {
//                     return false;
//                 }
//             }
//             else
//             {
//                 if (aValue > bValue)
//                 {
//                     return true;
//                 }
//                 else if (aValue < bValue)
//                 {
//                     return false;
//                 }
//             }
//         }
//         // If all elements are equal based on the specified order, return false
//         return false;
//     }
// };














// void initialSort()
// {
//     cout << "Initial Sorting" << endl;
//     CustomComparator customComparator(order);
//     cout << total_page << endl;
//     for (int i = 0; i < total_page; i++)
//     {
//         // string pageName = tablename + to_string(i);
//         // vector<vector<int>> curr = sortReadCSV(pageName);
//         // stable_sort(curr.begin(), curr.end(), customComparator);
//         // sortWriteCSV(pageName, curr);
//         cout << i << endl;
//         Cursor wantPage = Cursor(tname, i);
//         vector<vector<int>> curr;
//         while (true)
//         {
//             vector<int> rowFromCur = wantPage.getNextRow();
//             if (rowFromCur.size() == 0)
//                 break;
//             else
//                 curr.push_back(rowFromCur);
//         }
//         cout << "" << curr.size() << endl;
//         stable_sort(curr.begin(), curr.end(), customComparator);
//         sortBufferManager.writePage(tname, i, curr, curr.size());
//     }
// }
// void merge(vector<vector<int>> &rangeOfMerge, vector<vector<int>> &order)
// {
//     // cout << "Merging:: For range " << rangeOfMerge[0][0] << "-" << rangeOfMerge[rangeOfMerge.size() - 1][1] << endl;
//     logger.log("Merging:: For range " + to_string(rangeOfMerge[0][0]) + " - " + to_string(rangeOfMerge[rangeOfMerge.size() - 1][1]));
//     if (rangeOfMerge.size() < 2)
//     {
//         logger.log("Only single bucket:- " + to_string(rangeOfMerge[0][0]) + " " + to_string(rangeOfMerge[0][1]));
//         logger.log("Returning call");
//         return;
//     }
//     cout << rangeOfMerge.size() << endl;
//     if (rangeOfMerge.size() == 0)
//         return;
//     vector<int> ithBucketPage;
//     vector<int> pagePointer;
//     vector<string> tempfileName;
//     vector<string> OrigfileName;
//     CustomComparator customComparator(order);
//     priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator> pq(customComparator);

//     vector<vector<vector<int>>> kPages;
//     vector<vector<int>> output;

//     int blockNumber = rangeOfMerge[0][0];

//     for (int i = 0; i < rangeOfMerge.size(); i++)
//     {

//         logger.log("{" + to_string(rangeOfMerge[i][0]) + "-" + to_string(rangeOfMerge[i][1] - 1) + "}");
//         ithBucketPage.push_back(rangeOfMerge[i][0]);
//         string currpageName = tablename + to_string(rangeOfMerge[i][0]);
//         vector<vector<int>> curr = sortReadCSV(currpageName);
//         sortDeleteFile(currpageName);
//         // pushing first element of all
//         pq.push({curr[0], i});
//         kPages.push_back(curr);
//         pagePointer.push_back(0);
//     }
//     // cout << "\n";

//     while (!pq.empty())
//     {
//         pair<vector<int>, int> topMost = pq.top();
//         pq.pop();
//         output.push_back(topMost.first);
//         int kth = topMost.second;
//         pagePointer[kth]++;
//         //  checking if all the data of that page is already done
//         if (pagePointer[kth] == kPages[kth].size())
//         {
//             //  checking if any page is still remaing of the range
//             ithBucketPage[kth]++;
//             if (ithBucketPage[kth] < rangeOfMerge[kth][1])
//             {
//                 string currpageName = tablename + to_string(ithBucketPage[kth]);
//                 pagePointer[kth] = 0;
//                 vector<vector<int>> curr = sortReadCSV(currpageName);
//                 sortDeleteFile(currpageName);
//                 pq.push({curr[0], kth});
//                 kPages[kth] = curr;
//             }
//         }
//         else
//         {
//             pq.push({kPages[kth][pagePointer[kth]], kth});
//         }
//         // checking if the write-out block is full
//         /*REVIEW - HARD CODE*/
//         if (output.size() == mxRows)
//         {
//             string writeOutName = temptablename + to_string(blockNumber);
//             tempfileName.push_back(writeOutName);
//             OrigfileName.push_back(tablename + to_string(blockNumber));
//             sortWriteCSV(writeOutName, output);
//             blockNumber++;
//             output.clear();
//         }
//     }

//     if (output.size() > 0)
//     {
//         string writeOutName = temptablename + to_string(blockNumber);
//         OrigfileName.push_back(tablename + to_string(blockNumber));
//         tempfileName.push_back(writeOutName);
//         sortWriteCSV(writeOutName, output);
//         blockNumber++;
//         output.clear();
//     }
//     for (int i = 0; i < tempfileName.size(); i++)
//     {
//         sortRenameFile(tempfileName[i], OrigfileName[i]);
//     }
// }

// void mergesort(int l, int r)
// {
//     // Initialization
//     int totalPage = r - l;
//     vector<vector<int>> nextMergeLevel;
//     if (total_page == 1)
//         return;
    
//     if (totalPage > (buffer_size - 1))
//     {
//         // Level 0 merge
//         while (true)
//         {
//             nextMergeLevel.push_back({l, min(l + buffer_size - 1, r)});
//             vector<vector<int>> currentMergeLevel;
//             for (int i = l; i < min(l + buffer_size - 1, r); i++)
//             {
//                 currentMergeLevel.push_back({i, i + 1});
//             }

//             merge(currentMergeLevel, order);
//             currentMergeLevel.clear();

//             l = min(l + buffer_size - 1, r);
//             if (l == r)
//                 break;
//         }
//         // Level 1 and above merge
//         while (nextMergeLevel.size() > 1)
//         {
//             vector<vector<int>> currentMergeLevel = nextMergeLevel;
//             nextMergeLevel.clear();
//             int count = 0;
//             int start = -1;
//             vector<vector<int>> toMerge;
//             int i = 0;
//             while (i < currentMergeLevel.size())
//             {
//                 if (start == -1)
//                 {
//                     start = currentMergeLevel[i][0];
//                 }
//                 count++;
//                 toMerge.push_back(currentMergeLevel[i]);
//                 if (count == (buffer_size - 1))
//                 {
//                     merge(toMerge, order);
//                     nextMergeLevel.push_back({start, currentMergeLevel[i][1]});
//                     toMerge.clear();
//                     start = -1;
//                     count = 0;
//                 }
//                 else
//                 {
//                     if (i + 1 == currentMergeLevel.size())
//                     {
//                         merge(toMerge, order);
//                         nextMergeLevel.push_back({start, currentMergeLevel[i][1]});
//                         toMerge.clear();
//                         start = -1;
//                     }
//                 }
//                 i++;
//             }
//         }
//     }
//     else
//     {

//         vector<vector<int>> currentMergeLevel;
//         for (int i = l; i < r; i++)
//         {
//             currentMergeLevel.push_back({i, i + 1});
//         }
//         merge(currentMergeLevel, order);
//         currentMergeLevel.clear();
//     }
// }

// void sorting(int bufferSize, int maxRowPerBlock, int totalPages, string tableName, string newTableName, vector<vector<int>> sortOrder,BufferManager &bf)
// {
//     sortBufferManager = bf;
//     buffer_size = bufferSize;
//     tablename = dirPath + newTableName + "_Page";
//     temptablename = dirPath + "temp_" + newTableName + "_Page";
//     tname = tableName;
//     total_page = totalPages;
//     order = sortOrder;
//     mxRows = maxRowPerBlock;
//     initialSort();
//     mergesort(0, total_page);
// }
