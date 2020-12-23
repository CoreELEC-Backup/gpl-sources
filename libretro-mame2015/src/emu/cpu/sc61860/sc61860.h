/*****************************************************************************
 *
 *   sc61860.h
 *   portable sharp 61860 emulator interface
 *   (sharp pocket computers)
 *
 *   Copyright Peter Trauner, all rights reserved.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     peter.trauner@jk.uni-linz.ac.at
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *****************************************************************************/

#pragma once

#ifndef __SC61860_H__
#define __SC61860_H__

/*
  official names seam to be
  ESR-H, ESR-J
  (ESR-L SC62015 ist complete different)
 */

/* unsolved problems
   the processor has 8 kbyte internal rom
   only readable with special instructions and program execution
   64 kb external ram (first 8kbyte not seen for program execution?) */


enum
{
	SC61860_PC=1, SC61860_DP,
	SC61860_P, SC61860_Q, SC61860_R,
	SC61860_CARRY,
	SC61860_ZERO,
	// the following are in the internal ram!
	SC61860_BA,
	SC61860_X, SC61860_Y,
	SC61860_I, SC61860_J, SC61860_K, SC61860_L, SC61860_V, SC61860_W,
	SC61860_H

//  SC61860_NMI_STATE,
//  SC61860_IRQ_STATE
};


#define MCFG_SC61860_READ_RESET_HANDLER(_devcb) \
	devcb = &sc61860_device::set_reset_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_READ_BRK_HANDLER(_devcb) \
	devcb = &sc61860_device::set_brk_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_READ_X_HANDLER(_devcb) \
	devcb = &sc61860_device::set_x_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_READ_A_HANDLER(_devcb) \
	devcb = &sc61860_device::set_ina_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_WRITE_A_HANDLER(_devcb) \
	devcb = &sc61860_device::set_outa_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_READ_B_HANDLER(_devcb) \
	devcb = &sc61860_device::set_inb_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_WRITE_B_HANDLER(_devcb) \
	devcb = &sc61860_device::set_outb_cb(*device, DEVCB_##_devcb);

#define MCFG_SC61860_WRITE_C_HANDLER(_devcb) \
	devcb = &sc61860_device::set_outc_cb(*device, DEVCB_##_devcb);

class sc61860_device : public cpu_device
{
public:
	// construction/destruction
	sc61860_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// static configuration helpers
	template<class _Object> static devcb_base &set_reset_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_reset.set_callback(object); }
	template<class _Object> static devcb_base &set_brk_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_brk.set_callback(object); }
	template<class _Object> static devcb_base &set_x_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_x.set_callback(object); }
	template<class _Object> static devcb_base &set_ina_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_ina.set_callback(object); }
	template<class _Object> static devcb_base &set_outa_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_outa.set_callback(object); }
	template<class _Object> static devcb_base &set_inb_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_inb.set_callback(object); }
	template<class _Object> static devcb_base &set_outb_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_outb.set_callback(object); }
	template<class _Object> static devcb_base &set_outc_cb(device_t &device, _Object object) { return downcast<sc61860_device &>(device).m_outc.set_callback(object); }

	/* this is though for power on/off of the sharps */
	UINT8 *internal_ram();

	TIMER_CALLBACK_MEMBER(sc61860_2ms_tick);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_execute_interface overrides
	virtual UINT32 execute_min_cycles() const { return 2; }
	virtual UINT32 execute_max_cycles() const { return 4; }
	virtual UINT32 execute_input_lines() const { return 0; }
	virtual void execute_run();

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const { return (spacenum == AS_PROGRAM) ? &m_program_config : NULL; }

	// device_state_interface overrides
	virtual void state_import(const device_state_entry &entry);
	virtual void state_export(const device_state_entry &entry);
	void state_string_export(const device_state_entry &entry, astring &string);

	// device_disasm_interface overrides
	virtual UINT32 disasm_min_opcode_bytes() const { return 1; }
	virtual UINT32 disasm_max_opcode_bytes() const { return 4; }
	virtual offs_t disasm_disassemble(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, UINT32 options);

private:
	address_space_config m_program_config;

	devcb_read_line m_reset;
	devcb_read_line m_brk;
	devcb_read_line m_x;
	devcb_read8 m_ina;
	devcb_write8 m_outa;
	devcb_read8 m_inb;
	devcb_write8 m_outb;
	devcb_write8 m_outc;

	UINT8 m_p, m_q, m_r; //7 bits only?

	UINT8 m_c;        // port c, used for HLT.
	UINT8 m_d, m_h;
	UINT16 m_oldpc, m_pc, m_dp;

	int m_carry, m_zero;

	struct { int t2ms, t512ms; int count; } m_timer;

	address_space *m_program;
	direct_read_data *m_direct;
	int m_icount;
	UINT8 m_ram[0x100]; // internal special ram, should be 0x60, 0x100 to avoid memory corruption for now

	UINT32 m_debugger_temp;

	inline UINT8 READ_OP();
	inline UINT8 READ_OP_ARG();
	inline UINT16 READ_OP_ARG_WORD();
	inline UINT8 READ_BYTE(UINT16 adr);
	inline void WRITE_BYTE(UINT16 a, UINT8 v);
	inline UINT8 READ_RAM(int r);
	inline void WRITE_RAM(int r, UINT8 v);
	inline void PUSH(UINT8 v);
	inline UINT8 POP();
	inline void sc61860_load_imm(int r, UINT8 v);
	inline void sc61860_load();
	inline void sc61860_load_imm_p(UINT8 v);
	inline void sc61860_load_imm_q(UINT8 v);
	inline void sc61860_load_r();
	inline void sc61860_load_ext(int r);
	inline void sc61860_load_dp();
	inline void sc61860_load_dl();
	inline void sc61860_store_p();
	inline void sc61860_store_q();
	inline void sc61860_store_r();
	inline void sc61860_store_ext(int r);
	inline void sc61860_exam(int a, int b);
	inline void sc61860_test(int reg, UINT8 value);
	inline void sc61860_test_ext();
	inline void sc61860_and(int reg, UINT8 value);
	inline void sc61860_and_ext();
	inline void sc61860_or(int reg, UINT8 value);
	inline void sc61860_or_ext();
	inline void sc61860_rotate_right();
	inline void sc61860_rotate_left();
	inline void sc61860_swap();
	inline void sc61860_inc(int reg);
	inline void sc61860_inc_p();
	inline void sc61860_dec(int reg);
	inline void sc61860_dec_p();
	inline void sc61860_add(int reg, UINT8 value);
	inline void sc61860_add_carry();
	inline void sc61860_add_word();
	inline void sc61860_sub(int reg, UINT8 value);
	inline void sc61860_sub_carry();
	inline void sc61860_sub_word();
	inline void sc61860_cmp(int reg, UINT8 value);
	inline void sc61860_pop();
	inline void sc61860_push();
	inline void sc61860_prepare_table_call();
	inline void sc61860_execute_table_call();
	inline void sc61860_call(UINT16 adr);
	inline void sc61860_return();
	inline void sc61860_jump(int yes);
	inline void sc61860_jump_rel_plus(int yes);
	inline void sc61860_jump_rel_minus(int yes);
	inline void sc61860_loop();
	inline void sc61860_leave();
	inline void sc61860_wait();
	inline void sc61860_set_carry();
	inline void sc61860_reset_carry();
	inline void sc61860_out_a();
	inline void sc61860_out_b();
	inline void sc61860_out_f();
	inline void sc61860_out_c();
	inline void sc61860_in_a();
	inline void sc61860_in_b();
	inline void sc61860_test_special();
	inline void sc61860_add_bcd_a();
	inline void sc61860_add_bcd();
	inline void sc61860_sub_bcd_a();
	inline void sc61860_sub_bcd();
	inline void sc61860_shift_left_nibble();
	inline void sc61860_shift_right_nibble();
	inline void sc61860_inc_load_dp(int reg);
	inline void sc61860_dec_load_dp(int reg);
	inline void sc61860_inc_load_dp_load();
	inline void sc61860_dec_load_dp_load();
	inline void sc61860_inc_load_dp_store();
	inline void sc61860_dec_load_dp_store();
	inline void sc61860_fill();
	inline void sc61860_fill_ext();
	inline void sc61860_copy(int count);
	inline void sc61860_copy_ext(int count);
	inline void sc61860_copy_int(int count);
	inline void sc61860_exchange(int count);
	inline void sc61860_exchange_ext(int count);
	inline void sc61860_wait_x(int level);
	void sc61860_instruction();

};


extern const device_type SC61860;


#endif /* __SC61860_H__ */
