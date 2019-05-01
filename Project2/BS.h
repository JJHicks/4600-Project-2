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
#include <queue>

#include "process.h"
#include "BSnode.h"

class Manager{
    
private:
    std::vector< std::vector<BSnode *> > list;
    
    BSnode* root;
    
    int levels;
    int nodeCount;
    
    std::queue<Process *> readyQueue;
    
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
        
//        std::cout << list.size() << std::endl;
        
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
    
    void my_malloc(Process* p){
        void* memPtr = NULL;
        getMem(p->getMemory(), root, p, memPtr);
        
        std::cout << "my_malloc memPtr : " << memPtr << std::endl;
        
        if(memPtr == NULL){
            std::cout << "Not enough space for process " << p->getPID() << ". Adding to readyQueue." << std::endl;
            readyQueue.push(p);
        } else {
            p->assignMemPtr(memPtr);
            p->setIsRunning(true);
        }
        ShowTree(root, 0);
        printList();
    }
    
    void getMem(unsigned long long size, BSnode * node, Process * p, void* &memptr){
        
        //If p has been assigned, stop looking
        if( searchList(p->getPID()) ){
            return;
        }
        
        //If the level below can accomodate the process...
        if( node->getLevel() > 0 && pow( 2, node->getLevel() - 1 ) >= size ){

            //If both children are null the segment is whole, and the segment must be split
            if( node->getLeft() == NULL && node->getRight() == NULL ){

                node->setAvailable(false);
                node->setIsSplit(true);
                node->setLeft(new BSnode(nodeCount++, node->getLevel() - 1, node));
                node->setRight(new BSnode(nodeCount++, node->getLevel() - 1, node));
//                list[node->getLevel()].pop_back();
                list[node->getLevel()].erase(std::remove(list[node->getLevel()].begin(), list[node->getLevel()].end(), node), list[node->getLevel()].end());
                list[node->getLevel() - 1].push_back(node->getLeft());
                list[node->getLevel() - 1].push_back(node->getRight());

            }

            //If left child is not null, recurse left
            if( node->getLeft() != NULL && (node->getLeft()->getAvailable() || node->getLeft()->getIsSplit()) ){
                getMem(size, node->getLeft(), p, memptr);
            }

            //If right child is not null, recurse right
            if( node->getRight() != NULL && ( node->getRight()->getAvailable() || node->getRight()->getIsSplit() )){
                getMem(size, node->getRight(), p, memptr);
            }
            
//            memptr = NULL;
//            return;

        } else {
            
            if( !node->getAvailable() )
                return;
            
            //Make segment unavailable. Remove from list?
            node->setAvailable(false);
//            list[node->getLevel()].erase(std::remove(list[node->getLevel()].begin(), list[node->getLevel()].end(), node), list[node->getLevel()].end());
            node->setCurrentProcess(p);
            unsigned long long offset = findOffset(node);
            std::cout << "Offset is : " << offset << std::endl;
            memptr = (char*)main_block + offset;
            std::cout << "main : " << main_block << "\tmemptr : " << memptr << " \twhat null is : " << NULL << std::endl;
            
            return;
//            list[node->getLevel()].push_back(node);
        }
//        memptr = NULL;
//        return;
    }
    
     void my_free(Process* p){
//        BSnode* node = search(root, p);
         BSnode* node = searchList(p->getPID());
        
        if(node != NULL){
            node->setAvailable(true);
            cleanup(node);
            
            if( readyQueue.size() > 0 ){
                if( readyQueue.front()->getMemory() <= findLargest()->getSize() ){
                    this->my_malloc( readyQueue.front() );
                    std::cout << "Removing " << readyQueue.front()->getPID() << " from the readyQueue and starting." << std::endl;
                    readyQueue.pop();
                }
            }
            
            
        } else {
            std::cout << "Process not found in tree." << std::endl;
        }
    }
    
    void cleanup(BSnode * node){

        if( node->getLeft() != NULL && node->getRight() != NULL ){
            if( node->getLeft()->getAvailable() && node->getRight()->getAvailable() ){
                
                //Remove the nodes children from the list
                list[node->getLevel()-1].erase(std::remove(list[node->getLevel()-1].begin(), list[node->getLevel()-1].end(), node->getLeft()), list[node->getLevel()-1].end());
                list[node->getLevel()-1].erase(std::remove(list[node->getLevel()-1].begin(), list[node->getLevel()-1].end(), node->getRight()), list[node->getLevel()-1].end());
                
                node->setLeft(NULL);
                node->setRight(NULL);
                
                //Set as not split and available
                node->setIsSplit(false);
                node->setAvailable(true);
            }
        }
        
        if(node->getParent() != NULL){
            if( node->getParent()->getLeft()->getAvailable() && node->getParent()->getRight()->getAvailable() ){

                //Add the parent to the list
                list[node->getLevel() + 1].push_back(node->getParent());
                delete node->getLeft();
                delete node->getRight();
                
                cleanup(node->getParent());
            }
        }
        
    }
    
    unsigned long long findOffset(BSnode * node){
        unsigned long long sum = 0;
        std::vector<BSnode*> inOrder;
        offsetUtil(root, &inOrder);
        
//        std::cout << "inOrder: ";
//        for( auto node : inOrder ){
//            std::cout << node->getNodeID() << ":" <<node->getSize() << " ";
//        }
//        std::cout << std::endl;
        
        for(int i = 0; i < inOrder.size(); i++){
            
            //only do sum until node is reached
            if( inOrder[i] == node ){
                break;
            }
            
            //If the node is a leaf, add to sum
            if( inOrder[i]->getLeft() != NULL && inOrder[i]->getRight() != NULL ){
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
    
    BSnode * findLargest(){
        for(int i = (int)list.size() - 1; i >= 0; i--){
            for( int j = 0; j < list[i].size(); j++){
                if( list[i][j]->getAvailable() ){
                    return list[i][j];
                }
            }
        }
        return NULL;
    }

    BSnode * searchList(int ID){
        for(int i = 0; i < list.size(); i++){
            for( int j = 0; j < list[i].size(); j++){
                if(list[i][j]->getCurrentProcess() != NULL){
                    if( list[i][j]->getCurrentProcess()->getPID() == ID ){
                        return list[i][j];
                    }
                }
            }
        }
        return NULL;
    }
    
    
    void printList(){
        std::cout<<"\n\n"<<std::endl;
        for(int i = 0; i < list.size(); i++){
            std::cout << "List[" << i << "] : ";
            for( int j = 0; j < list[i].size(); j++){
                std::cout << "[" << list[i][j]->getNodeID() << "]";
                if(list[i][j]->getCurrentProcess() != NULL){
                    std::cout << list[i][j]->getCurrentProcess()->getPID();
                } else {
                    std::cout << "NONE";
                }
                std::cout << "(" << list[i][j]->getAvailable() << "," << list[i][j]->getIsSplit() << ") : ";
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
            
            std::cout << Node->getNodeID() << ":" << Node->getSize() << "[";
            if(Node->getCurrentProcess() != NULL){
                std::cout << Node->getCurrentProcess()->getPID();
            } else {
                std::cout << "NONE";
            }
            std::cout << "]:(" << Node->getAvailable() << "," << Node->getIsSplit() << ")";
            ShowTree(Node->getLeft(), l+1);
        }
    }
    
    ~Manager(){
        free(main_block);
    }
};



#endif /* BS_h */
