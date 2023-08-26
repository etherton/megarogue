
inline uint16_t modulo(uint32_t a,uint16_t b) {
	asm("divu.w %1, %0" : "+d" (a) : "d" (b));
	return (uint16_t)(a>>16);
}

inline uint32_t div_mod(uint32_t a,uint16_t b) { // dividend in upper word, modulo in lower
	asm("divu.w %1, %0" : "+d" (a) : "d" (b));
	return a;
}

inline uint32_t mul16(uint16_t a,uint32_t b) {
	asm volatile("mulu.w %1,%0" : "+d"(b) : "d"(a));
	return b;
}
