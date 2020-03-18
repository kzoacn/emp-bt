//Smith–Waterman algorithm
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
 
 

/*
const int maxn=1010; 

int f[maxn][maxn];
int g0[maxn][maxn];
int g1[maxn][maxn];

int pa[maxn],pb[maxn];
*/

vector<vector<int> >f;
vector<vector<int> >g0;
vector<vector<int> >g1;
vector<int>pa;
vector<int>pb;

string gene;
 
/*
TODO:
1.json
2.disk
3.improve tab
*/
const char mp[]="ATCG";
int main(int argc, char** argv) {


	if(argc!=2){
		fprintf(stderr,"usage: ./sw_plain <config.json>\n");
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
		int *tmp=new int[m+1];
		io->recv_data(tmp+1,m*sizeof(int));
		pb=vector<int>(tmp,tmp+m+1);
		
		
		io->send_data(&n,4);
		io->send_data(pa.data()+1,n*sizeof(int));
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
		io->send_data(pb.data()+1,m*sizeof(int));
		
		
		io->recv_data(&n,4);
		int *tmp=new int[m+1];
		io->recv_data(tmp+1,n*sizeof(int));
		pa=vector<int>(tmp,tmp+m+1);
		io->flush();
	} 



	f.resize(n+1);
	g0.resize(n+1);
	g1.resize(n+1);
	for(int i=0;i<=n;i++){
		f[i].resize(m+1);
		g0[i].resize(m+1);
		g1[i].resize(m+1);
	}


	ofstream tout;
	tout.open(temporary_file,std::ios::out | std::ios::binary);


	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			f[i][j]=max(f[i][j-1]-2,f[i-1][j]-2);
			
			g0[i][j]= f[i-1][j] <= f[i][j-1] ? 1 : 0;
			g1[i][j]= f[i][j] <= (f[i-1][j-1]+(pa[i]==pb[j]?5:-1)) ? 1 : 0;

			f[i][j]=max(f[i][j],f[i-1][j-1]+(pa[i]==pb[j]?5:-1));
		}
		tout.write((char*)g0[i].data(),(m+1)*sizeof(int));
		tout.write((char*)g1[i].data(),(m+1)*sizeof(int));
	}

	tout.close(); 


	ifstream tin;
	tin.open(temporary_file,std::ios::in | std::ios::binary);

	int nowx=n,nowy=m;
 
	vector<char>ans;
	
	while(nowx>=1&&nowy>=1){  

		tin.seekg(2LL*(nowx-1)*(m+1)*sizeof(int)+(m+1)*sizeof(int)+nowy*sizeof(int),ios::beg);
		int d1;
		tin.read((char*)&d1,4);
		//int d1=g1[nowx][nowy];
		if(d1!=g1[nowx][nowy]){
			puts("error");
			return -1;
		}
		if(d1==1){
			if(party==ALICE){
				ans.push_back(mp[pa[nowx]]);
			}else{
				ans.push_back(mp[pb[nowy]]);
			}

			nowx--;nowy--;
		}else{
			tin.seekg(2LL*(nowx-1)*(m+1)*sizeof(int)+nowy*sizeof(int),ios::beg);
			int d2;
			tin.read((char*)&d2,4);
			if(d2!=g0[nowx][nowy]){
				puts("error");
				return 0;
			}
			//int d2=g0[nowx][nowy];
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

	reverse(ans.begin(),ans.end());
	for(char c:ans)
		cout<<c;
	cout<<endl;

	
	return 0;
}
