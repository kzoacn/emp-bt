//https://github.com/samee/obliv-c/blob/obliv-c/src/ext/oblivc/obliv_bits.c#L1487
inline void add_full(Wire* dest, Wire * carryOut, const Wire * op1, const Wire * op2,
		const Wire * carryIn, int size) {
	Wire carry, bxc, axc, t;
	int skipLast; 
	int i = 0;
	if(size==0) { 
		if(carryIn && carryOut)
			*carryOut = *carryIn;
		return;
	}
	if(carryIn)
		carry = *carryIn;
	else 
		carry = false;
	// skip AND on last bit if carryOut==NULL
	skipLast = (carryOut == nullptr);
	while(size-->skipLast) { 
		axc = op1[i] ^ carry;
		bxc = op2[i] ^ carry;
		dest[i] = op1[i] ^ bxc;
		t = axc & bxc;
		carry =carry^t;
		++i;
	}
	if(carryOut != nullptr)
		*carryOut = carry;
	else
		dest[i] = carry ^ op2[i] ^ op1[i];
}
inline void sub_full(Wire * dest, Wire * borrowOut, const Wire * op1, const Wire * op2,
		const Wire * borrowIn, int size) {
	Wire borrow,bxc,bxa,t;
	int skipLast; int i = 0;
	if(size==0) { 
		if(borrowIn && borrowOut) 
			*borrowOut = *borrowIn;
		return;
	}
	if(borrowIn) 
		borrow = *borrowIn;
	else 
		borrow = false;
	// skip AND on last bit if borrowOut==NULL
	skipLast = (borrowOut == nullptr);
	while(size-- > skipLast) {
		bxa = op1[i] ^ op2[i];
		bxc = borrow ^ op2[i];
		dest[i] = bxa ^ borrow;
		t = bxa & bxc;
		borrow = borrow ^ t;
		++i;
	}
	if(borrowOut != nullptr) {
		*borrowOut = borrow;
	}
	else
		dest[i] = op1[i] ^ op2[i] ^ borrow;
}
inline void mul_full(Wire * dest, const Wire * op1, const Wire * op2, int size) {
	//	OblivBit temp[MAX_BITS]={},sum[MAX_BITS]={};
	Wire * sum = new Wire[size];
	Wire * temp = new Wire[size];
	for(int i = 0; i < size; ++i)sum[i]=false;
	for(int i=0;i<size;++i) {
		for (int k = 0; k < size-i; ++k)
			temp[k] = op1[k] & op2[i];
		add_full(sum+i, nullptr, sum+i, temp, nullptr, size-i);
	}
	memcpy(dest, sum, sizeof(Wire)*size);
	delete[] sum;
	delete[] temp;
}

inline void ifThenElse(Wire * dest, const Wire * tsrc, const Wire * fsrc, 
		int size, Wire cond) {
	Wire x, a;
	int i = 0;
	while(size-- > 0) {
		x = tsrc[i] ^ fsrc[i];
		a = cond & x;
		dest[i] = a ^ fsrc[i];
		++i;
	}
}
inline void condNeg(Wire cond, Wire * dest, const Wire * src, int size) {
	int i;
	Wire c = cond;
	for(i=0; i < size-1; ++i) {
		dest[i] = src[i] ^ cond;
		Wire t  = dest[i] ^ c;
		c = c & dest[i];
		dest[i] = t;
	}
	dest[i] = cond ^ c ^ src[i];
}

inline void div_full(Wire * vquot, Wire * vrem, const Wire * op1, const Wire * op2, 
		int size) {
	Wire * overflow = new Wire[size];
	Wire * temp = new Wire[size];
	Wire * rem = new Wire[size];
	Wire * quot = new Wire[size];
	Wire b;
	memcpy(rem, op1, size*sizeof(Wire));
	overflow[0] = false;
	for(int i  = 1; i < size;++i)
		overflow[i] = overflow[i-1] | op2[size-i];
	// skip AND on last bit if borrowOut==NULL
	for(int i = size-1; i >= 0; --i) {
		sub_full(temp, &b, rem+i, op2, nullptr, size-i);
		b = b | overflow[i];
		ifThenElse(rem+i,rem+i,temp,size-i,b);
		quot[i] = !b;
	}
	if(vrem != nullptr) memcpy(vrem, rem, size*sizeof(Wire));
	if(vquot != nullptr) memcpy(vquot, quot, size*sizeof(Wire));
	delete[] overflow;
	delete[] temp;
	delete[] rem;
	delete[] quot;
}


inline void init(Wire * bits, const bool* b, int length, int party) {
	bool * bbits = (bool *) bits;
	if (party == PUBLIC) {
		int one = Circuit_Gen::circ_exec->public_label(true);
		int zero = Circuit_Gen::circ_exec->public_label(false);
		for(int i = 0; i < length; ++i)
			bbits[i] = b[i] ? one : zero;
	}
	else {
		for(int i=0;i<length;i++){
			bbits[i]=Circuit_Gen::circ_exec->set_input(b[i],party);
		} 
	}
}

/*inline Number::Number(const bool * b, int length, int party) {
  bits = new Wire[length];
  init(bits,b,length, party);
  }*/

inline Number::Number(int len, const string& str, int party) : length(len) {
	bool* b = new bool[len];
	bool_data(b, len, str);
	bits = new Wire[length];
	init(bits,b,length, party);
	delete[] b;
}

inline Number::Number(int len, long long input, int party)
	: Number(len, std::to_string(input), party) {
	}

inline Number Number::select(const Wire & select, const Number & a) const{
	Number res(*this);
	for(int i = 0; i < size(); ++i)
		res[i] = bits[i].select(select, a[i]);
	return res;
}

inline Wire& Number::operator[](int index) {
	return bits[min(index, size()-1)];
}

inline const Wire &Number::operator[](int index) const {
	return bits[min(index, size()-1)];
}


inline int Number::size() const {
	return length;
}

//circuits
inline Number Number::abs() const {
	Number res(*this);
	for(int i = 0; i < size(); ++i)
		res[i] = bits[size()-1];
	return ( (*this) + res) ^ res;
}

inline Number& Number::resize(int len, bool signed_extend) {
	Wire * old = bits;
	bits = new Wire[len];
	memcpy(bits, old, min(len, size())*sizeof(Wire));
	Wire extended_bit = old[length-1] & signed_extend;
	for(int i = min(len, size()); i < len; ++i)
		bits[i] = extended_bit;
	this->length = len;
	delete[] old;
	return *this;
}

//Logical operations
inline Number Number::operator^(const Number& rhs) const {
	Number res(*this);
	for(int i = 0; i < size(); ++i)
		res.bits[i] = res.bits[i] ^ rhs.bits[i];
	return res;
}

inline Number Number::operator|(const Number& rhs) const {
	Number res(*this);
	for(int i = 0; i < size(); ++i)
		res.bits[i] = res.bits[i] | rhs.bits[i];
	return res;
}

inline Number Number::operator&(const Number& rhs) const {
	Number res(*this);
	for(int i = 0; i < size(); ++i)
		res.bits[i] = res.bits[i] & rhs.bits[i];
	return res;
}

inline Number Number::operator<<(int shamt) const {
	Number res(*this);
	if(shamt > size()) {
		for(int i = 0; i < size(); ++i)
			res.bits[i] = false;
	}
	else {
		for(int i = size()-1; i >= shamt; --i)
			res.bits[i] = bits[i-shamt];
		for(int i = shamt-1; i>=0; --i)
			res.bits[i] = false;
	}
	return res;
}

inline Number Number::operator>>(int shamt) const {
	Number res(*this);
	if(shamt >size()) {
		for(int i = 0; i < size(); ++i)
			res.bits[i] = false;
	}
	else {
		for(int i = shamt; i < size(); ++i)
			res.bits[i-shamt] = bits[i];
		for(int i = size()-shamt; i < size(); ++i)
			res.bits[i] = false;
	}
	return res;
}

inline Number Number::operator<<(const Number& shamt) const {
	Number res(*this);
	for(int i = 0; i < min(int(ceil(log2(size()))) , shamt.size()-1); ++i)
		res = res.select(shamt[i], res<<(1<<i));
	return res;
}

inline Number Number::operator>>(const Number& shamt) const{
	Number res(*this);
	for(int i = 0; i <min(int(ceil(log2(size()))) , shamt.size()-1); ++i)
		res = res.select(shamt[i], res>>(1<<i));
	return res;
}

//Comparisons
inline Wire Number::geq (const Number& rhs) const {
	assert(size() == rhs.size());
/*	Wire res(false);
	for(int i = 0; i < size(); ++i) {
		res = ((bits[i]^res)&(rhs[i]^res))^bits[i];
	} 
	return res; 
*/
	Number tmp = (*this) - rhs;
	return !tmp[tmp.size()-1];
}

inline Wire Number::equal(const Number& rhs) const {
	assert(size() == rhs.size());
	Wire res(true);
	for(int i = 0; i < size(); ++i)
		res = res & (bits[i] == rhs[i]);
	return res;
}

/* Arithmethics
 */
inline Number Number::operator+(const Number & rhs) const {
	assert(size() == rhs.size());
	Number res(*this);
	add_full(res.bits, nullptr, bits, rhs.bits, nullptr, size());
	return res;
}

inline Number Number::operator-(const Number& rhs) const {
	assert(size() == rhs.size());
	Number res(*this);
	sub_full(res.bits, nullptr, bits, rhs.bits, nullptr, size());
	return res;
}


inline Number Number::operator*(const Number& rhs) const {
	assert(size() == rhs.size());
	Number res(*this);
	mul_full(res.bits, bits, rhs.bits, size());
	return res;
}

inline Number Number::operator/(const Number& rhs) const {
	assert(size() == rhs.size());
	Number res(*this);
	Number i1 = abs();
	Number i2 = rhs.abs();
	Wire sign = bits[size()-1] ^ rhs[size()-1];
	div_full(res.bits, nullptr, i1.bits, i2.bits, size());
	condNeg(sign, res.bits, res.bits, size());
	return res;
}
inline Number Number::operator%(const Number& rhs) const {
	assert(size() == rhs.size());
	Number res(*this);
	Number i1 = abs();
	Number i2 = rhs.abs();
	Wire sign = bits[size()-1];
	div_full(nullptr, res.bits, i1.bits, i2.bits, size());
	condNeg(sign, res.bits, res.bits, size());
	return res;
}

inline Number Number::operator-() const {
	return Number(size(), 0, PUBLIC)-(*this);
}

//Others
inline Number Number::leading_zeros() const {
	Number res = *this;
	for(int i = size() - 2; i>=0; --i)
		res[i] = res[i+1] | res[i];

	for(int i = 0; i < res.size(); ++i)
		res[i] = !res[i];
	return res.hamming_weight();
}

inline Number Number::hamming_weight() const {
	vector<Number> vec;
	for(int i = 0; i < size(); i++) {
		Number tmp(2, 0, PUBLIC);
		tmp[0] = bits[i];
		vec.push_back(tmp);
	}

	while(vec.size() > 1) {
		int j = 0;
		for(size_t i = 0; i < vec.size()-1; i+=2) {
			vec[j++] = vec[i]+vec[i+1];
		}
		if(vec.size()%2 == 1) {
			vec[j++] = vec[vec.size()-1];
		}
		for(int i = 0; i < j; ++i)
			vec[i].resize(vec[i].size()+1, false);
		int vec_size = vec.size();
		for(int i = j; i < vec_size; ++i)
			vec.pop_back();
	}
	return vec[0];
}
inline Number Number::modExp(Number p, Number q) {
	// the value of q should be less than half of the MAX_INT
	Number base = *this;
	Number res(size(),1);
	for(int i = 0; i < p.size(); ++i) {
		Number tmp = (res * base) % q;
		res = res.select(p[i], tmp);
		base = (base*base) % q; 
	}
	return res;
}
