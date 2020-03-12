#ifndef ARI_GEN_H__
#define ARI_GEN_H__
#include "emp-tool/emp-tool.h"
#include "emp-arith/numeric.h"
#include "emp-arith/hash_idx.h"
#include <iostream>

namespace emp {
template<typename T>
class AriGen: public ArithmeticExecution{ 
public:
	PRP prp;
	T * io;
	int64_t gid = 0;
	AriGen(T * io) :io(io) {
		PRG tmp;
		block a,b;
		tmp.random_block(&a, 1);
		*((char *) &a) |= 0x01;
		tmp.random_block(&b, 1);
		modBlock(b);

		set_delta(a,b);
		eq_hash.reset();
	}
    /*
     FOR BOOLEAN
    */
	block constant[2];
	Number powerOf2[LOGMOD];
	Number zeros[LOGMOD];
	block delta;
	vector<char>buffer;
	PRG prg;
	bool is_public(const block & b, int party) {
		return false;
	}
	bool isDelta(const block & b) {
		__m128i neq = _mm_xor_si128(b, delta);
		return _mm_testz_si128(neq, neq);
	}
	void set_delta(const block &_delta,const block &_mdelta) {
		this->delta = make_delta(_delta);
		this->mdelta = _mdelta;
		modBlock(this->mdelta);
		PRG prg2(fix_key);prg2.random_block(constant, 2);
		*((char *) &constant[0]) &= 0xfe;
		*((char *) &constant[1]) |= 0x01;
		constant[1]=xorBlocks(constant[1],delta);

		for(int i=0;i<LOGMOD;i++){
			zeros[i]=Number();
			if(i==0){
				prg2.random_block(&powerOf2[0].mask, 1);
				modBlock(powerOf2[0].mask);
				delta2k[0]=this->mdelta;
				powerOf2[0].mask=subBlocks(powerOf2[0].mask,mdelta);
			}else{
				delta2k[i]=addBlocks(delta2k[i-1],delta2k[i-1]);
				add_gate(powerOf2[i],powerOf2[i-1],powerOf2[i-1]);
			}
		}
	}
	block public_label(bool b) {
		return constant[b];
	}
	block and_gate(const block& a, const block& b) {
		block out[2], table[2];
		garble_gate_garble_privacy_free(a, xorBlocks(a,delta), b, xorBlocks(b,delta), 
				&out[0], &out[1], delta, table, gid++, &prp.aes);
		io->send_block(table, 1);
		return out[0];
	}
	block xor_gate(const block&a, const block& b) {
		return xorBlocks(a, b);
	}
	block not_gate(const block& a) {
		return xor_gate(a, public_label(true));
	}

    /*
     FOR ARITHMETIC
    */
	block mdelta;
	block delta2k[LOGMOD];
	void add_gate(Number &c,const Number &a,const Number &b){
        c.mask=addBlocks(a.mask,b.mask);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
        c.mask=subBlocks(a.mask,b.mask);
	}	

	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		block diff=subBlocks(x.mask,y.mask);
//xiao: merge to one call?
//TODO
		block h0=hash_with_idx(b.bit,gid,&prp.aes);
		block h1=hash_with_idx(xorBlocks(b.bit,delta),gid,&prp.aes);
		gid++;
		modBlock(h0);
		modBlock(h1);
		block R=subBlocks(h1,h0);
		R=subBlocks(R,diff);
		io->send_block(&R,1);
		z.mask=subBlocks(y.mask,h0);
	}

	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		if(length*2*sizeof(block)>buffer.size())
			buffer.resize(length*2*sizeof(block));
		block *h=(block*)buffer.data();
		block *h0,*h1;
		
		hash_with_delta(length,h,(block*)bits,gid,delta,&prp.aes);
		gid+=length;

		h0=h;
		h1=h+length;

		for(int i=0;i<length;i++){	
			block diff=subBlocks(a[i].mask,b[i].mask);
			modBlock(h0[i]);
			modBlock(h1[i]);
			block R=subBlocks(h1[i],h0[i]);
			R=subBlocks(R,diff);
			io->send_block(&R,1);
			c[i].mask=subBlocks(b[i].mask,h0[i]);
		}

	}
	Number b2a_gate(const Integer &x){
		Number res[LOGMOD];
		powerOf2[x.size()-1]=-powerOf2[x.size()-1];
		sels_gate(x.size(),res,x.bits,powerOf2,zeros);
		powerOf2[x.size()-1]=-powerOf2[x.size()-1];
		Number c=zero_block();
		for(int i=0;i<x.size();i++)
			c=c+res[i];
		return c;
	}
	void set_gate(long long x,Number &a){
		prg.random_block(&a.mask,1);
		modBlock(a.mask);  
		block xdelta; 
		xdelta=mulCBlocks(mdelta,x);
		xdelta=addBlocks(xdelta,a.mask);
		io->send_block(&xdelta,1);		
	
	}
	int reveal_gate(const Number &a){
		block tmp;
		io->recv_block(&tmp,1);
		block val=subBlocks(tmp,a.mask);
		modBlock(val); 
		long long v=(get_val(val)%MOD+MOD)%MOD,t=0;
		long long d=get_val(mdelta)%MOD;
		// v*inv(d) mod MOD
		// stupid version
		// TODO 	
		for(int i=0;i<1000;i++){
			if(t%MOD==v)
				return i;
			if((MOD-t)%MOD==v)
				return -i;
			t=(t+d)%MOD;
		}
		return -233;		
	}
	Number proj_gate(const Number &a,int length,const int *x,const int *y){
		
		Number b;
		prg.random_block(&b.mask,1);
		
		modBlock(b.mask);
		// a+xdeleta ->  b+ydelta
		static block xdelta[BITLENGTH];
		static block ydelta[BITLENGTH];
		static block h[2*BITLENGTH],t[2*BITLENGTH];


		
		for(int i=0;i<length;i++){
			 
			xdelta[i]=addBlocks(a.mask,mulCBlocks(mdelta,x[i]));
			ydelta[i]=addBlocks(b.mask,mulCBlocks(mdelta,y[i]));
			
		}
		
 
		
		for(int i=0;i<length;i++){
			h[i*2]=xdelta[i];
			h[i*2+1]=xorBlocks(xdelta[i],one_block());
		}

		hash_with_indices_fix(length*2,h,h,gid,&prp.aes);
		for(int i=0;i<length;i++){

			t[i*2]=xorBlocks(h[i*2],ydelta[i]);
			t[i*2+1]=h[i*2+1]; 
		}

		io->send_block(t,2*length);
		gid++;
		//TODO shuffle
		return b;
	}

	Hash eq_hash;
	bool eq(const Number &a,const Number &b){
		Number diff=a-b;
		eq_hash.put(&diff.mask,sizeof(block));
		return true;
	}
	bool is_true(const Bit &bit){
		block tmp=xorBlocks(bit.bit,delta);
		eq_hash.put(&tmp,sizeof(block));
		return true;
	}
};
}


#endif// GEN_H__
