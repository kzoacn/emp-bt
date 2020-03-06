#ifndef ARITHMETICZK_EVA_H__
#define ARITHMETICZK_EVA_H__
#include <emp-tool/emp-tool.h>
#include <emp-ot/emp-ot.h>
#include "emp-arith/ari_eva.h" 

namespace emp {
template<typename IO>
class ArithmeticZKEva: public ProtocolExecution {
public:
	IO* io = nullptr;
	SHOTExtension<IO>* ot;
	AriEva<IO> * gc;
	PRG shared_prg;
	
	block seed;

	ArithmeticZKEva(IO *io, AriEva<IO> * gc): ProtocolExecution(BOB) {
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
	~ArithmeticZKEva() {
		delete ot;
	} 

	void feed(block * label, int party, const bool* b, int length) {


		ot->recv_cot(label,b,length); 
		for(int i=0;i<length;i++){
			*((char*)&label[i])&=0xfe;
			*((char*)&label[i])|=b[i];
		}

	}

	void reveal(bool * b, int party, const block * label, int length) {
        throw;
	}

};
}

#endif