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
#include <algorithm>

#include "process.h"
#include "BSnode.h"

class Manager{
    
private:
    std::vector< std::vector<BSnode *> > list;
    
    BSnode* root;
    
    int levels;
    int nodeCount;
    
    void* main_block;
    
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
        
        for( int i = 0; i <= max_level; i++ ){
            list.push_back(std::vector<BSnode *>());
        }
        
        std::cout << list.size() << std::endl;
        //Add root to the list
        list[max_level].push_back(root);
        
        //Get the initial memory block
        main_block = malloc(size);
        if( main_block == NULL ){
            std::cerr << " Unable to get initial memory.";
            exit(1);
        }
    }
    
    BSnode* getRoot(){ return root; }
    
    void my_malloc(unsigned long long size, BSnode * node, Process * p){
        
        //If the level below can accomodate the process...
        if( node->getLevel() > 0 && pow( 2, node->getLevel() - 1 ) >= size ){
            
            //If both children are null, the segment must be split
            if( node->getLeft() == NULL && node->getRight() == NULL ){
                
                node->setAvailable(false);
                node->setIsSplit(true);
                node->setLeft(new BSnode(nodeCount++, node->getLevel() - 1, node));
                node->setRight(new BSnode(nodeCount++, node->getLevel() - 1, node));
//                list[node->getLevel()].pop_back();
                list[node->getLevel() - 1].push_back(node->getLeft());
                list[node->getLevel() - 1].push_back(node->getRight());

            }
            
            //If left child is not null, recurse left
            if( node->getLeft() != NULL && (node->getLeft()->getAvailable() || node->getLeft()->getIsSplit()) ){
                my_malloc(size, node->getLeft(), p);
            }
            
            //If right child is not null, recurse right
            else if( node->getRight() != NULL && ( node->getLeft()->getAvailable() || node->getRight()->getIsSplit() )){
                my_malloc(size, node->getRight(), p);
            }
            
        } else {
            //Make segment unavailable. Remove from list?
            node->setAvailable(false);
//            list[node->getLevel()].erase(std::remove(list[node->getLevel()].begin(), list[node->getLevel()].end(), node), list[node->getLevel()].end());
            node->setCurrentProcess(p);
            std::cout << "Offset is : " << findOffset(node) << std::endl;
            list[node->getLevel()].push_back(node);
            
        }
        
        
            
    }
    
    unsigned long long findOffset(BSnode * node){
        unsigned long long sum = 0;
        std::vector<BSnode*> inOrder;
        offsetUtil(root, &inOrder);
        
        for(int i = 0; i < inOrder.size(); i++){
            //If the node is a leaf
            if( inOrder[i]->getLeft() && inOrder[i]->getRight() ){
                sum += inOrder[i]->getSize();
            }
        }
        return sum;
    }
    
    std::vector<BSnode*>* offsetUtil(struct BSnode* node, std::vector<BSnode*>* inOrder)
    {
        if (node == NULL)
            return NULL;

        
        /* first recur on left child */
        offsetUtil(node->getLeft(), inOrder);
        
        /* add to vector */
        inOrder->push_back(node);
        
        /* now recur on right child */
        offsetUtil(node->getRight(), inOrder);
        
        return inOrder;
    }
    
    void my_free(){
        
    }
    
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
        std::cout<<"\n\n"<<std::endl;
        for(int i = 0; i < list.size(); i++){
            std::cout << "List[" << i << "] : ";
            for( int j = 0; j < list[i].size(); j++){
                std::cout << "[" << list[i][j]->getNodeID() << "]" << list[i][j]->getSize() << "(" << list[i][j]->getAvailable() << ") : ";
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
                 std::cout<<"                  ";}
            
            std::cout << Node->getNodeID() << " : " << Node->getSize() << " : " << Node->getAvailable();
            ShowTree(Node->getLeft(), l+1);
        }
    }
    
    ~Manager(){
        free(main_block);
    }
};



#endif /* BS_h */
