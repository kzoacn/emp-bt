#include<iostream>
using namespace std;
 
inline void add_full(bool * dest, const bool * op1, const bool * op2,int size) {
	
    bool axc,bxc,carry=false,t;
    int i=0;
    while(size-->0) { 
		axc = op1[i] ^ carry;
		bxc = op2[i] ^ carry;
		dest[i] = op1[i] ^ bxc;
		t = axc & bxc;
		carry =carry^t;
		++i;
	}
	dest[i] = carry ^ op2[i] ^ op1[i];
}

void KoggeStone_adder(bool * dest, const bool * op1, const bool * op2,int size) {
    // size=2^k

    bool g[128],p[128];
    for(int i=0;i<size;i++){
        p[i]=op1[i]^op2[i];
        g[i]=op1[i]&op2[i];
    }
 
    for(int h=0;(1<<h)<size;h++){
        int l=1<<h;
        for(int i=size-1;i>=l;i--){
            //s[i]+=s[i-l];
            bool p1,p2,g1,g2;
            p1=p[i];p2=p[i-l];
            g1=g[i];g2=g[i-l];
            
            g[i]=g1^(p1&g2);
            p[i]=p1&p2;
        } 
    }

    dest[0]=op1[0]^op2[0];
    for(int i=1;i<size;i++)
        dest[i]=g[i-1]^op1[i]^op2[i];

}


bool a[111],b[111],c[111];
int main(){
    int n=32;
    srand(111);
    for(int i=0;i<n;i++){
        a[i]=rand()%2;
        b[i]=rand()%2;
    }
    for(int i=0;i<n;i++)
        printf("%d",a[i]);
    puts("");

    for(int i=0;i<n;i++)
        printf("%d",b[i]);
    puts("");

    add_full(c,a,b,n);
    for(int i=0;i<n;i++)
        printf("%d",c[i]);
    puts("");


    KoggeStone_adder(c,a,b,n);
    for(int i=0;i<n;i++)
        printf("%d",c[i]);
    puts("");

    return 0;
}