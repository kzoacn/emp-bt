#ifndef ARITHMETICZK_GEN_H__
#define ARITHMETICZK_GEN_H__
#include <emp-tool/emp-tool.h>
#include <emp-ot/emp-ot.h>
#include "emp-arith/ari_gen.h"
#include <iostream> 

namespace emp {
template<typename IO>
class ArithmeticZKGen: public ProtocolExecution {
public:
	IO* io;
	SHOTExtension<IO> * ot;//TODO SHOT/MOT
	PRG prg, shared_prg;
	AriGen<IO> *gc;
    block seed;
	ArithmeticZKGen(IO* io, AriGen<IO>* gc): ProtocolExecution(ALICE) {
		this->io = io;
		ot = new SHOTExtension<IO>(io);
		this->gc = gc;	
		shared_prg.reseed(fix_key);
		
#ifdef EMP_USE_RANDOM_DEVICE
			int * data = (int *)(&seed);
			std::random_device rand_div;
			for (size_t i = 0; i < sizeof(block) / sizeof(int); ++i)
				data[i] = rand_div();
#else
			unsigned long long r0, r1;
			_rdseed64_step(&r0);
			_rdseed64_step(&r1);
			seed = makeBlock(r0, r1);
#endif
		ot->prg.reseed(&seed);
	}
	~ArithmeticZKGen() {
		delete ot;
	} 

	

	void feed(block * label, int party, const bool* b, int length) {

		ot->send_cot(label,gc->delta,length);
		for(int i=0;i<length;i++)
			*((char*)&label[i])&=0xfe;

	}

	void reveal(bool * b, int party, const block * label, int length) {
		throw;
	}
};
}
#endif