 
#include <emp-tool/emp-tool.h>
#include "emp-arith/emp-arith.h"
#include "emp-arith/arithmetic_gen.h"
#include "emp-arith/arithmetic_eva.h"
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
int zero_x[40];
int zero_y[40];
int arg_x[40];
int arg_y[40];
Number t_max0(Number a){
	return project(a,20,max_x,max_y);
}

Number t_max(Number a,Number b){
	return a+t_max0(b-a);
}



Number arg_max(Number a,Number b){
	return project(a-b,20,arg_x,arg_y);
}
// 0 1

Number sv(Number a){
	return project(a,8,zero_x,zero_y);
}
Number s_value(Number a,Number b){
	return sv(b-a);
}


const int maxn=1010;
Number a[maxn],b[maxn];
Number dp[maxn][maxn];

Number arg0[maxn][maxn];
Number arg1[maxn][maxn];

Number tab[maxn][maxn];

int f[maxn][maxn];
int g0[maxn][maxn];
int g1[maxn][maxn];

int pa[maxn],pb[maxn];
 

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port); 
	setup_arithmetic(io, party); 

	for(int i=0;i<40;i++){
		max_x[i]=i%2==0?i/2:-((i+1)/2);
		max_y[i]=max(0,max_x[i]); 

		zero_x[i]=i%2==0?i/2:-((i+1)/2);
		zero_y[i]= zero_x[i]==0 ? 5 :-1; 

		arg_x[i]=i%2==0?i/2:-((i+1)/2);
		arg_y[i]= arg_x[i]<=0 ? 1 : 0; 
	}



	
	/*for(int i=-4;i<=3;i++)
	for(int j=-4;j<=3;j++){

		Number t1(BITLENGTH,i,ALICE);
		Number t2(BITLENGTH,j,BOB);

		Number bt=arg_max(t1,t2);
		int r=bt.reveal();
		if(party==ALICE)
			cout<< (i<=j)<<" " <<r <<endl;
		
	}
	return 0;*/

 
    int n=1000,m=1000; 

    for(int i=1;i<=n;i++){
        pa[i]=rand()%4;
        pb[i]=rand()%4;

        a[i]=Number(BITLENGTH,pa[i],ALICE);
        b[i]=Number(BITLENGTH,pb[i],BOB);
    }


	for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            tab[i][j]=s_value(a[i],b[j]);
        }
    }

    
    for(int i=0;i<=n;i++)
        dp[i][0]=Number(BITLENGTH,0,ALICE);
    for(int i=0;i<=m;i++)
        dp[0][i]=Number(BITLENGTH,0,BOB);

	Number two(BITLENGTH,2,ALICE);
 	for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            dp[i][j]=t_max(dp[i-1][j],dp[i][j-1])-two;

            arg0[i][j]=arg_max(dp[i-1][j],dp[i][j-1]);
		    arg1[i][j]=arg_max(dp[i][j],dp[i-1][j-1]+tab[i][j]); 

			dp[i][j]=t_max(dp[i][j],dp[i-1][j-1]+tab[i][j]);
        }
    }

	cout<<dp[n][m].reveal()<<endl;



	for(int i=1;i<=n;i++)
	for(int j=1;j<=m;j++){
		f[i][j]=max(f[i][j-1]-2,f[i-1][j]-2);
		
		g0[i][j]= f[i-1][j] <= f[i][j-1] ? 1 : 0;
		g1[i][j]= f[i][j] <= (f[i-1][j-1]+(pa[i]==pb[j]?5:-1)) ? 1 : 0;
		
		f[i][j]=max(f[i][j],f[i-1][j-1]+(pa[i]==pb[j]?5:-1));
	}
	
	int ans1=0,ans2=0;
	for(int i=1;i<=n;i++)
	for(int j=1;j<=m;j++){
		
		//printf("%d%c",f[i][j]," \n"[j==m]);
		int d1=abs(f[i-1][j]-f[i][j-1]);
		int d2=abs(max(f[i-1][j],f[i][j-1])-f[i-1][j-1]-(pa[i]==pb[j]?5:-1));

		int mn=min(min(f[i-1][j-1],f[i][j]),min(f[i][j-1],f[i-1][j]));
		
		ans1=max(ans1,d1);
		ans2=max(ans2,d2);

	} 
	cout<<f[n][m]<<endl; 

	for(int i=1;i<=n;i++)
	for(int j=1;j<=m;j++){

		myassert(f[i][j]==dp[i][j].reveal());
		myassert(g0[i][j]==arg0[i][j].reveal()); 
		myassert(g1[i][j]==arg1[i][j].reveal()); 
	}

	int nowx=n,nowy=m;

	int h1=0;
	while(nowx>=1&&nowy>=1){
		//printf("%d %d\n",nowx,nowy);
		
		h1^=nowx*2;
		h1+=nowy;

		int d1=g1[nowx][nowy];
		if(d1==1){
			nowx--;nowy--;
		}else{
			int d2=g0[nowx][nowy];
			if(d2==0)nowx--;
			if(d2==1)nowy--;
		}
	}
	cout<<h1<<endl;


	nowx=n,nowy=m;

	int h2=0;
	while(nowx>=1&&nowy>=1){
		//printf("%d %d\n",nowx,nowy);
		
		h2^=nowx*2;
		h2+=nowy;

		int d1=arg1[nowx][nowy].reveal();
		if(d1==1){
			nowx--;nowy--;
		}else{
			int d2=arg0[nowx][nowy].reveal();
			if(d2==0)nowx--;
			if(d2==1)nowy--;
		}
	}
	cout<<h2<<endl;


	
	return 0;
}
