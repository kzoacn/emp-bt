#ifndef ARITHMETICZK_H__
#define ARITHMETICZK_H__
#include "emp-arith/arithmetic_gen.h"
#include "emp-arith/arithmetic_eva.h" 
#include <emp-tool/emp-tool.h>

 

namespace emp {
template<typename IO>
inline void setup_arithmetic(IO* io, int party) {
	if(party == ALICE) {
		AriGen<IO> * t = new AriGen<IO>(io);
		CircuitExecution::circ_exec = t;
		ArithmeticExecution::ari_exec=t;
		ProtocolExecution::prot_exec = new ArithmeticZKGen<IO>(io, t);
	} else {
		AriEva<IO> * t = new AriEva<IO>(io);
		CircuitExecution::circ_exec = t;
		ArithmeticExecution::ari_exec=t;
		ProtocolExecution::prot_exec = new ArithmeticZKEva<IO>(io, t);
	}
}
 




}
#endif
