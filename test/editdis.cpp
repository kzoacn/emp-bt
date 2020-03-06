 
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


int min_x[40];
int min_y[40];
int zero_x[40];
int zero_y[40];
Number t_min0(Number a){
	return project(a,8,min_x,min_y);
}

Number t_min(Number a,Number b){
	return a+t_min0(b-a);
}

Number is_notzero(Number a){
	return project(a,8,zero_x,zero_y);
}
Number neq(Number a,Number b){
	return is_notzero(b-a);
}


const int maxn=1010;
Number a[maxn],b[maxn];
Number dp[maxn][maxn];
Number tab[maxn][maxn];
int f[maxn][maxn];

int pa[maxn],pb[maxn];

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_arithmetic(io, party); 
	for(int i=0;i<10;i++){
		min_x[i]=i%2==0?i/2:-((i+1)/2);
		min_y[i]=min(0,min_x[i]); 

		zero_x[i]=i%2==0?i/2:-((i+1)/2);
		zero_y[i]= zero_x[i]!=0; 
	}
 
    int n=1000,m=1000;

    for(int i=1;i<=n;i++){
        pa[i]=rand()%4;
        pb[i]=rand()%4;

        a[i]=Number(BITLENGTH,pa[i],ALICE);
        b[i]=Number(BITLENGTH,pb[i],BOB);
    }

   /* if(party==ALICE){
        for(int i=1;i<=n;i++)
            printf("%d%c",pa[i]," \n"[i==n]);
    }
    if(party==BOB){
        for(int i=1;i<=n;i++)
            printf("%d%c",pb[i]," \n"[i==n]);
    }
*/
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            tab[i][j]=neq(a[i],b[j]);
        }
    }


    double st=clock();

    for(int i=0;i<=n;i++)
        dp[i][0]=Number(BITLENGTH,i,ALICE);
    for(int i=0;i<=m;i++)
        dp[0][i]=Number(BITLENGTH,i,BOB);

    Number c_del(BITLENGTH,1,ALICE);
    Number c_ins(BITLENGTH,1,ALICE); 

    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            dp[i][j]=t_min(dp[i-1][j]+c_ins,dp[i][j-1]+c_del);
            dp[i][j]=t_min(dp[i][j],dp[i-1][j-1]+tab[i][j]);
        }
    }
    
    cout<<(clock()-st)/CLOCKS_PER_SEC<<endl;


    for(int i=0;i<=n;i++)
        f[i][0]=i;
    for(int i=0;i<=m;i++)
        f[0][i]=i;
 



    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            f[i][j]=min(f[i-1][j]+1,f[i][j-1]+1);
            f[i][j]=min(f[i][j],f[i-1][j-1]+(pa[i]!=pb[j]));
        
        }
    }
    
     

    
    cout<<dp[n][m].reveal()<<endl;
    cout<<f[n][m]<<endl;





	puts("Yes");



	delete io;
}
