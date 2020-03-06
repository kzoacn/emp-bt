 
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


int max_x[40];
int max_y[40];
Number t_max0(Number a){
	return project(a,40,max_x,max_y);
}

Number t_max(Number a,Number b){
	return a+t_max0(b-a);
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_arithmetic(io, party);
	//ALICE = Gen
	//BOB = Eva
	for(int i=0;i<40;i++){
		max_x[i]=i%2==0?i/2:-((i+1)/2);
		max_y[i]=max(0,max_x[i]);

	//	cout<<max_x[i]<<":"<<max_y[i]<<endl;
	}
 


	Number a=Number(BITLENGTH,2,ALICE);
	Number b=Number(BITLENGTH,1,ALICE);


	cout<<a.reveal()<<endl;

	int cx[5]={-2,-1,0,1,2};
	int cy[5]={0,0,0,1,2};

	int r=project(a,5,cx,cy).reveal();

	if(party==ALICE)
		cout<< r <<endl;
	

	for(int i=-5;i<5;i++)
	for(int j=-5;j<5;j++){
		Number a(BITLENGTH,i,ALICE);
		Number b(BITLENGTH,j,BOB);
		Number c=t_max(a,b);
		int r=c.reveal();
		if(party==ALICE)
			cout<< i << " " << j <<" " << r <<endl;

	}


	puts("Yes");



	delete io;
}
