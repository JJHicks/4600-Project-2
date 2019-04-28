//
//  BSnode.h
//  Project2
//
//  Created by Justin Hicks on 4/28/19.
//  Copyright © 2019 Justin Hicks. All rights reserved.
//

#ifndef BSnode_h
#define BSnode_h

class BSnode{
private:
        int nodeID;
        int level;
    
        bool available;
    
        BSnode * left;
        BSnode * right;
public:
    BSnode(int ID, int l){
        nodeID = ID;
        level = l;
        available = true;
        left = right = NULL;
    }
    
    int getNodeID(){ return nodeID; }
    int getLevel(){ return level; }
    bool getAvailable(){ return available; }
    BSnode * getRight(){ return right; }
    BSnode * getLeft(){ return left; }
    
    void setRight(BSnode * r){ right = r; }
    void setLeft(BSnode * l){ left = l; }
    
    void toggleAvailable(){
        available = !available;
    }
    
    ~BSnode(){
        
    }
};

#endif /* BSnode_h */
