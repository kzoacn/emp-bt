#ifndef CIR_H
#define CIR_H
#include <iostream>
#include <string>
#include<emp-tool/utils/constants.h>
#include<emp-tool/io/net_io_channel.h>

namespace emp{

 
class Circuit_Gen { 

public: 
	static Circuit_Gen * circ_exec; 
    NetIO *io;
    int party;
    bool mask;

    Circuit_Gen(NetIO *i,int p){
        io=i;
        party=p;
        mask=party==ALICE?1:0;
    }
 

    long long num_gate, num_wire, n1, n2, n3;
    long long cur_n1,cur_n2,cur_n3;
    long long cur;
    
	bool and_gate(const bool& x1, const bool& y1){ 
        /*
            Beaver Triple
            (a1+a2)(b1+b2)=c1+c2
            default all zero
        */
       bool a1=false,b1=false,c1=false;

        /*
            e=open(x+a)
            d=open(y+b)
            [xy]=[c]+e[y]+d[x]-ed
        */
        bool e1=x1^a1,d1=y1^b1;
        bool e2,d2;
        if(this->party==ALICE){
            io->send_data(&e1,1);
            io->send_data(&d1,1);
            io->recv_data(&e2,1);
            io->recv_data(&d2,1);
        }else{
            io->recv_data(&e2,1);   
            io->recv_data(&d2,1);
            io->send_data(&e1,1);
            io->send_data(&d1,1);    
        }
        bool e=e1^e2,d=d1^d2;
        bool res=c1^(e&y1)^(d&x1)^(e&d&mask);
        //printf("%d %d %d %d %d\n",e,d,x1,y1,(int)res);
        return res;    
    }
	bool xor_gate(const bool&in1, const bool&in2){ 
        return in1^in2;
    }
	bool not_gate(const bool& in1){ 
        return in1^mask;
    }
	bool public_label(bool b,int party){ 
        return party==ALICE?b:0;
    }
    bool set_input(bool b,int party){ 
        if(party==this->party)
            return b;
        return 0;
    }
    bool output(bool b1){
        bool b2;
        if(this->party==ALICE){
            io->send_data(&b1,1);
            io->recv_data(&b2,1);
        }else{
            io->recv_data(&b2,1);
            io->send_data(&b1,1);
        }
        return b1^b2;
    }
    
    
	~Circuit_Gen (){ }
}; 

emp::Circuit_Gen* emp::Circuit_Gen::circ_exec = nullptr; 

void setup(NetIO *io,int party){
    io->set_nodelay();
    emp::Circuit_Gen::circ_exec = new emp::Circuit_Gen(io,party); 
}

}

#endif