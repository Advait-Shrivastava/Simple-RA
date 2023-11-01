## Project Phase 2

## Commands:

For all the commands initially a syntactic check and followed by a semantic check is performed.

### EXTERNAL SORT ALGORITHM:

#### Logic:
The logic of the external sort was to divide the available buffer pages into two parts. We keep one page separate to do the write-out operations and we keep the remaining ```(bufferSize-1)``` for doing the external sort of operation which we define (bufferSize - 1) as ```Nr```. 


#### 1) Initial Sort 

- We start the algorithm by initially sorting every page of the file with a custom comparator and using the default sort functionality of the C++ we sort that in the order provided by the user. 

- Once we have the initial all the pages sorted, we go for the external merge sort.  

- For that as specified above we have used a recursive algorithm.  

#### 2) Merge Sort Algorithm 

- Firstly, we check the file size (the number of pages that a file contains) to see if it is lesser than the ```Nr.``` If it is less than Nr, we merge them up, and in this single merge pass, the entire file will be sorted. 

- Now if the size of the file is greater than the size of the buffer then we'll require multiple passes of the merge. In this case, we first do a step we call ```Zeroth``` pass of the merge sort which is merging up files in the batch of ```Nr.``` We divide the let us say file contains  ```Nb``` number of pages in the smaller groups of size Nr. We will have total ```ceil (Nb/Nr)``` groups. After merging each of these groups we push this range into a vector of pairs int the format ```{start, end}``` (end is exclusive). 

- Once we are over with the ```Zeroth pass```, we end up with a vector of pairs which contains the ranges of the merged intervals. 

- Now we keep on merging these ranges till we are left with only one entry in the vector, one crucial point to note is that when we merge the ranges, we combine them and push new combined ranges in the vector. 

#### 3) Merge 

- In the merge algorithm we receive a vector of ranges which are required to be merged and all these ranges are internally sorted. The number of the ranges is less than equal to the ```Nr.```  

- We make a ```priority queue``` using the standard STL priority queue data structure and we customize it with a custom comparator which follows the same order as the order provided by the user. 

- Now we load up the initial rows of the first page for every range.  

- Now we go inside and loop and continue iterating till the priority queue is empty in every iteration of this loop we will pop one entry from the priority queue, and we push this entry inside a separate vector. 

- On popping entry, we are going to push inside an entry from the same page from which this entry which currently popped belongs to. 

- To achieve this, we push two things to the priority queue one the entry and we also push the range index which this entry belongs to this help us to push the next entry from the corresponding page/range. This also helps us to perform a ```stable sort.``` 
- If a page is over and it is all the entries are already pushed, then we push the next page in the range for which we use a data structure to store till what pages we have already processed for the range. 
- Now for the write out when the vector which stores the popped-out entries from the priority queue if it becomes equal to the size of the  ``` max row count in a page``` than we write the content of the vector in the page. 
- It is important to note that we change the name of the page written by prefixing the name with string ```“temp"```. This is being done to prevent overwriting of a page which is not yet merged and contains unmerged page data.

---

### JOIN COMMAND:

#### Logic:

The entire content of the join command was divided into 3 segments, namely JOIN_EQUAL, JOIN_NOTEQUAL and JOIN_OTHERS

JOIN_EQUAL: 
To join two tables based on their equality of an attribute the following operations were performed: 

1) Both the tables were sorted in ascending order. 
2) JOIN_MERGE Algorithm was performed in order to efficienty join.

JOIN_MERGE Algorithm: https://www.youtube.com/watch?v=jiWCPJtDE2c

JOIN_NOTEQUAL: 
To join two tables on their inequality (!=) of a operator the following operations were performed:

1) The tables are not sorted in any order. 
2) Here we perform a nested loop access of the blocks. Here we assumed that the no of block were the attributes are matching are very less compared to the size of the relation. Hence sorting the files will only lead to an external overhead with very little benefit. 

JOIN_OTHERS: 
To join two tables on their inequality (<, <=, >, >=) of a operator the following operations were performed:

1) For < and <=, we sort the first relation in ascending order and the second relation in descending oder. 
For every element of the first relation we then perform linear scan till the condition is first invalid.
2) For > and >=, we sort the first relation in descending order and the first relation in ascending oder. 
For every element of the first relation we then perform linear scan till the condition is first invalid.

---

### GROUP BY HAVING COMMAND:

#### Logic:
1) In the case of a ```group by``` operation, sorting is essential for row comparison. However, to avoid excessive overhead from sorting the entire table with unnecessary columns, we opt to project only the essential columns. 
2) The only columns required for comparing two rows are the ones used in the ```"group by"``` operation, the attributes on which we apply an aggregate function, and the column specified in the ```"having"``` condition.
3) Once we've projected these columns into a new table, we proceed to perform an in-place sort on this projected table. The sorting is carried out in ascending order based on the ```group by``` attribute.
4) After sorting iterate through all the rows of the table, maintain temporary variable storing the conditional logic and result for aggregate function. If the aggregate value satisfies the conditional logic, then that group will be present in the result. 


---

### ORDER BY COMMAND:

#### Logic:
1) "Order by" is just like sorting, but the main difference is that it makes a new table to store the result, while sorting is done in place.
2) We used the table's copy constructor to make a new table that is an exact copy of the original, but with a different name.
3) Once the new table is created, we proceed to invoke the sort function, which performs in-place sorting on this newly created table.


---
## Learnings:

Project Learning Highlights:

Throughout our project, we gained valuable insights and skills that significantly contributed to our success:

JOIN-MERGE Algorithm: A comprehensive understanding of the join-merge sort algorithm for equijoin was studied. 

Use of External Sorting: We understood how sorting a file can help increase the efficiency in performing various operations like JOIN, GROUP BY HAVING and ORDER BY.

In Group By with having we learned that it is not necessary to work with every columns when it is not needed. We can ignore the column by projecting only the relevant ones.


## Contribution:
Contribution of each member in the Project:

1. EXTERNAL SORTING - Pranshu, Advait 
2. JOIN - Yash
3. GROUP BY HAVING - Advait , Pranshu
4. ORDER BY - Yash
5. PARSING - Advait, Pranshu, Yash


To coordinate as a team we kept multiple meet regularly used drawing platforms like micro and excalidraw.