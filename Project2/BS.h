//
//  BS.h
//  Project2
//
//  Created by Justin Hicks on 4/28/19.
//  Copyright © 2019 Justin Hicks. All rights reserved.
//

#ifndef BS_h
#define BS_h

#include <iostream>
#include <vector>
#include <cmath>

#include "process.h"
#include "BSnode.h"

class Manager{
    
private:
    std::vector< std::vector<BSnode *> > list;
    
    BSnode* root;
    
    int levels;
    int nodeCount;
    
public:
    
    Manager(int size){
        int max_level = 0;
        while( pow(2, max_level) < size ){
            max_level++;
        }
        max_level--;
        
        std::cout << "Max level for Manager: " << max_level << std::endl;
        levels = max_level;
        nodeCount = 0;
        
        root = new BSnode(nodeCount++, max_level, NULL);
        
        for( int i = 0; i < max_level; i++ ){
            list.push_back(std::vector<BSnode *>());
        }
        
        std::cout << list.size() << std::endl;
        //Add root to the list
        list[max_level-1].push_back(root);
    
    }
    
    BSnode* getRoot(){ return root; }
    
    BSnode* search(BSnode* node, Process* p){
        if (node == NULL)
            return NULL;
        
        if ( node->getCurrentProcess() == p ){
            return node;
        }
        
        /* then recur on left subtree */
        search(node->getLeft(), p);
        
        /* now recur on right subtree */
        search(node->getRight(), p);
        
        return NULL;
    }
    
    void printList(){
        for(int i = 0; i < list.size(); i++){
            std::cout << "List[" << i << "] : ";
            for( int j = 0; j < list[i].size(); j++){
                std::cout << list[i][j]->getSize() << " : ";
            }
            std::cout<<std::endl;
        }
    }
    
    void ShowTree(BSnode* Node, int l){
        int i;
        if(Node != NULL){
            ShowTree(Node->getRight(), l+1);
            std::cout<<std::endl;
            if(Node == root)
            std::cout<<"Root: ";
            else{
                for(i = 0;i < l;i++)
                std::cout<<"      ";}
            
            std::cout << Node->getNodeID() << " : " << Node->getSize() << " : " << Node->getAvailable();
            ShowTree(Node->getLeft(), l+1);
        }
    }
    
    ~Manager(){
        
    }
};



#endif /* BS_h */
