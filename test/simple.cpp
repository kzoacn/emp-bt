 
#include <emp-tool/emp-tool.h>
#include "emp-arith/emp-arith.h"
using namespace emp;
using namespace std;


int port, party;
NetIO * io;
 

 void myassert(bool f){
	 if(party!=ALICE)
	 	return;
	assert(f);
 }
int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_arithmetic(io, party);
	//ALICE = Gen
	//BOB = Eva


	int x,y;
	if(party==ALICE){
		x=1;
	}else{
		y=4;
	}

	Number a(BITLENGTH,x,ALICE);
	Number b(BITLENGTH,y,BOB);
	Number c=a+b;
	myassert(c.reveal()==5);



	Number s(BITLENGTH,9,ALICE);
	const int cx[4]={0,1,2,9};
	const int cy[4]={1,2,4,51};

	Number r=project(s,4,cx,cy);

	cout<< r.reveal() <<endl;


	puts("Yes");



	delete io;
}
