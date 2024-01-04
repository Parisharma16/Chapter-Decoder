#include <iostream>
using namespace std;
#include <queue>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include<cassert>
#include <utility>

class HybridNode {
public:
    string key;     // Word
    string element;    // Chapter
    HybridNode* parent;   // Parent node
    HybridNode* left_child;  // Left child node
    HybridNode* right_child;  // Right child node
    HybridNode* next_node;  // Next node in the linked list
    string color = "red";    // Color of the node               // changed colour from black to red

    int * arr;      // histogram for every node

    HybridNode(string key_element, string element_element) : key(key_element), element(element_element), parent(NULL), left_child(NULL), right_child(NULL), next_node(NULL) {
        arr = new int[100];
        for (int i = 0; i < 100; i++) {
            arr[i] = 0;
        }
    }
  

    bool is_left() { 
        return this == parent->left_child; 
    }

    HybridNode  *find_uncle() {
        if (parent == NULL or parent->parent == NULL){
            return NULL;
        }

        if (parent->is_left()){
            return parent->parent->right_child;
        }
        else{
            return parent->parent->left_child;
        }
    }

    HybridNode  *find_sibling() {
        if (parent == NULL){
            return NULL;
        }

        if (is_left()){
            return parent->right_child;
        }

        return parent->left_child;
    }

    
    void move_node_down(HybridNode  *Parent_node) {
        if (parent != NULL) {
            if (is_left()) {
                parent->left_child = Parent_node;
            } else {
                parent->right_child = Parent_node;
            }
        }
        Parent_node->parent = parent;
        parent = Parent_node;
    }

    bool has_red_child() {
        return (left_child != NULL and left_child->color =="red") or
            (right_child != NULL and right_child->color =="red");
    }
};

class RedBlackTree {
private:
    HybridNode* root;   // Root node

   
void leftRotate(HybridNode  *x) {
	HybridNode  *Parent_node = x->right_child;
	if (x == root){
	    root = Parent_node;
    }
	x->move_node_down(Parent_node);
	x->right_child = Parent_node->left_child;
	if (Parent_node->left_child != NULL){
	    Parent_node->left_child->parent = x;
    }
	Parent_node->left_child = x;
}

void rightRotate(HybridNode  *x) {
	HybridNode  *Parent_node = x->left_child;
	if (x == root){
	    root = Parent_node;
    }
	x->move_node_down(Parent_node);
	x->left_child = Parent_node->right_child;
	if (Parent_node->right_child != NULL){
	    Parent_node->right_child->parent = x;
    }
	Parent_node->right_child = x;
}

void colour_swap(HybridNode  *x1, HybridNode  *x2) {
	string temp;
	temp = x1->color;
	x1->color = x2->color;
	x2->color = temp;
}

void value_swap(HybridNode  *u, HybridNode  *v) {
	string temp;
	temp = u->key;
	u->key = v->key;
	v->key= temp;
}

void fix_double_red_problem(HybridNode  *x) {
	if (x == root) {
	    x->color = "black";
	    return;
	}

	HybridNode  *parent = x->parent, *grandparent = parent->parent,
		*find_uncle = x->find_uncle();

	if (parent->color != "black") {
        if (find_uncle != NULL && find_uncle->color == "red") {
            parent->color = "black";
            find_uncle->color = "black";
            grandparent->color = "red";
            fix_double_red_problem(grandparent);
        } 
        else {
            // perform LR, LL, RL, RR
            if (parent->is_left()) {
                if (x->is_left()) {
                    // for left_child right_child
                    colour_swap(parent, grandparent);
                } 
                else {
                    leftRotate(parent);
                    colour_swap(x, grandparent);
                }
            // for left_child left_child and left_child right_child
                rightRotate(grandparent);
            } 
            else {
                if (x->is_left()) {
                    // for right_child left_child
                    rightRotate(parent);
                    colour_swap(x, grandparent);
                } 
                else {
                    colour_swap(parent, grandparent);
                }
		        // for right_child right_child and right_child left_child
		        leftRotate(grandparent);
		    }
	    }
	}
}

void fix_double_black_problem(HybridNode  *x) {
	if (x == root)
	return;

	HybridNode  *find_sibling = x->find_sibling(), *parent = x->parent;
	if (find_sibling == NULL) {
	    fix_double_black_problem(parent);
	} 
    else {
        if (find_sibling->color == "red") {
            parent->color = "red";
            find_sibling->color = "black";
            if (find_sibling->is_left()) {
            // left_child case
            rightRotate(parent);
            } else {
            // right_child case
            leftRotate(parent);
            }
            fix_double_black_problem(x);
        } 
        else {
            // find_sibling black
            if (find_sibling->has_red_child()) {
            // at least 1 red children
                if (find_sibling->left_child != NULL and find_sibling->left_child->color == "red") {
                    if (find_sibling->is_left()) {
                        // left_child left_child
                        find_sibling->left_child->color = find_sibling->color;
                        find_sibling->color = parent->color;
                        rightRotate(parent);
                    } 
                    else {
                        // right_child left_child
                        find_sibling->left_child->color = parent->color;
                        rightRotate(find_sibling);
                        leftRotate(parent);
                    }
                } else {
                    if (find_sibling->is_left()) {
                        // left_child right_child
                        find_sibling->right_child->color = parent->color;
                        leftRotate(find_sibling);
                        rightRotate(parent);
                    } 
                    else {
                        // right_child right_child
                        find_sibling->right_child->color = find_sibling->color;
                        find_sibling->color = parent->color;
                        leftRotate(parent);
                    }
            }
		        parent->color = "black";
		    }
             else {
                // 2 black children
                find_sibling->color = "red";
                if (parent->color == "black")
                    fix_double_black_problem(parent);
                else
                    parent->color = "black";
            }
	    }
	}
}


HybridNode  *find_successor(HybridNode  *x) {
	HybridNode  *temp = x;

	while (temp->left_child != NULL){
	    temp = temp->left_child;
    }

	return temp;
}


HybridNode  *replace_BST(HybridNode  *x) {
	// 2 children
	if (x->left_child != NULL and x->right_child != NULL){
	    return find_successor(x->right_child);
    }

	// 0 child
	if (x->left_child == NULL and x->right_child == NULL){
	    return NULL;
    }

	// 1 child
	if (x->left_child != NULL){
	    return x->left_child;
    }
	else{
	    return x->right_child;
    }
}


void deleteHybridNode (HybridNode  *v) {
	HybridNode  *u = replace_BST(v);

	bool uvBlack = ((u == NULL or u->color == "black") and (v->color == "black"));
	HybridNode  *parent = v->parent;

	if (u == NULL) {
        if (v == root) {
            root = NULL;
        }
        else {
            if (uvBlack) {
                fix_double_black_problem(v);
            } else {
                if (v->find_sibling() != NULL){
                    v->find_sibling()->color = "red";
                }
            }

            if (v->is_left()) {
                parent->left_child = NULL;
            } else {
                parent->right_child = NULL;
            }
        }
	    delete v;
	    return;
	}

	if (v->left_child == NULL or v->right_child == NULL) {
        if (v == root) {
            v->element = u->element;
            v->left_child = v->right_child = NULL;
            delete u;
        } 
        else {
            if (v->is_left()) {
            parent->left_child = u;
            } else {
            parent->right_child = u;
            }
            delete v;
            u->parent = parent;
            if (uvBlack) {
            fix_double_black_problem(u);
            } else {
            u->color = "black";
            }
        }
	return;
	}
	value_swap(u, v);
	deleteHybridNode (u);
}

void levelOrder(HybridNode  *x) {
	if (x == NULL){
	    return;
    }

	queue<HybridNode  *> q;
	HybridNode  *current ;

	q.push(x);

	while (!q.empty()) {
        current = q.front();
        q.pop();

        cout << current->key << " ";

        // push children to queue
        if (current->left_child != NULL)
            q.push(current->left_child);
        if (current->right_child != NULL)
            q.push(current->right_child);
    }
}

void inorder(HybridNode  *x) {
	if (x == NULL){
	    return;
    }
	inorder(x->left_child);
	cout << x->key << " ";
	inorder(x->right_child);
}




public:

    //RedBlackTree() : root(NULL) {}
    RedBlackTree(){
        this->root=NULL;
    }

    HybridNode* getRoot() {
        return root;  // Return the root node
    }

    void setRoot(HybridNode* node) {
        root = node;  // Set the root node
    }

    HybridNode* insert(string key, string element) {
        HybridNode *newNode = new HybridNode(key,element);
        //cout<<"1"<<endl;
        //cout<<root<<endl;
        if (root == NULL) {
            // when root is null
            // simply insert value at root
            newNode->color = "black";
            //root = newNode;
            this->setRoot(newNode);
            //cout<<"2"<<endl;
        } else {
            //cout<<"3"<<endl;
            HybridNode *temp = search_next(key);
            //cout<<"4"<<endl;
                if (temp->key == key) {
                    // return if value already exists
                    return temp;
                }
            //cout<<"5"<<endl;
            // if value is not found, search returns the node
            // where the value is to be inserted
        
            // connect new node to correct node
                newNode->parent = temp;
        
                if (key < temp->key)
                    temp->left_child = newNode;
                else
                    temp->right_child = newNode;

            // fix red red violation if exists
                fix_double_red_problem(newNode);
        }
        // Implement Red-Black Tree insertion
        //cout<<"inserted successfully"<<endl;
        return newNode;
    }

    bool deleteNode(string key) {
        // Implement Red-Black Tree deletion

        if (root == NULL)
        // Tree is empty
            return false;
    
        HybridNode *v = search_next(key);
    
        if (v->key != key) {
            cout << "No node found to delete with value:" << key<< endl;
            return false;
        }
    
        deleteHybridNode(v);
        return true;
    }

    void printInOrder() {
        cout << "Inorder: " << endl;
        if (root == NULL)
        cout << "Tree is empty" << endl;
        else
        inorder(root);
        cout << endl;
  }
 
  // prints level order of the tree
  void printLevelOrder() {
    cout << "Level order: " << endl;
    if (root == NULL)
      cout << "Tree is empty" << endl;
    else
      levelOrder(root);
    cout << endl;
  }
  
    vector<HybridNode*> traverseUp(HybridNode* node) {
        // Traverse up the tree from the given node to the root
        // Return the vector of nodes in the path
        vector<HybridNode*> path;
        while (node != NULL) {
            path.push_back(node);
            node = node->parent;
        }
        return path;
    }

    vector<HybridNode*> traverseDown(HybridNode* node, string bit_sequence) {
        // Traverse down the tree based on the bit sequence
        // Return the vector of nodes in the path
        vector<HybridNode*> path;
        if (node == NULL) {
            return path;
        }
        int index=0;
        while(node!=NULL && index<bit_sequence.size()){
            path.push_back(node);
            if(bit_sequence[index]=='1'){
                node=node->left_child;
            }else if(bit_sequence[index]=='0'){
                node=node->right_child;
            }
            index++;
        }
        return path;
    }


    vector<HybridNode *> path_vector;

    void preOrder_within_depth(HybridNode * node,int current_depth,int specified_depth){
        if(node==NULL){
            return;
        }
        path_vector.push_back(node);
        if(current_depth==specified_depth){
            return;
        }else{
            preOrder_within_depth(node->left_child,current_depth+1,specified_depth);
            preOrder_within_depth(node->right_child,current_depth+1,specified_depth);
        }
    }

    vector<HybridNode*> preOrderTraversal(HybridNode* node, int depth) {
        path_vector.clear();
        preOrder_within_depth(node,0,depth);
        return path_vector;
    }

    vector<HybridNode *> nodes;
    void nodes_list(HybridNode * node){
        if(node==NULL){
            return;
        }
        nodes.push_back(node);
        nodes_list(node->left_child);
        nodes_list(node->right_child);
    }
    
    vector<HybridNode *> Treenodes(HybridNode * node){
        nodes.clear();
        nodes_list(node);
        return nodes;
    }

   HybridNode * search_tree(HybridNode * node,string key){
        if(node==NULL){
            return node;
        }
        if(node->key>key){
            return search_tree(node->left_child,key);
        }else if(node->key<key){
            return search_tree(node->right_child,key);
        }else{
            return node;
        }
    }

    HybridNode* search(string key) {
        if(root==NULL){
            return root;
        }
        return search_tree(root,key);
        // Implement Red-Black Tree search
    }
            
HybridNode* search_next(string key) {
      HybridNode *temp = root;
                while (temp != NULL) {
                    if (key < temp->key) {
                        if (temp->left_child == NULL){
                            break;
                        }
                        else{
                            temp = temp->left_child;
                        }
                    } else if (key == temp->key) {
                        break;
                    } 
                    else {
                        if (temp->right_child == NULL){
                            break;
                        }
                        else{
                            temp = temp->right_child;
                        }
                    }
                }
                return temp;
}

    
  
    int blackheight(HybridNode* node) {
        if (node == NULL) {
            // For null nodes, the black height is 0.
            return 0;
        }

        int leftBlackHeight = blackheight(node->left_child);
        int rightBlackHeight = blackheight(node->right_child);

        // Check if the left and right subtrees have the same black height and return it.
        if (leftBlackHeight != -1 && leftBlackHeight == rightBlackHeight) {
            // If the current node is black, add 1 to the black height.
            if (node->color == "black") {
                return leftBlackHeight + 1;
            } else {
                return leftBlackHeight;
            }
        }

    // If the left and right subtrees have different black heights, or if a red-red violation is detected,
    // return -1 to indicate an invalid Red-Black Tree.
        return -1;
    }

};




class IndexEntry {
private:
    string word;
    vector<pair<string, int>> chapter_word_counts;  // List of (chapter, word_count) tuples

public:
    IndexEntry(string word_val) : word(word_val) {}

    void setWord(string word_val) {  // Set the word
        word = word_val;
    }

    string getWord() const {  // Get the word
        return word;
    }

    void setChapterWordCounts(vector<pair<string, int>> chapter_word_counts_val) {  // Set the list of (chapter, word_count) tuples
        chapter_word_counts = chapter_word_counts_val;
    }

    vector<pair<string, int>> getChapterWordCounts() {  // Get the list of (chapter, word_count) tuples
        return chapter_word_counts;
    }
};



bool compareIndexEntries(const IndexEntry& entry1, const IndexEntry& entry2) {
            return entry1.getWord() < entry2.getWord() ; 
}

class Lexicon {
private:
    RedBlackTree red_black_tree;  // Red-Black Tree
    vector<string> names_of_chapters;
public:
    Lexicon() {
        //red_black_tree.setRoot(NULL);
        //cout<<"Inside constructor"<<endl;
    }

    void setRedBlackTree(RedBlackTree tree) {  // Set the Red-Black Tree
        red_black_tree = tree;
    }

    RedBlackTree getRedBlackTree() {  // Get the Red-Black Tree
        return red_black_tree;
    }

    void readChapters(vector<string> chapter_names) {  
        // Process words from a chapter and update the Red-Black Tree
        // chapter_names is a vector of chapter names

        names_of_chapters=chapter_names;
        for(int i=0;i<chapter_names.size();i++){
            cout<<"CHAPTER NAME : "<<chapter_names[i]<<endl;
            cout<<chapter_names[i]+".txt"<<endl;
            ifstream inputfile(chapter_names[i]+".txt");
            if(!inputfile.is_open()) {
                cerr << "Unable to open file" << endl;
                return;
            }
            string line;
           int extractedNumber=chapter_names[i][chapter_names[i].size()-1]-48 ;
            cout<<extractedNumber<<endl;

            while(true){
                //cout<<"Hi"<<endl;
                getline(inputfile, line);
                //cout<<line<<endl;
                istringstream iss(line);
                string word;
                while(iss >> word){
                    transform(word.begin(),word.end(),word.begin(),::tolower);      // converted into lowercase 
                    //cout << word << endl;
                    //red_black_tree.insert(word,chapter_names[i]);
                    //cout<<"done"<<endl;
                    //tree->insert(word,chapter_names[i]);
                    HybridNode * fetched=red_black_tree.search(word);
                    //cout<<fetched<<endl;
                   if(fetched==NULL){
                        //cout<<"1"<<endl;
                        red_black_tree.insert(word,chapter_names[i]);
                        fetched=red_black_tree.search(word);
                        fetched->arr[extractedNumber-1]+=1;
                        //cout<<red_black_tree.search(word)->color<<endl;
                    }else{
                        //cout<<"Updating count"<<endl;
                        fetched->arr[extractedNumber-1]+=1;        // updating count of a particular word from a particular chapter
                    }
                }
                if (inputfile.peek() == EOF){
                    cout<<"eof found"<<endl;
                    break;
                }
            }
        }

        //red_black_tree.printInOrder();
        cout<<"File Reading Done"<<endl;
        //tree_traversal(red_black_tree.getRoot(),chapter_names.size());        // 251 words
        cout<<"Now Pruning tree..."<<endl;
        Prune_tree(red_black_tree.getRoot(),chapter_names.size());           
        //red_black_tree.printInOrder();       // 234 words
}

    void tree_traversal(HybridNode * root,int chapters){
        if(root==NULL){
            return;
        }
        tree_traversal(root->left_child,chapters);
        tree_traversal(root->right_child,chapters);
        cout<<root->key<<endl;
        for(int i=0;i<chapters;i++){
            cout<<root->arr[i]<<' ';
        }
        cout<<endl;
        tree_traversal(root->left_child,chapters);
        tree_traversal(root->right_child,chapters);
    }

    void Prune_tree(HybridNode * root, int chapters){
        if(root==NULL){
            return;
        }
        Prune_tree(root->left_child,chapters);
        Prune_tree(root->right_child,chapters);
        //cout<<root->key<<endl;
        int * p=root->arr;
        int count=0;
        for(int i=0;i<chapters;i++){
            if(p[i]>=1){
                count++;
            }
        }
        //cout<<count<<endl;
        if(count==chapters){
            red_black_tree.deleteNode(root->key);
        }
    }

    

    vector<IndexEntry> buildIndex() {
        // Build the index using the remaining words in the Red-Black Tree
        vector<HybridNode *> all_nodes=red_black_tree.Treenodes(red_black_tree.getRoot());
        vector<IndexEntry> Indexes;
        Indexes.clear();
        IndexEntry * index1;
        //cout<<"Initial indexes : "<<Indexes.size()<<endl;
        //cout<<"Chapter Number "<<names_of_chapters.size()<<endl;
        vector<pair<string, int>> chapter_word_count;
        for(int i=0;i<all_nodes.size();i++){
            HybridNode * node=all_nodes[i];
            index1=new IndexEntry(node->key);
            int * array=node->arr;
            chapter_word_count.clear();
            for(int j=0;j<names_of_chapters.size();j++){
                chapter_word_count.push_back(std::make_pair(names_of_chapters[j],array[j]));
            }
            index1->setChapterWordCounts(chapter_word_count);
            Indexes.push_back(*index1);
        }
        

        std::sort(Indexes.begin(), Indexes.end(),compareIndexEntries); //, compareIndexEntries

        all_nodes.clear();
        return Indexes;
    }
};