#include <iostream>
#include <thread>
//#include <emp-tool/circuits/circuit_file.h>
#include "emp-bt/cir.h"
#include "emp-bt/wire.h"
#include "emp-bt/num.h"
using namespace std;
using namespace emp;
 
 
const int THREAD_NUM=32;

NetIO *io[THREAD_NUM];
thread t[THREAD_NUM];

const int maxn=1e4;
Number a[maxn],b[maxn],c[maxn];

void run(int id,NetIO *io,int party){

    emp::Circuit_Gen::circ_exec = new emp::Circuit_Gen(io,party); 
    for(int i=id;i<maxn;i+=THREAD_NUM){
        
        a[i]=Number(32,i,ALICE);
        b[i]=Number(32,i,BOB); 
        c[i]=a[i]+b[i];
        c[i]=a[i]+b[i];
        c[i]=a[i]+b[i];
        c[i]=a[i]+b[i];
        c[i]=a[i]+b[i];
        c[i]=a[i]+b[i];
    }
}


int main(int argc, char** argv) {
	int party,port;
    parse_party_and_port(argv, &party, &port);
	//NetIO *io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port); 

    //setup(io,party);
    emp::Circuit_Gen::circ_exec = new emp::Circuit_Gen(new NetIO(party==ALICE ? nullptr : "127.0.0.1", port-1),party); 
    for(int i=0;i<THREAD_NUM;i++){
        io[i]=new NetIO(party==ALICE ? nullptr : "127.0.0.1", port+i);
        io[i]->set_nodelay();
        t[i]=thread(run,i,io[i],party);
    }

    for(int i=0;i<THREAD_NUM;i++){
        t[i].join();
        io[i]->flush();    
    }
    cout<<party<<endl;
    
    cout<< c[5].output() <<endl;   

}
    