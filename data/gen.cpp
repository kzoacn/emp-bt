#include<bits/stdc++.h>
using namespace std;
int main(int argc,char **argv){
	srand(time(0));
	int n;
	sscanf(argv[1],"%d",&n);
	for(int i=1;i<=n;i++){
		int r=rand()%4;
		char c;
		if(r==0)c='A';
		if(r==1)c='T';
		if(r==2)c='C';
		if(r==3)c='G';
		cout<<c;
	}
	cout<<endl;
}
