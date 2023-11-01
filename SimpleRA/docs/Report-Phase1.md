## Project Phase 1

### General Page Design to store matrices:

To deal with matrices we decided to follow a unquie page design where rather than storing each row of the matrix line by line (a single row won't fit in a 1000B block if n > 250) into the pages, we divide the larger matrix (if required) into smaller matrices such that they can be stored within a single page.

#### Ideal Size of smaller Matrix:

To compute the idea size of the smaller matrix we use the formula :

Size of smaller matrix = $\lfloor \sqrt{(BLOCK-SIZE*1000B)/(SIZE-OF-INT)} \rfloor = \lfloor \sqrt{(1*1000)/4} \rfloor = \lfloor \sqrt{250} \rfloor = \lfloor 15.81 \rfloor = 15$

This means that for all matrices that have size \<= 15 can be stored in a single page while matrices having size > 15 will be divided into 15x15 matrices and stored.

#### Mapping of individual matrices onto the pages will be as follows :

(0,0) = Stores the elements between (0,0) and (14,14)

(0,1) = Stores the elements between (0,15) and (14,29)

(0,2) = Stores the elements between (0, 30) and (14, 44)

(1,0) - Stores the elements between (15,0) and (29,14)

(1,1) - Stores the elements between (15,15) and (29,29)

(1,2) -Stores the elements between (15,30) and (29,44)

(2,0) - Stores the elements between (30,0) and (44,14)

(2,1) - Stores the elements between (30,15) and (44,29)

(2,2) - Stores the elements between (30,30) and (44,44)

and so on......

#### Visual Representation:

![Alt text](Matrix%20Page%20Design.png)

#### Advantages of this page design:

Due to our page design, operations like TRANSPOSE, CHECKSYMMETRY, COMPUTE can be performed with less number of block access and in least time. Logic of how is described below.

#### Disadvantages of this page design:

The main disadvantage of our design is Internal Fragmentation.

For each page we have around 250-225 = 15B of memory that is being wasted.

Suppose for any nxn matrix, assume you need NxN blocks.

Total memory required = (N\*N\*1000B)

For all blocks other than rightmost column and bottom row (which we assume are empty), total memory lost due to internal fragmentation is = ((N\*N) - (2\*N-1))\*15B

Memory lost for rightmost column and bottom row in worst case (single column filled, but for simple calculated we assume it to be empty) = (2\*N-1)\*1000B

Percentage of internal fragmentation = (((N\*N) - (2\*N-1))\*15B + (2\*N-1)\*1000B) / (N\*N\*1000B)

\= 1.5% as n tends towards infinity.

## Commands:

For all the commands initially a syntactic check and followed by a semantic check is performed.

### LOAD MATRIX:

#### Logic:

1.  Open the file using its name and create a empty 3D matrix (storage) to store its contents.
2.  Iterate over the contents of the file line by line.
    1.  For every line check if column number (c\_counter) is divisible by the maximum number of columns (and rows) you can store in a block (blockMatSize).
        *   If it is then it indicates the start of data for a new page. Think of it as moving from block A to block B in our above representation.
        *   So we then check whether a 2D matrix representing this page is already present in our storage matrix or not.
        *   If not, we add it.
    2.  While updating our storage matrix, we also update some attribute, namely:
        *   rangeOfPage: Given the page name it tell us the number of rows and columns filled in that page.
        *   columnCount: Informs us about the number of columns in our matrix.
    3.  When the r\_counter is divisible by blockMatSize, the data required to store an entire row of pages (Think A, B, and C in our above diagram) is present in out storage matrix and hence we create those pages.
3.  Check the corner case if the .csv file provided was empty or not.
    * Incase it was empty return false.

#### Number of block access based on input size:
2 x Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

#### Error handling:

Proper syntactic and semantic parsing is done to avoid any errors. 

---

### PRINT MATRIX:

#### Logic:

1.  Determine the number of rows you need to print - min (20, Dimentions of matrix) (count)
2.  Calculate the number of pages (rows and columns) that needed to be accessed in order to print count.
3.  Define a matrix that stores all those elements. (matrixElements)
4.  Use cursor to iterator through the pages, extract each row and update matrixElements.
5.  Print every row of matrixElement onto the terminal.

#### Number of block access based on input size:

Due to the count constraint, at max only 4 blocks will be accessed.

#### Error handling:

Proper syntactic and semantic parsing is done to avoid any errors. 

---

### TRANSPOSE MATRIX:

#### Logic:

An important obeservation to make in our page design is that to find the transpose, all we need to do is exchange the page (i,j) with page (j,i) and transpose the content of page (i,j) and page (j,i), both of which are single pages and can be accessed using 1 block access.

1.  Iterate over all the pages used to store our matrix.
2.  For every page where i == j :
    1.  Bring the page into the main memory via buffer (called as page1).
    2.  Create a new transposed page, whose rows matrix is the transposed rows matrix of page1
    3.  Write this new transposed page into the disk.
    4.  Remove page1 from main memory.
3.  For every page where i != j:
    1.  Bring both our pages i.e. (i, j) and (j, i) into main memory (called as page1 and page2)
    2.  Create two new trasposed pages, whose rows matrix will be the tranposed rows matrix of page1 and page2.
    3.  Write both these transposed pages into the disk.
    4.  Remove page1 and page2 from main memory.

#### Number of block access based on input size:

Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

At a time at max only two block are brought in the main memory.

#### Error handling:

Proper syntactic and semantic parsing is done to avoid any errors.

---

### EXPORT MATRIX:

#### Logic:

The logic of export is very similar to that of print. We just don't stop at 20 row and columns.

1.  Calculate the number of pages (rows and columns) that needed to be accessed.
2.  Define a matrix that stores all those elements. (matrixElements)
3.  Use cursor to iterator through the pages, extract each row and update matrixElements.
4.  Print every row of matrixElement onto the output file.

#### Number of block access based on input size:

2 x Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

At a time at max only two block are brought in the main memory.

#### Error handling:
Proper syntactic and semantic parsing is done to avoid any errors.

---

### RENAME MATRIX:

#### Logic:

1. Here we have to update all the pages in main memory as well as the table catalogue.
2. To update the pages in main memeory, we iterate through them, replace the old name with new name and attach the rest of the substring as it is.
3. To update the table catalogue, we simple erase the old matrix and insert the new one.


#### Number of block access based on input size: 
Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

At a time at max only two block are brought in the main memory.

#### Error handling: 
Proper syntactic and semantic parsing is done to avoid any errors.

---

### CHECKSYMMETRY:
#### Logic: 
An important obeservation to make in our page design is that to check if the matrix is symmetric, all we need to do is compare the content of page (i,j) with page (j,i) to see if they are equal. For diagonal pages (i, i) we just need to compare its upper triangular matrix with its lower triangular matrix. 


1. Iterate over all the pages used to store our matrix. 
2. For every page where i == j : 
    1. Bring the page into the main memory via buffer (called as page1). 
    2. Compare its upper triangular matrix with lower matrix for equality. (implemented in checkSymmetryOfDiagonal)

3. For every page where i != j: 
    1. Bring both our pages i.e. (i, j) and (j, i) into main memory (called as page1 and page2)
    2. Compare their contents for equality. (implemented in checkSymmetryOfNonDiagonal)

#### Number of block access based on input size: 
Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

At a time at max only two block are brought in the main memory.

#### Error handling: 

Proper syntactic and semantic parsing is done to avoid any errors. 

--- 

### COMPUTE:

#### Logic: 
An important obeservation to make in our page design is that to perform the compute operation, all we need to do is subtract the content of page (j,i) from page (i,j) and replace elements in page(j,i) with negative of elements in page (i,j). For diagonal pages (i, i) we need to perform similar operation but with just itself.

1. Create a new result matrix (resultMatrix)
2. Iterate over all the pages used to store our matrix. 
3. For every page where i == j : 
    1. Bring the page into the main memory via buffer (called as page1). 
    2. Store its copy into another page.
    3. Perform the subtraction as mentioned above (implemented in differenceOfTwoMatrixDiagonal)
    4. Store the result into a new page and write this page.

4. For every page where i != j: 
    1. Bring both our pages i.e. (i, j) and (j, i) into main memory (called as page1 and page2)
    2. Perform the subtraction as mentioned above (implemented in differenceOfTwoMatrixNonDiagonal)
    3. Store the result into a new page and write this page.

#### Number of block access based on input size: 
Number of Pages = O(Pages) 

Number of Pages = $(Dimension-of-Matrix / Maximum-rows-in-each-block)^2$ = O($n^2$)

At a time at max only two block are brought in the main memory.
#### Error handling: 

Proper syntactic and semantic parsing is done to avoid any errors. 

---

## Assumptions:

1.  While running the application, no changes are made to the temp folder which contains all the files containing pages. This means no files in the temp folder are manually deleted, renamed and modified. 
2.  Files are properly formatted . CSV files do not contain additional extra empty lines. This is required for the load command to work properly.
3.  Subtraction of two values doesn't result in any integer underflow or overflow.

## Learnings:

Project Learning Highlights:

Throughout our project, we gained valuable insights and skills that significantly contributed to our success:

Make Command: We understood the 'make' command, streamlining our workflow and automating tasks.

Git and GitHub: Effective version control and collaboration were key in managing our codebase and working cohesively as a team.

Teamwork: We improved our communication, task delegation, and conflict resolution abilities.

Codebase Management: Handling a complex codebase with multiple files became second nature, ensuring maintainability.

DBMS Page and Architecture Design: We gained expertise in designing and implementing database management system pages and architecture.

## Contribution:
Contribution of each member in the Project:

1. Architecture - Pranshu, Advait, Yash
2. LOAD - Pranshu, Advait, Yash
3. TRANSPOSE - Advait, Pranshu
4. RENAME - Advait, Yash
5. PRINT - Yash, Pranshu 
6. COMPUTE - Pranshu, Advait
7. EXPORT - Yash, Advait
8. CHECKSYMMETRY - Pranshu, Yash

To coordinate as a team we kept multiple meet regularly used drawing platforms like micro and excalidraw.