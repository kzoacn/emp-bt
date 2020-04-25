#ifndef NUM_H__
#define NUM_H__

#include "emp-bt/wire.h" 
#include <vector>
#include <bitset>
#include <algorithm>
#include <math.h>
#include <execinfo.h>
using std::vector;
using std::min;
namespace emp {

template<typename T>
class Comparable { public:
	Wire operator>=(const T&rhs) const {
		return static_cast<const T*>(this)->geq(rhs);
	}
	Wire operator<(const T& rhs) const {
		return !( (*static_cast<const T*>(this))>= rhs );
	}

	Wire operator<=(const T& rhs) const {
		return rhs >= *static_cast<const T*>(this);
	}

	Wire operator>(const T& rhs) const {
		return !(rhs >= *static_cast<const T*>(this));
	}
	
	Wire operator==(const T& rhs) const {
		return static_cast<const T*>(this)->equal(rhs);
	}
	Wire operator!=(const T& rhs) const {
		return !(*static_cast<const T*>(this) == rhs);
	}
};

class Number : public Swappable<Number>, public Comparable<Number> { public:
	int length = 0;
	Wire* bits = nullptr;
	Number(Number&& in) : length(in.length) {
		bits = in.bits;
		in.bits = nullptr;
	}
	Number(const Number& in): length(in.length) {
		bits = new Wire[length];
		memcpy(bits, in.bits, sizeof(Wire)*length);
	}
	Number& operator= (Number rhs){
		length = rhs.length;
		std::swap(bits, rhs.bits);
		return *this;
	}
	Number(int len, const void * b) : length(len) {
		bits = new Wire[len];
		memcpy(bits, b, sizeof(Wire)*len);
	}
	~Number() {
		if (bits!=nullptr) delete[] bits;
	}

	Number(int length, const string& str, int party = PUBLIC);
	Number(int length, long long input, int party = PUBLIC);
	Number() :length(0),bits(nullptr){ }

//Comparable
	Wire geq(const Number & rhs) const;
	Wire equal(const Number & rhs) const;

//Swappable
	Number select(const Wire & sel, const Number & rhs) const;
	Number operator^(const Number& rhs) const;

	int size() const; 

	Number abs() const;
	Number& resize(int length, bool signed_extend = true);
	Number modExp(Number p, Number q);
	Number leading_zeros() const;
	Number hamming_weight() const;

	Number operator<<(int shamt)const;
	Number operator>>(int shamt)const;
	Number operator<<(const Number& shamt)const;
	Number operator>>(const Number& shamt)const;

	Number operator+(const Number& rhs)const;
	Number operator-(const Number& rhs)const;
	Number operator-()const;
	Number operator*(const Number& rhs)const;
	Number operator/(const Number& rhs)const;
	Number operator%(const Number& rhs)const;
	Number operator&(const Number& rhs)const;
	Number operator|(const Number& rhs)const;

	Wire& operator[](int index);
	const Wire & operator[](int index) const;
	
//batcher
	template<typename... Args>
	static size_t bool_size(size_t size, Args... args) {
		return size;
	}
	static void bool_data(bool* data, size_t len, long long num) {
		bool_data(data, len, std::to_string(num));
	}
	static void bool_data(bool* data, size_t len, string str) {
		string bin = dec_to_bin(str);
		std::reverse(bin.begin(), bin.end());
//		cout << "convert " <<str<<" "<<bin<<endl;
		int l = (bin.size() > (size_t)len ? len : bin.size());
		for(int i = 0; i < l; ++i)
			data[i] = (bin[i] == '1');
		for (size_t i = l; i < len; ++i)
			data[i] = data[l-1];
	}
	int output(){
		int ans=0;
		for(int i=0;i<length;i++){
			bool b=bits[i].output();
			if(b)ans+=1<<i;
		}
		return ans;
	}
};

void init(Wire * bits, const bool* b, int length, int party = PUBLIC);
#include "emp-bt/num.hpp"
}
#endif// INTEGER_H__
