
inline uint16_t modulo(uint32_t a,uint32_t b) {
	asm("divu.w %1, %0" : "+d" (a) : "d" (b));
	return (uint16_t)(a>>16);
}

inline uint32_t div_mod(uint32_t a,uint32_t b) { // dividend in lower word, modulo in upper
	asm("divu.w %1, %0" : "+d" (a) : "d" (b));
	return a;
}

inline uint32_t mul16(uint32_t a,uint32_t b) {
	asm volatile("mulu.w %1,%0" : "+d"(b) : "d"(a));
	return b;
}

inline uint32_t mul32(uint32_t ab,uint32_t cd) {
	// AB * CD = (AC << 32) + ((B*C + A*D)<<16) + B*D
	return ((mul16((uint16_t)ab,(uint16_t)(cd>>16)) + mul16((uint16_t)(ab>>16),(uint16_t)cd)) << 16) + mul16((uint16_t)ab,(uint16_t)cd);
}

