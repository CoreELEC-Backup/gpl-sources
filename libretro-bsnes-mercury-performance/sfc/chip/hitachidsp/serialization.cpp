#ifdef HITACHIDSP_CPP

vector<uint8> HitachiDSP::firmware() {
  vector<uint8> buffer;
  if(cartridge.has_hitachidsp() == false) return buffer;
  buffer.reserve(1024 * 3);
  for(unsigned n = 0; n < 1024; n++) {
    buffer.append(dataROM[n] >>  0);
    buffer.append(dataROM[n] >>  8);
    buffer.append(dataROM[n] >> 16);
  }
  return buffer;
}

void HitachiDSP::serialize(serializer& s) {
  HG51B::serialize(s);
  Thread::serialize(s);

  s.integer(mmio.dma);
  s.integer(mmio.dma_source);
  s.integer(mmio.dma_length);
  s.integer(mmio.dma_target);
  s.integer(mmio.r1f48);
  s.integer(mmio.program_offset);
  s.integer(mmio.r1f4c);
  s.integer(mmio.page_number);
  s.integer(mmio.program_counter);
  s.integer(mmio.r1f50);
  s.integer(mmio.r1f51);
  s.integer(mmio.r1f52);
  s.array(mmio.vector);
}

#endif
