//
//  BSnode.h
//  Project2
//
//  Created by Justin Hicks on 4/28/19.
//  Copyright © 2019 Justin Hicks. All rights reserved.
//

#ifndef BSnode_h
#define BSnode_h

#include "process.h"

class BSnode{
private:
        int nodeID;
        int level;
        unsigned long long size;
    
        bool available;
    
        BSnode * left;
        BSnode * right;
        BSnode * parent;
    
        Process* currentProcess;
    
public:
    BSnode(int ID, int l, BSnode* p){
        nodeID = ID;
        level = l;
        
        size = pow( 2, level );
        
        available = true;
        left = right = NULL;
        parent = p;
        
        currentProcess = NULL;
        
    }
    
    int getNodeID(){ return nodeID; }
    int getLevel(){ return level; }
    bool getAvailable(){ return available; }
    unsigned long long getSize(){ return size; }
    Process* getCurrentProcess(){ return currentProcess; }
    
    BSnode * getRight(){ return right; }
    BSnode * getLeft(){ return left; }
    BSnode * getParent() { return parent; }
    
    void setRight(BSnode * r){ right = r; }
    void setLeft(BSnode * l){ left = l; }
    
    void setCurrentProcess(Process* p){ currentProcess = p; }
    void setAvailable(bool a){ available = a; }
    
    ~BSnode(){
        
    }
};

#endif /* BSnode_h */
