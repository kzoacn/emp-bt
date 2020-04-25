#include <iostream>

//#include <emp-tool/circuits/circuit_file.h>
#include "emp-bt/cir.h"
#include "emp-bt/wire.h"
#include "emp-bt/num.h"
using namespace std;
using namespace emp;
 
 
 

const int seed=255;
int n=100,m=100;

const char mp[]="ATCG";
void plain(void *ctx) {
    vector<vector<int> >f;
    vector<int>g0;
    vector<int>g1;

    vector<int>pa;
    vector<int>pb; 
 
    srand(seed);
	pa.resize(n+1);
	for(int i=0;i<n;i++){
        pa[i+1]=rand()%4; 
	}  
	
    pb.resize(m+1);
	for(int i=0;i<m;i++){
        pb[i+1]=rand()%4; 
	}  


	f.resize(2);
	g0.resize(m+1);
	g1.resize(m+1);

	f[0].resize(m+1);
	f[1].resize(m+1);

 

	for(int i=0;i<=m;i++)
		f[0][i]=-i;
	for(int i=1;i<=n;i++){
		int cur=i&1;
		int pre=cur^1;
		f[cur][0]=-i;
		for(int j=1;j<=m;j++){
			f[cur][j]=max(f[cur][j-1]-2,f[pre][j]-2);
			
			g0[j]= f[pre][j] <= f[cur][j-1] ? 1 : 0;
			g1[j]= f[cur][j] <= (f[pre][j-1]+(pa[i]==pb[j]?5:-1)) ? 1 : 0;

			f[cur][j]=max(f[cur][j],f[pre][j-1]+(pa[i]==pb[j]?5:-1));
		} 
	}
 
	std::cerr<<"ans is "<<f[n&1][m]<<std::endl;
}





void code(void *ctx) {

    vector<vector<Number> >dp;
    vector<Wire>arg0;
    vector<Wire>arg1;
    vector<Number>a,b;

    vector<int>pa;
    vector<int>pb; 
  
    srand(seed);
	pa.resize(n+1);
	for(int i=0;i<n;i++){
        pa[i+1]=rand()%4;
	} 
	
    pb.resize(m+1);
	for(int i=0;i<m;i++){
        pb[i+1]=rand()%4;
	} 
    const int BITLENGTH=32;
    a.resize(n+1);
	b.resize(m+1); 

    for(int i=1;i<=n;i++){ 
        a[i]=Number(BITLENGTH,pa[i],ALICE); 
    }
    for(int i=1;i<=m;i++){  
        b[i]=Number(BITLENGTH,pb[i],BOB);
    } 
 
	Number two(BITLENGTH,2,PUBLIC);
	Number five(BITLENGTH,5,PUBLIC);
	Number mone(BITLENGTH,-1,PUBLIC);
 
 
	dp.resize(2);
	arg0.resize(m+1);
	arg1.resize(m+1);
	dp[0].resize(m+1);
	dp[1].resize(m+1); 
	
	for(int i=0;i<=m;i++)
		dp[0][i]=Number(BITLENGTH,-i,PUBLIC);
	for(int i=1;i<=n;i++){
		int cur=i&1;
		int pre=cur^1;
        Number t;
		dp[cur][0]=Number(BITLENGTH,-i,PUBLIC);
		for(int j=1;j<=m;j++){
			t=If(a[i]==b[j],five,mone); 
            arg0[j]= dp[pre][j]<=dp[cur][j-1];
            dp[cur][j]=dp[pre][j].select(arg0[j],dp[cur][j-1])-two;
            arg1[j]= dp[cur][j]<=dp[pre][j-1]+t;
            dp[cur][j]=dp[cur][j].select(arg1[j],dp[pre][j-1]+t);


        } 
    }

    cout<< dp[n&1][m].output() <<std::endl;
}
int main(int argc, char** argv) {
	int party,port;
    parse_party_and_port(argv, &party, &port);
	NetIO *io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port); 

    setup(io,party);

     plain(NULL);
 
    code(NULL);
}
