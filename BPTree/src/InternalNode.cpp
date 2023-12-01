#include "InternalNode.hpp"

// creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr)
{
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

// max element from tree rooted at this node
Key InternalNode::max()
{
    Key max_key = DELETE_MARKER;
    TreeNode *last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

// if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr()
{
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

// inserts <key, record_ptr> into subtree rooted at this node.
// returns pointer to split node if exists
// TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr)
{
    TreePtr new_tree_ptr = NULL_PTR;

    // We'll have 3 steps here :
    // Step 1 : Find the correct position where we need to insert this key and call insert on this position.
    // Step 2 : Check if this lead to internal splitting withing the nodes. If Yes then we need to add this splitted key into our current internal node. If child node is leafnode then don't delete the first element else delete the first element.
    // Step 3 :  Check if this node is going to split.

    // this->keys.erase(this->keys.begin() + this->size-1, this->keys.end());

    // Step 1 :

    TreePtr insertInto = NULL_PTR;

    if (key < this->keys[0])
        insertInto = this->tree_pointers[0];
    else
    {
        for (int i = 0; i < this->keys.size() - 1; i++)
        {
            if (this->keys[i] <= key && key < this->keys[i + 1])
                insertInto = this->tree_pointers[i + 1];
            break;
        }
    }

    if (is_null(insertInto))
        insertInto = this->tree_pointers[this->tree_pointers.size() - 1];

    TreeNode *child_node = TreeNode::tree_node_factory(insertInto);
    TreePtr potential_split_node_ptr = child_node->insert_key(key, record_ptr);

    // Step 2 :

    if (!is_null(potential_split_node_ptr))
    {
        TreeNode *splitted_ChildNode = tree_node_factory(potential_split_node_ptr);

        Key newKey = child_node->max();

        // cout << newKey << endl;
        // for(int i =0; i<this->keys.size(); i++)
        //     cout << this->keys[i] << " ";
        // cout << endl;

        // Insert this key into our current node.
        this->keys.push_back(newKey);
        int insertion_Into_Internal_Node_KeyIndex = this->keys.size() - 2;
        for (; insertion_Into_Internal_Node_KeyIndex >= 0; insertion_Into_Internal_Node_KeyIndex--)
        {
            if (this->keys[insertion_Into_Internal_Node_KeyIndex] > newKey)
                this->keys[insertion_Into_Internal_Node_KeyIndex + 1] = this->keys[insertion_Into_Internal_Node_KeyIndex];
            else
                break;
        }
        this->keys[insertion_Into_Internal_Node_KeyIndex + 1] = newKey;

        // for(int i =0; i<this->keys.size(); i++)
        //     cout << this->keys[i] << " ";
        // cout << endl;

        // for(int i =0; i<this->tree_pointers.size(); i++)
        //     cout << this->tree_pointers[i] << " ";
        // cout << endl;

        // Insert the corresponding childpointer into our current node.
        this->tree_pointers.push_back(potential_split_node_ptr);
        TreePtr temp_Treeptr = potential_split_node_ptr;
        int insertion_Into_Internal_Node_TreePtrIndex = this->tree_pointers.size() - 2;

        for (; insertion_Into_Internal_Node_TreePtrIndex > insertion_Into_Internal_Node_KeyIndex + 1; insertion_Into_Internal_Node_TreePtrIndex--)
            this->tree_pointers[insertion_Into_Internal_Node_TreePtrIndex + 1] = this->tree_pointers[insertion_Into_Internal_Node_TreePtrIndex];

        this->tree_pointers[insertion_Into_Internal_Node_TreePtrIndex + 1] = potential_split_node_ptr;

        this->size++;

        // for(int i =0; i<this->tree_pointers.size(); i++)
        //     cout << this->tree_pointers[i] << " ";
        // cout << endl;

        // Step 3 :
        if (this->overflows())
        {
            InternalNode *newInternal = (InternalNode *)tree_node_factory(INTERNAL);
            new_tree_ptr = newInternal->tree_ptr;

            // cout << "Before->This : " << endl;

            // for(int i =0; i<this->keys.size(); i++)
            //     cout << this->keys[i] << " ";
            // cout << endl;

            // for(int i =0; i<this->tree_pointers.size(); i++)
            //     cout << this->tree_pointers[i] << " ";
            // cout << endl;

            // cout << "Before->newInternal : " << endl;

            // for(int i =0; i<newInternal->keys.size(); i++)
            //     cout << newInternal->keys[i] << " ";
            // cout << endl;

            // for(int i =0; i<newInternal->tree_pointers.size(); i++)
            //     cout << newInternal->tree_pointers[i] << " ";
            // cout << endl;

            int removalIndex = (int)MIN_OCCUPANCY;
            copy(this->keys.begin() + removalIndex, this->keys.end(), back_inserter(newInternal->keys));
            copy(this->tree_pointers.begin() + removalIndex, this->tree_pointers.end(), back_inserter(newInternal->tree_pointers));

            this->keys.erase(this->keys.begin() + removalIndex-1, this->keys.end());
            this->tree_pointers.erase(this->tree_pointers.begin() + removalIndex, this->tree_pointers.end()); 

            // cout << "After->This : " << endl;

            // for(int i =0; i<this->keys.size(); i++)
            //     cout << this->keys[i] << " ";
            // cout << endl;

            // for(int i =0; i<this->tree_pointers.size(); i++)
            //     cout << this->tree_pointers[i] << " ";
            // cout << endl;

            // cout << "After->newInternal : " << endl;

            // for(int i =0; i<newInternal->keys.size(); i++)
            //     cout << newInternal->keys[i] << " ";
            // cout << endl;

            // for(int i =0; i<newInternal->tree_pointers.size(); i++)
            //     cout << newInternal->tree_pointers[i] << " ";
            // cout << endl;

            newInternal->size = newInternal->tree_pointers.size();
            this->size = this->tree_pointers.size();

            newInternal->dump();
        }
    }

    this->dump();
    return new_tree_ptr;
}

void InternalNode :: leftBorrow(TreePtr leftSib,TreePtr rightSib, TreePtr parentPtr, int posOfParent,InternalNode *thisNode)
{
    InternalNode *thisNodeParent = new InternalNode(parentPtr);
    InternalNode *thisNodeLeftSib = new InternalNode(leftSib);

    // NOTE - PARENT INDEX
    /* Borrow from the left sibling create inconsistensies at parent
        Finding the index to update in parent */
    int parentPosChange = -1;
    if (rightSib == NULL_PTR)
    {
        parentPosChange = posOfParent;
    }
        
    else
    {
        parentPosChange = (posOfParent - 1);
    }
        

    // NOTE - LEFT SIBLING
    /* Last key of the left sibling(Maximum) and the record that follows it
            Pi Ki "Pi+1"
                as last key Ki is being shifted we'll also have to shift the pointer after the Ki
    */
    Key maxKeyValue = thisNodeLeftSib->keys[thisNodeLeftSib->keys.size() - 1];
    TreePtr ptrToRightOfMaxKeyValue = thisNodeLeftSib->tree_pointers[thisNodeLeftSib->tree_pointers.size() - 1];

    // NOTE - CURRENT NODE
    /* Insert the parent key to current node creates a void in parent*/
    vector<Key> newKeyVector;
    newKeyVector.push_back(thisNodeParent->keys[parentPosChange]);
    for (auto i : thisNode->keys)
        newKeyVector.push_back(i);
    thisNode->keys = newKeyVector;
    // thisNode->keys.insert(thisNode->keys.begin(), thisNodeParent->keys[parentPosChange]);

    // NOTE - INSERTING THE POINTER
    /*Insert of the pointer in front of the tree pointers As it now points to left of the newly inserted key from the parent(left sibling)*/
    vector<TreePtr> newPointerVector;
    newPointerVector.push_back(ptrToRightOfMaxKeyValue);
    for (auto i : thisNode->tree_pointers)
        newPointerVector.push_back(i);
    thisNode->tree_pointers = newPointerVector;
    // thisNode->tree_pointers.insert(thisNode->tree_pointers.begin(), ptrToRightOfMaxKeyValue);

    // NOTE - PARENT RESTORE
    thisNodeParent->keys[parentPosChange] = maxKeyValue;

    // NOTE - REMOVING THE POINTERS
    thisNodeLeftSib->keys.pop_back();
    thisNodeLeftSib->tree_pointers.pop_back();

    // NOTE - RESIZING
    thisNodeLeftSib->size = thisNodeLeftSib->tree_pointers.size();
    thisNode->size = thisNode->tree_pointers.size();

    // ANCHOR - WRITE BACKS
    thisNodeLeftSib->dump();
    thisNodeParent->dump();
    thisNode->dump();

    delete thisNodeLeftSib;
    delete thisNodeParent;
    return;

}

void InternalNode :: leftMerge(TreePtr leftSib,TreePtr rightSib, TreePtr parentPtr, int posOfParent,InternalNode *thisNode)
{
    // NOTE - Creating Nodes of PARENT and LEFT SIBLING
    InternalNode *thisNodeParent = new InternalNode(parentPtr);
    InternalNode *thisNodeLeftSib = new InternalNode(leftSib);

    // NOTE - Position to be removed from PARENT after merger
    /*Once we merge two subtree will become one Thus leads to reduction in number of keys and pointers in parent*/
    int parentRemovePos = -1;

    if (rightSib != NULL_PTR)
    {
        parentRemovePos = posOfParent - 1;
    }
        
    else
    {
        parentRemovePos = posOfParent;
    }
        

    // NOTE - Adding the parent to the left sibling keys
    thisNodeLeftSib->keys.push_back(thisNodeParent->keys[parentRemovePos]);

    // NOTE - Adding the current node keys to the parent
    for (auto i : thisNode->keys)
    {
        thisNodeLeftSib->keys.push_back(i);
    }
        

    // NOTE - Adding the root pointers to the left subtree
    for (auto i : thisNode->tree_pointers)
    {
        thisNodeLeftSib->tree_pointers.push_back(i);
    }
        

    thisNodeParent->keys.erase(thisNodeParent->keys.begin() + parentRemovePos);
    thisNodeParent->tree_pointers.erase(thisNodeParent->tree_pointers.begin() + parentRemovePos + 1);

    // NOTE -  Modifying the size of the parent and left sibling
    thisNodeParent->size -= 1;
    thisNodeLeftSib->size += (this->size + 1);
    thisNodeParent->dump();

    // NOTE -  Removing elements from the current node
    //  thisNode->keys.clear();
    //  thisNode->tree_pointers.clear();

    thisNodeLeftSib->dump();
    delete thisNodeParent;
    delete thisNodeLeftSib;
    return;
}

void InternalNode :: rightBorrow(TreePtr leftSib,TreePtr rightSib, TreePtr parentPtr, int posOfParent,InternalNode *thisNode)
{
    InternalNode *thisNodeParent = new InternalNode(parentPtr);
    InternalNode *thisNodeRightSib = new InternalNode(rightSib);

    // NOTE - Inserting the parent key to the current Node :: First step of shifting
    thisNode->keys.push_back(thisNodeParent->keys[posOfParent]);

    // NOTE - Getting the smallest value from the right sibling
    Key minKeyValue = thisNodeRightSib->keys[0];
    TreePtr max_record_ptr = thisNodeRightSib->tree_pointers[0];

    // NOTE - Replacing that smallest at parent :: Step 2 of the shifting
    thisNodeParent->keys[posOfParent] = minKeyValue;

    // NOTE - Inserting the tree pointer
    thisNode->tree_pointers.push_back(max_record_ptr);

    // NOTE - Erasing the values from the right siblings
    thisNodeRightSib->keys.erase(thisNodeRightSib->keys.begin());
    thisNodeRightSib->tree_pointers.erase(thisNodeRightSib->tree_pointers.begin());

    // NOTE - Resizing
    thisNodeRightSib->size -= 1;
    // thisNode->size = thisNode->tree_pointers.size();
    thisNode->size = thisNode->tree_pointers.size();

    // ANCHOR - Write back the values
    thisNodeRightSib->dump();
    thisNodeParent->dump();
    thisNode->dump();

    delete thisNodeRightSib;
    delete thisNodeParent;
    return;

}

void InternalNode :: rightMerge(TreePtr leftSib,TreePtr rightSib, TreePtr parentPtr, int posOfParent,InternalNode *thisNode)
{
    // NOTE - Creating Nodes from the pointers
    InternalNode *thisNodeRightSib = new InternalNode(rightSib);
    InternalNode *thisNodeParent = new InternalNode(parentPtr);

    // LINK - 3 step process

    // NOTE - Step 1 of merge, pull the key from the parent and push back in this node
    thisNode->keys.push_back(thisNodeParent->keys[posOfParent]);

    // NOTE - Step 2 Part 1 of the merge :: Add all the keys from the right sib to this node
    for (auto i : thisNodeRightSib->keys)
    {
        thisNode->keys.push_back(i);
    }

    // NOTE - Step 2 Part 2 of the merge :: Add all the corresponding pointers of the right sib node
    for (auto i : thisNodeRightSib->tree_pointers)
    {
        thisNode->tree_pointers.push_back(i);
    }

    // NOTE - Step 3 Erasing the key we pulled from the parent

    thisNodeParent->keys.erase(thisNodeParent->keys.begin() + posOfParent);
    thisNodeParent->tree_pointers.erase(thisNodeParent->tree_pointers.begin() + posOfParent + 1);


    thisNodeRightSib->keys.clear();
    thisNodeRightSib->tree_pointers.clear();

    // NOTE - Modifying the size data variable
    thisNodeParent->size = thisNodeParent->tree_pointers.size();
    thisNode->size = thisNode->tree_pointers.size();


    // ANCHOR - WriteBack the values
    thisNodeParent->dump();
    thisNode->dump();

    delete thisNodeParent;
    delete thisNodeRightSib;
    return;
}

void InternalNode::delete_key_checks(const Key &key){
    // cout<<"CHECKING FOR INTERNAL NODE"<<endl;
    cout<<this->keys.size()<<endl;
    for(int i = 0; i < this->keys.size(); i++){
        if(key <= this->keys[i]){
            TreeNode *childNode = TreeNode::tree_node_factory(this->tree_pointers[i]);
            if(key == this->keys[i]){
                // cout<<"DELETED KEY PRESENT AT INTERNAL NODE --> REMOVING"<<endl;
                // cout<<"CHANGING "<<this->keys[i]<<" to "<<childNode->max()<<endl;
                this->keys[i] = childNode->max();
                this->dump();
            }
            else if(childNode->node_type == INTERNAL){
                // cout<<"DELETED KEY NOT PRESENT AT INTERNAL NODE --> GOING TO CHILD"<<endl;
                InternalNode *ChildNode = new InternalNode(this->tree_pointers[i]);
                ChildNode->delete_key_checks(key);
            }
            else return;
        }
    }
}

// deletes key from subtree rooted at this if exists
// TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key, TreePtr leftSib, TreePtr rightSib, TreePtr parentPtr, int posOfParent)
{

    // declaration of child sibling pointers
    TreePtr pointerOfChildLeftSib = NULL_PTR;
    TreePtr pointerOfChildRightSib = NULL_PTR;

    // declaration of position of left/right siblings
    int posOfChildRightSib = -1;
    int posOfChildLeftSib = -1;

    int n = this->keys.size();
    int i = 0;
    bool flagCheck = false;
    while (i < n)
    {

        posOfChildRightSib = i + 1;
        posOfChildLeftSib = i - 1;

        if (key <= this->keys[i])
        {
            flagCheck = true;
            if (i != 0)
            {
                pointerOfChildLeftSib = this->tree_pointers[posOfChildLeftSib];
            }
            else
            {
                pointerOfChildLeftSib = NULL_PTR;
            }
            pointerOfChildRightSib = this->tree_pointers[posOfChildRightSib];
            TreeNode *thisNode = TreeNode::tree_node_factory(this->tree_pointers[i]);
            thisNode->delete_key(key, pointerOfChildLeftSib, pointerOfChildRightSib, this->tree_ptr, i); // recursion
        }
        if (flagCheck)
        {
            break;
        }
            
        if (i == (n - 1))
        {
            flagCheck = true;

            pointerOfChildRightSib = NULL_PTR;
            posOfChildLeftSib = i;

            pointerOfChildLeftSib = this->tree_pointers[posOfChildLeftSib];

            TreeNode *currChild = tree_node_factory(this->tree_pointers[i + 1]);
            currChild->delete_key(key, pointerOfChildLeftSib, pointerOfChildRightSib, this->tree_ptr, i); // recursion
        }

        if (flagCheck)
        {
            break;
        }
            
        i++;
    }

    InternalNode *thisNode = new InternalNode(this->tree_ptr);

    if (thisNode->keys.size() >= ceil((((double)(FANOUT)) / 2) - 1))
    {
        return;
    }

    // ANCHOR - Left Sibling
    if (leftSib != NULL_PTR)
    {
        // Obtain the left node from the left pointer
        InternalNode *thisNodeLeftSib = new InternalNode(leftSib);

        if (thisNodeLeftSib->keys.size() >= ((FANOUT + 1) / 2))
        {
            leftBorrow(leftSib,rightSib,parentPtr,posOfParent,thisNode);
        }

        else
        {
            leftMerge(leftSib,rightSib,parentPtr,posOfParent,thisNode);
        }
    }

    // ANCHOR - Right Sibling
    if (rightSib != NULL_PTR)
    {
        InternalNode *thisNodeRightSib = new InternalNode(rightSib);

        if (thisNodeRightSib->keys.size() >= ((FANOUT + 1) / 2))
        {
            rightBorrow(leftSib,rightSib,parentPtr,posOfParent,thisNode);            
        }
        else
        {
            rightMerge(leftSib,rightSib,parentPtr,posOfParent,thisNode);
        }
    }
}




// runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const
{
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++)
    {
        if (min_key <= this->keys[i])
        {
            auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

// exports node - used for grading
void InternalNode::export_node(ostream &os)
{
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++)
    {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

// writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os)
{
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++)
    {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1)
        {
            link += to_string(this->keys[i - 1]) + " < x";
        }
        else
        {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream &InternalNode::write(ostream &os) const
{
    TreeNode::write(os);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream &InternalNode::read(istream &is)
{
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
