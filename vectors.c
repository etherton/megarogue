
extern void _start(), crash(), interrupt_v(), interrupt_h();

const unsigned vectors[] = {
	0x1000000, (unsigned)_start, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,

	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)interrupt_h, (unsigned)crash, (unsigned)interrupt_v, (unsigned)crash,

	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,

	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
	(unsigned)crash, (unsigned)crash, (unsigned)crash, (unsigned)crash,
};
