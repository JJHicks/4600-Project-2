/************************************************************************
 *   Authors: Justin Hicks and Chris Cox                                 *
 *    Course: CSCE 4600                                                  *
 *    Section: 001                                                       *
 *    Assignment:  Project 1                                             *
 *    Professor: Dr. Armin Mikler                                        *
 *    Date: March 19, 2019                                               *
 *    CSCE Machine Used: cse06.cse.unt.edu                               *
 *    Github Repository: https://github.com/JJHicks/4600Project.git      *
 *********************************************************************** */

#include <iostream>
#include <random>
#include <list>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

#include "process.h"
#include "BS.h"
#include "BSnode.h"

using namespace std;



//Function used to print the linked list containing processes
void show_list(list<Process*> processes){
    
    //range based for loop to print each process info
    for(auto p : processes)
    p->show_info();
    
    cout << endl;
}

//function that gets the process from a specific index of the list
Process* getProcessAtIndex(list<Process*> processes, int index){
    
    //initialize iterator to beginning of list
    //and integer holder for index
    list<Process*> :: iterator it = processes.begin();
    int i = 0;
    
    //iterate through the list until at the index requested
    while(i < index && it != processes.end()){
        it++;
        i++;
    }
    
    //if index was found, return it
    //else return null
    if(i == index){
        return *it;
    }
    else {
        return nullptr;
    }
}


//function that calculates the average cylce count of a given process list
unsigned long long averageCyclesList(list<Process *> processes){
    
    unsigned long long count = 0;
    
    //range based for loop to calculate total cylces of given list
    for(auto p: processes)
    count += p->getCycles();
    
    return count / processes.size();
}

//function that calculates the average memory usage of a given process list
unsigned long long averageMemoryList(list<Process *> processes){
    
    unsigned long long count = 0;
    
    //range based for loop to calculate total memory in given process list
    for(auto p : processes)
    count += p->getMemory();
    
    return count / processes.size();
}

//function that returns the total cycles for a process vector
//this is the same as the process list calculations
unsigned long long totalCyclesVector(vector<Process *> processes){
    unsigned long long count = 0;
    for( auto p : processes){
        count += p->getCycles();
    }
    return count;
}

//function that returns the total memory for a process vector
//this is the same as the process list calculations
unsigned long long totalMemoryVector(vector<Process *> processes){
    unsigned long long count = 0;
    for( auto p : processes){
        count += p->getMemory();
    }
    return count;
}

bool processesComplete(list<Process*> processes){
    for( auto p : processes ){
        if( !p->isFinished() )
            return false;
    }
    return true;
}

void resetProcesses(list<Process*> processes){
    for( auto p : processes ){
        p->setFinished(false);
        p->resetCyclesRemaining();
    }
}

//funciton that generates processes, to the amount specified in main()
void GenerateProcesses(list<Process*> &processes, int num_processes){
    
    //seed random generator
    random_device rd;
    mt19937_64 generator(rd());
    
    //distribution from 10 * 10^4 to 50 * 10^5
    uniform_int_distribution<long long unsigned> cycle_distribution(100000,5000000);
//    uniform_int_distribution<long long unsigned> cycle_distribution(10, 15);
    
    //distribution from ( 1,000 bytes to 160,000 bytes )
    uniform_int_distribution<long long unsigned> memory_distribution(1000,160000);
    unsigned long long num_cycles, mem_footprint;
    
    //For the numbers of processes to be created
    for (int i=0; i < num_processes ; ++i) {
        
        //get cycle count and memory foot print
        num_cycles = cycle_distribution(generator);
        mem_footprint = memory_distribution(generator);
        
        //create the process and place it in process list
        Process* next_process = new Process(i, num_cycles, mem_footprint);
        processes.push_back(next_process);
    }
}

double systemManager(list<Process*> processes){
    struct timeval  tv1, tv2;
    //Start time, move this around to skip array creation or loading if desired
    gettimeofday(&tv1, NULL);
    
    int time = 0;
    int next_process = 0;
    list<Process*> running;
    
    while(!processesComplete(processes)){
        
        //New process comes
        if( time % 50 == 0 && next_process < 50 ){
            std::cout << "Process " << getProcessAtIndex(processes, next_process)->getPID() << " has arrived." << std::endl;
            //Insert the next process in to the running list
            running.push_back(getProcessAtIndex(processes, next_process));
            
            //Allocate the memory for the new process
            running.back()->assignMemPtr( malloc(running.back()->getMemory()) );
            
            next_process++;
        }
        
//        std::cout << running.size() << " processes running." << std::endl;
        
        //Decriment the remaining time of all processes
        for( auto p : running ){
            p->cycle();
            //Remove finished processes from the "running" list
            if( p->isFinished() ){
                std::cout << p->getPID() << " has finished." << std::endl;

                //Free the memory of the finished process
                free( p->getMemPtr() );
            }
        }
        
        running.remove_if([](Process* p){ return p->isFinished(); });
        
        time++;
    }
    
    //Stop time
    gettimeofday(&tv2, NULL);
    
    //Calculate elapsed time
    std::cout << "Total time = " <<((double) (tv2.tv_usec - tv1.tv_usec)) / 1000000 +
    ((double) (tv2.tv_sec - tv1.tv_sec)) << " seconds " << std::endl;
    
    return ((double) (tv2.tv_usec - tv1.tv_usec)) / 1000000 + ((double) (tv2.tv_sec - tv1.tv_sec));
}

double myManager(list<Process*> processes){
    struct timeval  tv1, tv2;
    //Start time, move this around to skip array creation or loading if desired
    gettimeofday(&tv1, NULL);
    
    int time = 0;
    int next_process = 0;
    list<Process*> running;
    
    Manager myManager = Manager(10000000);
    
    while(!processesComplete(processes)){
        
        //New process comes
        if( time % 50 == 0 && next_process < 50 ){
            std::cout << "Process " << getProcessAtIndex(processes, next_process)->getPID() << " has arrived." << std::endl;
            //Insert the next process in to the running list
            running.push_back(getProcessAtIndex(processes, next_process));
            
            //Allocate the memory for the new process
            myManager.my_malloc(running.back());
            
            next_process++;
        }
        
        //        std::cout << running.size() << " processes running." << std::endl;
        
        //Decriment the remaining time of all processes
        for( auto p : running ){
            p->cycle();
            //Remove finished processes from the "running" list
            if( p->isFinished() ){
                std::cout << p->getPID() << " has finished." << std::endl;
//                myManager.ShowTree(myManager.getRoot(), 0);
                //Free the memory of the finished process
                myManager.my_free( p );
            }
        }
        
        running.remove_if([](Process* p){ return p->isFinished(); });
        
        time++;

        
    }
    
    //Stop time
    gettimeofday(&tv2, NULL);
    
    //Calculate elapsed time
    std::cout << "Total time = " <<((double) (tv2.tv_usec - tv1.tv_usec)) / 1000000 +
    ((double) (tv2.tv_sec - tv1.tv_sec)) << " seconds " << std::endl;
    
    myManager.ShowTree(myManager.getRoot(), 0);
    myManager.printList();
    
    return ((double) (tv2.tv_usec - tv1.tv_usec)) / 1000000 + ((double) (tv2.tv_sec - tv1.tv_sec));
}

int main(int argc, const char * argv[]) {
    
    //Container object for initial pool of processes
    list<Process*> processes;
    
    //call the function to generate a set of X amount of processes
    GenerateProcesses(processes, 50);
    
    double time1 = systemManager(processes);
    
    resetProcesses(processes);
    
    double time2 = myManager(processes);
    
    cout << "Time difference : " << time1 - time2 << endl ;
    
//    Manager myManager = Manager(10000000);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    Process* sample_process1 = new Process(52, 50, pow( 2, 20 ));
//    myManager.my_malloc(sample_process1->getMemory(), myManager.getRoot(), sample_process1);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    Process* sample_process2 = new Process(53, 50, pow( 2, 19 ) + 10000);
//    myManager.my_malloc(sample_process2->getMemory(), myManager.getRoot(), sample_process2);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    Process* sample_process3 = new Process(54, 50, pow( 2, 21 ) + 100);
//    myManager.my_malloc(sample_process3->getMemory(), myManager.getRoot(), sample_process3);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    Process* sample_process4 = new Process(55, 50, pow( 2, 18 ) );
//    myManager.my_malloc(sample_process4->getMemory(), myManager.getRoot(), sample_process4);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    Process* sample_process5 = new Process(56, 50, pow( 2, 22 ) );
//    myManager.my_malloc(sample_process5->getMemory(), myManager.getRoot(), sample_process5);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
//
//    myManager.my_free(sample_process4);
//
//    myManager.ShowTree(myManager.getRoot(), 0);
//    myManager.printList();
    
    return 0;
}
