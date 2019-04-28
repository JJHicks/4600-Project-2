//
//  Header.h
//  Project2
//
//  Created by Justin Hicks on 4/28/19.
//  Copyright © 2019 Justin Hicks. All rights reserved.
//



#ifndef process_h
#define process_h

#include <iostream>

//Process simulation class
class Process{
private:
    
    //variable declarations
    int PID;
    unsigned long long num_cycles;
    unsigned long long mem_footprint;
    
    unsigned long long cycles_remaining;
    bool finished;
    
    void* memPtr;
    
public:
    //Constructor
    Process(int p, unsigned long long c, unsigned long long f){
        PID = p;
        num_cycles = c;
        cycles_remaining = c;
        mem_footprint = f;
        finished = false;
        
    }
    //Copy constructor
    Process(const Process &p2){
        PID = p2.PID;
        num_cycles = p2.num_cycles;
        mem_footprint= p2.mem_footprint;
        cycles_remaining = p2.cycles_remaining;
        finished = p2.finished;
    }
    
    //functions to return private variables
    unsigned long long getCycles(){ return this->num_cycles; }
    unsigned long long getMemory(){ return this->mem_footprint; }
    unsigned long long getCyclesRemaining(){ return this->cycles_remaining; }
    
    void assignMemPtr(void* p){
        this->memPtr = p;
    }
    
    void* getMemPtr(){
        return this->memPtr;
    }
    
    //Object print method
    void show_info(){
        std::cout<<"\t\tProcess : "<<this->PID<<"\n\t\t\tCycles : "<<this->num_cycles<<"\n\t\t\tMemory : "<<this->mem_footprint<<"\n";
    }
    
    int getPID(){
        return this->PID;
    }
    
    void cycle(){
        if( this->cycles_remaining > 0 )
            this->cycles_remaining -= 1;
    
        if( this->cycles_remaining == 0 && !this->finished )
            this->finished = true;
    }
    
    bool isFinished(){
        return this->finished;
    }
    
    //Destructor
    ~Process(){}
    
};

#endif /* Header_h */
