#ifndef TREE234_H
#define	TREE234_H
/*
 * Based on http://www.unf.edu/~broggio/cop3540/Chapter%2010%20-%202-3-4%20Trees%20-%20Part%201.ppt
 * See also: http://grail.cba.csuohio.edu/~lin/cis506/Chapt10.pdf
 */
#include <utility>
#include <iostream>
#include <exception>

// fwd declarations
template<typename T> class Tree234;    
template<typename K> class Node234; 

class duplicatekey :  public std::exception {
public:
    
  virtual const char* what() const throw()
  {
    return "Attempting to insert duplicate key ";
  }
};

template<typename K> class Node234 {
   private: 
       friend class Tree234<K>;             
       static int MAX;   
       Node234(K small);
       Node234(K small, K larger);
       Node234(K small, K middle, K large);

       Node234<K> *parent;
       
       int totalItems; /* If totalItems is 1, then two node; if 2, then three node, if 3, then four node. */   
       K keys[3];
       Node234<K> *children[4];

       bool find(K key, int& index);
       int insertItem(K key);
       bool isFull();
       Node234<K> *getParent();
       bool isLeaf(); 
       void connectChild(int childNum, Node234<K> *child);
};  

template<typename K> int Node234<K>::MAX = 3;

template<typename K> inline Node234<K>::Node234(K small) : totalItems(1)
{ 
   keys[0] = small; 

   for (int i = 0; i < MAX; i++) {		
       children[i] = 0;
    }
}

template<typename K> inline Node234<K>::Node234(K small, K middle) : totalItems(2)
{ 
   keys[0] = small; 
   keys[1] = middle; 

   for (int i = 0; i < MAX; i++) {		
       children[i] = 0;
    }
}
template<typename K> inline Node234<K>::Node234(K small, K middle, K large) : totalItems(3)
{ 
   keys[0] = small; 
   keys[1] = middle; 
   keys[3] = large; 
    
   for (int i = 0; i < MAX; i++) {		
       children[i] = 0;
    }
}
/*
 * precondition: childNum is with the range for the type of node.
 * child is not 0.
 */
template<typename K> inline void Node234<K>::connectChild(int childNum, Node234<K> *child)
{
  children[childNum] = child;
  child->parent = this;
}
/*
 * preconditions: node is not full, not a four node (full), and key is not already in node. It may or may not be a leaf.
 * shifts keys in node as needed so that key will be inserted in sorted position
 */
template<typename K> inline int Node234<K>::insertItem(K key)
{ 
  // start on right, examine items

  for(int i = totalItems - 1; i >= 0 ; i--) {

/* java code had a check for 'null' evidently bc of the way Node234<K>::removeItem() works
        if (values[i] == null) {

            continue;
        } else if (key < keys[i]) { // if it's bigger  
*/
        if (key < keys[i]) { // if it's bigger

            keys[i + 1] = keys[i]; // shift keys[i] right

        } else {

            keys[i + 1] = key; // insert new item

            return i + 1; // return index to inserted key.
        } 
    } 

    // shifted all items, insert new item at position 0

    keys[0] = key;  
  ++totalItems; // increase the total item count
    return 0;

  /*
   * TODO: shift pointers appropriately? This seems to be handled separately by the calling code.
   * A: Also, I believe we are inserting into a leaf, so that is not needed?
   */
}

template<typename K> inline  bool Node234<K>::isFull()  
{ 
   return totalItems == MAX;
}

template<typename K> inline  Node234<K> *Node234<K>::getParent()  
{ 
   return parent;
}
template<typename K> inline  bool Node234<K>::isLeaf()  
{ 
   return !children[0] ? true : false;
}

template<typename K> inline bool Node234<K>::find(K key, int& index)
{ 
   for(int i =0; i < totalItems; i++) {
   
       if (keys[i] == key) {

            index = i;
            return true;
       }
   }
   return false;
}

template<typename K> class Tree234 {
          
  protected:

    Node234<K> *root; 
    
    bool DoSearch(K key, Node234<K> *&location, int& index);
    template<typename Functor> void DoTraverse(Functor f, Node234<K> *root);
    Node234<K> *getNextChild(Node234<K> *current, K key);
    void split(Node234<K> *node);

  public:    
     Tree234() { root = 0; } 
     bool search(K key);
     bool remove(K key, Node234<K> *location=0);
     template<typename Functor> void traverse(Functor f);
     void insert(K key); // throw(duplicatekey) 
};
	

template<typename K> bool Tree234<K>::search(K key)
{
    // make sure tree has at least one element    
    if (root == 0) {
    
       return false;
        
    } else {
        int index;  
        Node234<K> *location;
        return DoSearch(key, location, index);
    }
}   
/*
 * Precondition: current is initially root.
 * Descends tree getting next child until key found or leaf encountered.
 * If key is found, additionally returns node and index within node
 */
template<typename K>  bool Tree234<K>::DoSearch(K key, Node234<K> *&location, int& index)
{
  Node234<K> *current = root;

  if (root == 0) {

     return false;
  }

  while(true) {
 
      if (current->find(key, index)) {

          location = current;
          return true; 

      } else if (current->isLeaf()) {

          return false;

      } else {

          current = getNextChild(current, key);	
      }  
    }
}
/* 
 * Precondition: assumes node is not empty, not full, not a leaf
 * Returns next child (there could be up to three children)
 */
template<typename K> inline Node234<K> *Tree234<K>::getNextChild(Node234<K> *current, K key)
{
  int i = 0;
  
  for(; i < current->totalItems; i++) {        

     // Are we less?
     if (key < current->keys[i]) {

           return current->children[i];  
     }
  }

  // we're greater, so return right-most child
  return current->children[i];   
}

template<typename K> template<typename Functor> inline void Tree234<K>::traverse(Functor f)
{     
  DoTraverse(f, root);    
}

template<typename K> template<typename Functor> void Tree234<K>::DoTraverse(Functor f, Node234<K> *current)
{     
   if (current == 0) {

	return;
   }

   switch (current->totalItems) {

      case 1: // two node
            DoTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoTraverse(f, current->children[1]);
            break;

      case 2: // three node
            DoTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoTraverse(f, current->children[1]);
 
            f(current->keys[1]);

            DoTraverse(f, current->children[2]);
            break;

      case 3: // four node
            DoTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoTraverse(f, current->children[1]);
 
            f(current->keys[1]);

            DoTraverse(f, current->children[2]);

            f(current->keys[2]);

            DoTraverse(f, current->children[3]);
 
            break;
   }
}

template<typename K> void Tree234<K>::insert(K key)
{
Node234<K> *current = root;

   /* Descend until a leaf node is found, splitting four nodes as they are encountered */

    while(true) {

        if( current->isFull() )  {// if four node, split it, moving a value up to parent.

            split(current); 
      
            // resume search with parent.
            current = current->getParent(); 

            current = getNextChild(current, key);

        } else if(current->isLeaf()) {

                /* done descending. */
	        break; 
	} else { 

            /* node is internal but not full, so descend, getting next in-order child. */ 
                              
            current = getNextChild(current, key);
        }
    } 
    while(true) {

        if( current->isLeaf() )  {

            /* done descending. */
            break;

        } else if(current->isFull()) {// if four node, split it, moving a value up to parent.

            split(current); 
      
            // resume search with parent.
            current = current->getParent(); 

            current = getNextChild(current, key);

	} else { 

            /* node is internal but not full, so descend, getting next in-order child. */ 
                              
            current = getNextChild(current, key);
        }

    // current is now a leaf and not full.
    current->insertItem(key); 
} 
/* split(Node234<K> *nod)
 * Preconditions: node is full, a four node.
 *
 * Pseudo code
 * The four node is either: 
 * 1. the root
 * 2. has a two node parent
 * 3. has a three node parent
 */ 
template<typename K> void Tree234<K>::split(Node234<K> *node)
{
    K  itemB, itemC;

    Node234<K> *parent, *child2, *child3;

    int itemIndex;

    // remove two largest (of three total) keys...
        
    itemC = node->keys[2];
    itemB = node->keys[1]; 
    node->totalItems = 1; // ...by setting totalItems to 1. 

    // Remove two right-most children from this node. 
    child2 = node->children[2]; 
    child3 = node->children[3]; 

    Node234<K> *newRight = new Node234<K>(itemC); // make new right child node from largest item

    /* set its left and right children to be the two right-most children of node */

    newRight->connectChild(0, child2); // connect to 0 and 1

    newRight->connectChild(1, child3); // on newRight

    /* we will covert node into a two node whose left and right children will be the two left most children
       This occurs by default. We only need adjust totalItems  */
    node->children[2] = 0; 
    node->children[3] = 0; 


    // if this is the root,
    if(node == root) { 
        /* make new root two node using node's middle value */  
        root = new Node234<K>(itemB); 
        parent = root;          // root is parent of node
        root->connectChild(0, node); // connect node to root as left child
        root->connectChild(1, newRight);
        return;

    }         

    parent = node->getParent(); 
    bool bParentWas2Node = parent->totalItems == 1;

    // deal with parent, moving itemB middle value to parent.

    int insert_index = parent->insertItem(itemB);
    int last_index = parent->totalItems - 1;
    
    for(int i = last_index; i > insert_index; i--)  {// move parent's connections right, start from new last index up to insert_index

        Node234<K> *temp = parent->children[i];  // one child
        parent->connectChild(i + 1, temp);       // to the right
    }

    parent->connectChild(insert_index + 1,  newRight);
    /* By default, we do not need to insert node. It will be at the correct position. So we do not need to do:
    parent->connectChild(insert_index, node); 
    */
  
    return;
}
/*
 * The delete alogithm is depicted at: http://www.cs.mtsu.edu/~jhankins/files/3110/presentations/2-3Trees.ppt 
 * The startegy is to turn two nodes on the way down into three or four nodes. These two sites give pseudo code:
 * http://www.usna.edu/Users/cs/taylor/courses/ic312/class/class23.shtml
 * http://www2.thu.edu.tw/~emtools/Adv.%20Data%20Structure/2-3,2-3-4%26red-blackTree_952.pdf  <-- very good
 * http://penguin.ewu.edu/cscd320/Topic/B-Tree/2_3_4_Operations.html
 */
template<typename K> bool Tree234<K>::remove(K key, Node234<K> *location=0)
{
  return true;
}
#endif
