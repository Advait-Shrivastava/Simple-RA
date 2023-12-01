#include "RecordPtr.hpp"
#include "LeafNode.hpp"
#include "InternalNode.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr) {
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    if(!is_null(tree_ptr))
        this->load();
}

//returns max key within this leaf
Key LeafNode::max() {
    auto it = this->data_pointers.rbegin();
    return it->first;
}

//inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
//split node is returned
//TODO: LeafNode::insert_key to be implemented
TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_leaf = NULL_PTR; //if leaf is split, new_leaf = ptr to new split node ptr

    this->data_pointers[key] = record_ptr; 
    ++this->size;

    if(this->overflows()){
        LeafNode* newLeaf = (LeafNode*)tree_node_factory(LEAF);
        newLeaf->next_leaf_ptr = this->next_leaf_ptr;
        this->next_leaf_ptr = newLeaf->tree_ptr;
        
        new_leaf = newLeaf->tree_ptr; 

        int count = 0; 
        for(auto it : this->data_pointers){
            if(count >= (int)MIN_OCCUPANCY){
                newLeaf->data_pointers[it.first] = it.second; 
                ++newLeaf->size;
            }
            else 
                count++; 
        }

        for(auto it : newLeaf->data_pointers)
            this->data_pointers.erase(it.first); 
        this->size = MIN_OCCUPANCY; 

        newLeaf->dump(); 
    }

    this->dump();
    return new_leaf;
}

//key is deleted from leaf if exists
//TODO: LeafNode::delete_key to be implemented

void LeafNode :: leftBorrow(TreePtr leftSib,TreePtr rightSib, TreePtr ptrToParent, int positionOfParent)
{
    LeafNode *NodeOfLeftSib = new LeafNode(leftSib);

    auto lastElementInNode = prev(NodeOfLeftSib->data_pointers.end());
    auto lastKey = lastElementInNode->first;
    auto pointerAtLastKey = lastElementInNode->second;

    NodeOfLeftSib->data_pointers.erase(lastKey);
    NodeOfLeftSib->size = NodeOfLeftSib->size - 1;

    this->data_pointers[lastKey] = pointerAtLastKey;
    this->size = this->size+1;

    NodeOfLeftSib->dump();

    //If internal node exists

    if(ptrToParent!=NULL_PTR)
    {
        int positionAtParentNode;
        
        if(rightSib!=NULL_PTR)
        {
            positionAtParentNode = positionOfParent - 1;
        }
        else
        {
            positionAtParentNode = positionOfParent;
        }

        InternalNode* node = new InternalNode(ptrToParent);

        auto newLastElementInSib = prev(NodeOfLeftSib->data_pointers.end());
        auto newlastKey = newLastElementInSib->first;

        node->keys[positionAtParentNode] = newlastKey;
        node->dump();
    }
    this->dump();
    return;
}

void LeafNode :: rightBorrow(TreePtr leftSib,TreePtr rightSib, TreePtr ptrToParent, int positionOfParent)
{
    LeafNode *NodeOfRightSib = new LeafNode(rightSib);

    auto firstElementInNode = NodeOfRightSib->data_pointers.begin();
    auto firstKey = firstElementInNode->first;
    auto pointerAtFirstKey = firstElementInNode->second;

    NodeOfRightSib->data_pointers.erase(firstKey);
    NodeOfRightSib->size = NodeOfRightSib->size - 1;

    this->data_pointers[firstKey] = pointerAtFirstKey;
    this->size = this->size+1;

    NodeOfRightSib->dump();

    //If internal node exists

    if(ptrToParent!=NULL_PTR)
    {
        InternalNode* node = new InternalNode(ptrToParent);

        node->keys[positionOfParent] = firstKey;
        node->dump();
    }
    this->dump();
    return;
}

void LeafNode :: leftMerge(TreePtr leftSib,TreePtr rightSib, TreePtr ptrToParent, int positionOfParent)
{

    LeafNode *NodeOfLeftSib = new LeafNode(leftSib);

    NodeOfLeftSib->size = NodeOfLeftSib->size + this->size;
    NodeOfLeftSib->data_pointers.insert(this->data_pointers.begin(),this->data_pointers.end());

    // cout<<"All values pushed, Size of left sib is now : "<<NodeOfLeftSib->data_pointers.size()<<endl;


    NodeOfLeftSib->dump();
    this->data_pointers.clear();
    this->size = 0;
    this->dump();

    NodeOfLeftSib->next_leaf_ptr = this->next_leaf_ptr;

    InternalNode* node = new InternalNode(ptrToParent);


    // cout<<"Right Sibling : "<<rightSib<<endl;

    int indexOFKeyToDelete;
    int indexOFTreePointerToDelete;

    if(rightSib!=NULL_PTR)
    {
        indexOFKeyToDelete = positionOfParent - 1;
    }

    else
    {
        indexOFKeyToDelete = node->keys.size() - 1;
    }

    if(rightSib!=NULL_PTR)
    {
        indexOFTreePointerToDelete = positionOfParent;
    }

    else
    {
        indexOFTreePointerToDelete = node->tree_pointers.size() - 1;
    }



    node->keys.erase(node->keys.begin() + indexOFKeyToDelete);
    node->tree_pointers.erase(node->tree_pointers.begin() + indexOFTreePointerToDelete);
    node->size = node->size - 1;
    node->dump();

    // cout<<"Printing the tree pointers of my parent"<<endl;
    // for(auto it : node->tree_pointers)
    // {
    //     cout<<it<<" "<<endl;
    // }

    return;
}

void LeafNode :: rightMerge(TreePtr leftSib,TreePtr rightSib, TreePtr ptrToParent, int positionOfParent)
{

    LeafNode *NodeOfRightSib = new LeafNode(rightSib);

    this->size = NodeOfRightSib->size + this->size;
    this->data_pointers.insert(NodeOfRightSib->data_pointers.begin(),NodeOfRightSib->data_pointers.end());


    this->next_leaf_ptr = NodeOfRightSib->next_leaf_ptr;

    this->dump();
    NodeOfRightSib->data_pointers.clear();
    NodeOfRightSib->size = 0;
    NodeOfRightSib->dump();

    InternalNode* node = new InternalNode(ptrToParent);

    node->keys.erase(node->keys.begin() + positionOfParent);
    node->tree_pointers.erase(node->tree_pointers.begin() + positionOfParent+1);
    node->size = node->size - 1;
    node->dump();

    return;
}


void LeafNode::delete_key(const Key &key,TreePtr leftSib,TreePtr rightSib, TreePtr ptrToParent, int positionOfParent)
{
    // cout << "LeafNode::delete_key not implemented" << endl;

    if(!this->data_pointers.count(key))
    {
        return;
    }

    
    this->data_pointers.erase(key);
    this->size = this->size-1;

    if(!this->underflows())
    {
        this->dump();
        return;
    }


    // First Priority ---> Left Sibling
    
    if(leftSib!=NULL_PTR)
    {
        LeafNode *NodeOfLeftSib = new LeafNode(leftSib);

        if((NodeOfLeftSib->size + this->size) >= 2 * MIN_OCCUPANCY)    // Borrow from left sibling
        {
            leftBorrow(leftSib,rightSib,ptrToParent,positionOfParent);
        }

        else        // Merge with left sibling
        {
            leftMerge(leftSib,rightSib,ptrToParent,positionOfParent);
        }
    }

    // Second Priority Right Sibling
    else
    {
        LeafNode *NodeOfRightSib = new LeafNode(rightSib);

        if((NodeOfRightSib->size + this->size) >= 2 * MIN_OCCUPANCY)    // Borrow from right sibling
        {
            rightBorrow(leftSib,rightSib,ptrToParent,positionOfParent);
        }

        else        // Merge with right sibling
        {
            rightMerge(leftSib,rightSib,ptrToParent,positionOfParent);
        }

    }
}







//runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(const auto& data_pointer : this->data_pointers){
        if(data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if(data_pointer.first > max_key)
            return;
    }
    if(!is_null(this->next_leaf_ptr)){
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

//exports node - used for grading
void LeafNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(const auto& data_pointer : this->data_pointers){
        os << data_pointer.first << " ";
    }
    os << endl;
}

//writes leaf as a mermaid chart
void LeafNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for(const auto& data_pointer: this->data_pointers) {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream& LeafNode::write(ostream &os) const {
    TreeNode::write(os);
    for(const auto & data_pointer : this->data_pointers){
        if(&os == &cout)
            os << "\n" << data_pointer.first << ": ";
        else
            os << "\n" << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    return os;
}

istream& LeafNode::read(istream& is){
    TreeNode::read(is);
    this->data_pointers.clear();
    for(int i = 0; i < this->size; i++){
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if(&is == &cin)
            cout << "K: ";
        is >> key;
        if(&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key,RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    return is;
}