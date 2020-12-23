#include <sfc/sfc.hpp>

#define DSP3_CPP
namespace SuperFamicom {

DSP3 dsp3;

namespace DSP3i {
  #define bool8 uint8
  #include "dsp3emu.c"
  #undef bool8
};

void DSP3::init() {
}

void DSP3::load() {
}

void DSP3::unload() {
}

void DSP3::power() {
}

void DSP3::reset() {
  DSP3i::DSP3_Reset();
}

uint8 DSP3::read(unsigned addr) {
  DSP3i::dsp3_address = addr & 0xffff;
  DSP3i::DSP3GetByte();
  return DSP3i::dsp3_byte;
}

void DSP3::write(unsigned addr, uint8 data) {
  DSP3i::dsp3_address = addr & 0xffff;
  DSP3i::dsp3_byte = data;
  DSP3i::DSP3SetByte();
}

}
