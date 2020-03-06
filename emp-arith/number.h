#ifndef ZK_NUMBER_H__
#define ZK_NUMBER_H__
#include "emp-tool/emp-tool.h"
#include "emp-arith/arithmetic_execution.h"
#include "emp-arith/numeric.h"
namespace emp {
class Number{ public:
	block mask; 

	Number(const block& _m=zero_block()) {
		memcpy(&mask, &_m, sizeof(block)); 
	}
    
    Number(int length, long long input, int party);


	Number operator+(const Number& rhs) const; 
	Number operator-(const Number& rhs) const; 
    Number operator-() const;

    /*Number operator*(const Number& rhs) const; 
    Number operator*(const Integer& rhs) const;
    Number operator/(const Number& rhs) const;  
    Number operator%(const Number& rhs) const;  
    

    Bit operator==(const Number& rhs) const;
    Bit operator!=(const Number& rhs) const;
    Bit operator<(const Number &rhs) const;
    Bit operator<=(const Number &rhs) const;
    Bit operator>(const Number &rhs) const;
    Bit operator>=(const Number &rhs) const;

    Number operator>>(int shamt) const; 
    Number operator<<(int shamt) const; 
	Number operator<<(const Number& shamt)const;
	Number operator>>(const Number& shamt)const;


	Number operator&(const Number& rhs)const;
	Number operator|(const Number& rhs)const;    
    std::pair<Number,Number> div(const Number &rhs)const;

    Number sqrt()const;
    */
    Number select(Bit bit,Number rhs)const;
    int reveal()const;
};
inline Number b2a(Integer x);
//inline Integer a2b(const Number &x,int length);
inline Number project(const Number &a,int length,const int *x,const int *y);// x[i] -> y[i] 
#include "emp-arith/number.hpp"
}
#endif
