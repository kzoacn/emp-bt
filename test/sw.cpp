//MPC for Smith–Waterman algorithm
//kaiyi zhang(kzoacn@outlook.com)
#include <emp-tool/emp-tool.h>
#include "emp-arith/emp-arith.h"
#include "emp-arith/arithmetic_gen.h"
#include "emp-arith/arithmetic_eva.h"
#include "emp-arith/json.hpp"
using namespace emp;
using namespace std;
using json=nlohmann::json;


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

Number sv(Number a){
	return project(a,8,zero_x,zero_y);
}
Number s_value(Number a,Number b){
	return sv(b-a);
}


vector<Number>a,b;

vector<vector<Number> >dp;
vector<Number>arg0,arg1;
 
vector<int>pa;
vector<int>pb;

char mp[]="ATCG";
/*
TODO:
1.json done
2.disk done
3.improve tab
4. inverse in reveal
5. NetIO
*/

int main(int argc, char** argv) { 

	for(int i=0;i<40;i++){
		max_x[i]=i%2==0?i/2:-((i+1)/2);
		max_y[i]=max(0,max_x[i]); 

		zero_x[i]=i%2==0?i/2:-((i+1)/2);
		zero_y[i]= zero_x[i]==0 ? 5 :-1; 

		arg_x[i]=i%2==0?i/2:-((i+1)/2);
		arg_y[i]= arg_x[i]<=0 ? 1 : 0; 
	}

	if(argc!=2){
		fprintf(stderr,"usage: ./sw <config.json>\n");
		return 0;	
	}

	ifstream fin(argv[1]);
	json js;
	fin>>js;

	party=js["party"];
	port=js["port"];
	string ip=js["ip"];
	string input_file=js["input_file"];
	string temporary_file=js["temporary_file"];
	bool from_stdin=js["from_stdin"];

	//parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : ip.c_str(), port); 
	setup_arithmetic(io, party); 

    int n=0,m=0;
	string gene;

	if(from_stdin){
		cin>>gene;
	}else{
		ifstream gin(input_file);
		gin>>gene; 
	}

	
	if(party==ALICE){
		n=gene.length();
		pa.resize(n+1);
		for(int i=0;i<n;i++){
			if(gene[i]=='A')pa[i+1]=0;
			if(gene[i]=='T')pa[i+1]=1;
			if(gene[i]=='C')pa[i+1]=2;
			if(gene[i]=='G')pa[i+1]=3;
		}
		io->recv_data(&m,4); 
		pb.resize(m+1);
		io->send_data(&n,4); 
		io->flush();
		
	}else{
		m=gene.length();
		pb.resize(m+1);
		for(int i=0;i<m;i++){
			if(gene[i]=='A')pb[i+1]=0;
			if(gene[i]=='T')pb[i+1]=1;
			if(gene[i]=='C')pb[i+1]=2;
			if(gene[i]=='G')pb[i+1]=3;
		}
		io->send_data(&m,4); 
		io->recv_data(&n,4); 
		pa.resize(n+1);
		io->flush();
	} 
  
	a.resize(n+1);
	b.resize(m+1);
    for(int i=1;i<=n;i++){ 
        a[i]=Number(BITLENGTH,pa[i],ALICE); 
    }
    for(int i=1;i<=m;i++){  
        b[i]=Number(BITLENGTH,pb[i],BOB);
    }


	/*for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            tab[i][j]=s_value(a[i],b[j]);
        }
    }
 
    for(int i=0;i<=n;i++)
        dp[i][0]=Number(BITLENGTH,0,ALICE);
    for(int i=0;i<=m;i++)
        dp[0][i]=Number(BITLENGTH,0,BOB);
*/

	Number two(BITLENGTH,2,ALICE);
 	
	dp.resize(2);
	arg0.resize(m+1);
	arg1.resize(m+1);
	dp[0].resize(m+1);
	dp[1].resize(m+1);
	ofstream tout;
	tout.open(temporary_file,std::ios::out | std::ios::binary);

	for(int i=1;i<=m;i++)
		dp[0][i]=Number(BITLENGTH,-i,ALICE);
	for(int i=1;i<=n;i++){
		int cur=i&1;
		int pre=cur^1;
        Number t;
		dp[cur][0]=Number(BITLENGTH,-i,ALICE);
		for(int j=1;j<=m;j++){
			t=s_value(a[i],b[j]);
            dp[cur][j]=t_max(dp[pre][j],dp[cur][j-1])-two;
            arg0[j]=arg_max(dp[pre][j],dp[cur][j-1]);
		    arg1[j]=arg_max(dp[cur][j],dp[pre][j-1]+t); 
			dp[cur][j]=t_max(dp[cur][j],dp[pre][j-1]+t);
        }

		tout.write((char*)arg0.data(),(m+1)*sizeof(Number));
		tout.write((char*)arg1.data(),(m+1)*sizeof(Number));

    } 
	tout.close();

 	ifstream tin;
	tin.open(temporary_file,std::ios::in | std::ios::binary);
	int nowx=n,nowy=m;
	vector<char>ans;
	
	while(nowx>=1&&nowy>=1){  

		tin.seekg(2LL*(nowx-1)*(m+1)*sizeof(Number)+(m+1)*sizeof(Number)+nowy*sizeof(Number),ios::beg);
		
		Number D1;
		tin.read((char*)&D1,sizeof(Number));
		int d1=D1.reveal();
		if(d1==1){
			if(party==ALICE){
				ans.push_back(mp[pa[nowx]]);
			}else{
				ans.push_back(mp[pb[nowy]]);
			}

			nowx--;nowy--;
		}else{
			tin.seekg(2LL*(nowx-1)*(m+1)*sizeof(Number)+nowy*sizeof(Number),ios::beg);
			
			Number D2;
			tin.read((char*)&D2,sizeof(Number));
			int d2=D2.reveal();
			
			if(d2==0){
				if(party==ALICE){
					ans.push_back(mp[pa[nowx]]);
				}else{
					ans.push_back('-');
				}
				nowx--;
			}
			if(d2==1){
				if(party==ALICE){
					ans.push_back('-');
				}else{
					ans.push_back(mp[pb[nowy]]);
				}
				nowy--;
			}
		}
	}
	while(nowx>=1){
		if(party==ALICE)
			ans.push_back(mp[pa[nowx]]);
		else
			ans.push_back('-');
		nowx--;
	}
	while(nowy>=1){
		if(party==ALICE)
			ans.push_back('-');	
		else
			ans.push_back(mp[pb[nowy]]);
		nowy--;
	}

	reverse(ans.begin(),ans.end());
	for(char c:ans)
		cout<<c;
	cout<<endl;


	
	return 0;
}
