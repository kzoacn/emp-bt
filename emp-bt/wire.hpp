inline Wire::Wire(bool b, int party) {
	if (party == PUBLIC)
		bit = Circuit_Gen::circ_exec->public_label(b);
	else{
		//  b must be 0 if this is not your
		bit = Circuit_Gen::circ_exec->set_input(b,party);
	}  
}

inline Wire Wire::select(const Wire & select, const Wire & new_v) const{
	Wire tmp = *this;
	tmp = tmp ^ new_v;
	tmp = tmp & select;
	return *this ^ tmp;
}
 
 

inline Wire Wire::operator==(const Wire& rhs) const {
	return !(*this ^ rhs);
}

inline Wire Wire::operator!=(const Wire& rhs) const {
	return (*this) ^ rhs;
}

inline Wire Wire::operator &(const Wire& rhs) const{
	Wire res;
	res.bit = Circuit_Gen::circ_exec->and_gate(bit, rhs.bit);
	return res;
}
inline Wire Wire::operator ^(const Wire& rhs) const{
	Wire res;
	res.bit = Circuit_Gen::circ_exec->xor_gate(bit, rhs.bit);
	return res;
}

inline Wire Wire::operator |(const Wire& rhs) const{
	return (*this ^ rhs) ^ (*this & rhs);
}

inline Wire Wire::operator!() const {
	Wire res;
	res.bit=Circuit_Gen::circ_exec->not_gate(bit);
	return res;
}
