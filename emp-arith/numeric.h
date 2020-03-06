#ifndef NUMERIC_H
#define NUMERIC_H
#include "emp-tool/emp-tool.h"

namespace emp {
 

#define MOD ((1LL<<61)-1)
#define HALFMOD (MOD>>1)
#define LOGMOD 60

#ifndef BITLENGTH

#define BITLENGTH 55

#endif
const block P=_mm_set_epi64x(MOD,MOD);

void printBlock(block var)  {
    int64_t *v64val = (int64_t*) &var;
    printf("%.16llx %.16llx\n", (long long)v64val[1], (long long)v64val[0]);
}

inline void modBlock(block &x) {
    x=_mm_add_epi64(_mm_and_si128(x,P),_mm_srli_epi64(x,61));
    x=_mm_sub_epi64(x,_mm_andnot_si128(_mm_cmpgt_epi64(P,x),P));
}

inline void modBlock_one(block &x) {
    x=_mm_sub_epi64(x,_mm_andnot_si128(_mm_cmpgt_epi64(P,x),P));
}

inline block addBlocks(const block &x,const block &y) {
    block res=_mm_add_epi64(x,y);
    modBlock_one(res);
    return res;
}

inline block subBlocks(const block &x,const block &y) {
    block res=_mm_sub_epi64(P,y);
    return addBlocks(x,res);
}

inline block addCBlocks(const block &x,long long y) {
    return addBlocks(x,_mm_set_epi64x(y,y));
}
inline block mulCBlocks(const block &x,long long y) {
    int flag=1;
    if(y<0){
        flag=-1;
        y=-y;
    }
    block c=zero_block(),t=x;
    for(int i=0;i<64;i++){
        if(y>>i&1)
            c=addBlocks(c,t);
        t=addBlocks(t,t);
    }
    if(flag==-1)
        c=_mm_sub_epi64(P,c);
    modBlock(c);
    return c;
}

inline block clear_high(const block &val){//TODO  40bit
    block t=_mm_and_si128(val,_mm_set_epi64x(0,-1));
    return t;
}
inline bool is_high_clear(const block &val){ 
    long long *vv=(long long*)&val;
    return vv[1]==0;
}

long long get_val(const block &val) {
    long long *vv=(long long*)&val;
    return *vv;
}

long long get_low(const block &val) {
    long long *vv=(long long*)&val;
    return *vv;
}
long long get_high(const block &val) {
    long long *vv=(long long*)&val;
    return vv[1];
}

}
#endif
