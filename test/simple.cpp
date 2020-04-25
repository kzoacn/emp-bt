#include <iostream>

//#include <emp-tool/circuits/circuit_file.h>
#include "emp-bt/cir.h"
#include "emp-bt/wire.h"
#include "emp-bt/num.h"
using namespace std;
using namespace emp;
 
 
 
int main(int argc, char** argv) {
	int party,port;
    parse_party_and_port(argv, &party, &port);
	NetIO *io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port); 

    setup(io,party);
    const int BITLENGTH=32;
    Number a(BITLENGTH,4,PUBLIC);
    Number b(BITLENGTH,12,BOB);
    

    cout<< a.output() <<endl;   

}
    