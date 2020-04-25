#ifndef WIRE_H__
#define WIRE_H__
#include "emp-bt/cir.h"
#include "emp-tool/utils/utils.h" 
#include "emp-tool/utils/constants.h"

namespace emp {

class Wire;
template<typename T>
class Swappable { public:
	T If(const Wire & sel, const T& rhs) const {
		return static_cast<const T*>(this)->select(sel, rhs);
	}
};
template<typename T>
inline T If(const Wire & select, const T & o1, const T & o2) {
	T res = o2;
	return res.If(select, o1);
}
template<typename T>
inline void swap(const Wire & swap, T & o1, T & o2) {
	T o = If(swap, o1, o2);
	o = o ^ o2;
	o1 = o ^ o1;
	o2 = o ^ o2;
}

class Wire : public Swappable<Wire>{ public:
	bool bit;

	Wire(bool _b = false, int party = PUBLIC); 

	Wire operator!=(const Wire& rhs) const; 
	Wire operator==(const Wire& rhs) const;
	Wire operator &(const Wire& rhs) const;  
	Wire operator |(const Wire& rhs) const;
	Wire operator !() const; 

	//swappable
	Wire select(const Wire & select, const Wire & new_v)const ;
	Wire operator ^(const Wire& rhs) const;

	//batcher
	template<typename... Args>
	static size_t bool_size(Args&&... args) {
		return 1;
	}

	static void bool_data(bool *b, bool data) {
		b[0] = data;
	}

	Wire(size_t size, const bool* a) {
		memcpy(&bit, a, sizeof(bool));
	}
	bool output(){
		return Circuit_Gen::circ_exec->output(bit);
	}
};
#include "emp-bt/wire.hpp"
}
#endif
