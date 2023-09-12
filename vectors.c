
extern void _start(), crash(), interrupt_v(), interrupt_h();
extern void bus_error(), address_error(), illegal_inst(), div_zero(), chk_exception(), trapv_exception();

const unsigned vectors[] = {
	0x1000000, (unsigned)_start, (unsigned)bus_error, (unsigned)address_error,
	(unsigned)illegal_inst, (unsigned)div_zero, (unsigned)chk_exception, (unsigned)trapv_exception,
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
