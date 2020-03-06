#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include "emp-azkgc/emp-azkgc.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;


Bit ez(void* ctx){
    Number a=1;
    Number b=2;
	return a<b;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==PROVER ? nullptr : "127.0.0.1", port);

	setup_arithmetic_zk(io, party);


	if(!judge(io,party,NULL,ez)){
		error("ez failed");
		return 0;
	}
    

	puts("Yes");

	delete io;
}
