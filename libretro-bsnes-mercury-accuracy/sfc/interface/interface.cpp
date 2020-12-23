#include <sfc/sfc.hpp>

namespace SuperFamicom {

Interface* interface = nullptr;

string Interface::title() {
  return cartridge.title();
}

double Interface::videoFrequency() {
  switch(system.region()) { default:
  case System::Region::NTSC: return system.cpu_frequency() / (262.0 * 1364.0 - 4.0);
  case System::Region::PAL:  return system.cpu_frequency() / (312.0 * 1364.0);
  }
}

double Interface::audioFrequency() {
  return system.apu_frequency() / 768.0;
}

bool Interface::loaded() {
  return cartridge.loaded();
}

string Interface::sha256() {
  return cartridge.sha256();
}

unsigned Interface::group(unsigned id) {
  switch(id) {
  case ID::IPLROM:
    return 0;
  case ID::Manifest:
  case ID::ROM:
  case ID::RAM:
  case ID::EventROM0:
  case ID::EventROM1:
  case ID::EventROM2:
  case ID::EventROM3:
  case ID::EventRAM:
  case ID::SA1ROM:
  case ID::SA1IRAM:
  case ID::SA1BWRAM:
  case ID::SuperFXROM:
  case ID::SuperFXRAM:
  case ID::ArmDSPPROM:
  case ID::ArmDSPDROM:
  case ID::ArmDSPRAM:
  case ID::HitachiDSPROM:
  case ID::HitachiDSPRAM:
  case ID::HitachiDSPDROM:
  case ID::HitachiDSPDRAM:
  case ID::Nec7725DSPPROM:
  case ID::Nec7725DSPDROM:
  case ID::Nec7725DSPRAM:
  case ID::Nec96050DSPPROM:
  case ID::Nec96050DSPDROM:
  case ID::Nec96050DSPRAM:
  case ID::EpsonRTC:
  case ID::SharpRTC:
  case ID::SPC7110PROM:
  case ID::SPC7110DROM:
  case ID::SPC7110RAM:
  case ID::SDD1ROM:
  case ID::SDD1RAM:
  case ID::OBC1RAM:
  case ID::SuperGameBoyBootROM:
  case ID::BsxROM:
  case ID::BsxRAM:
  case ID::BsxPSRAM:
    return 1;
  case ID::SuperGameBoy:
  case ID::SuperGameBoyManifest:
  case ID::SuperGameBoyROM:
  case ID::SuperGameBoyRAM:
    return 2;
  case ID::Satellaview:
  case ID::SatellaviewManifest:
  case ID::SatellaviewROM:
    return 3;
  case ID::SufamiTurboSlotA:
  case ID::SufamiTurboSlotAManifest:
  case ID::SufamiTurboSlotAROM:
  case ID::SufamiTurboSlotARAM:
    return 4;
  case ID::SufamiTurboSlotB:
  case ID::SufamiTurboSlotBManifest:
  case ID::SufamiTurboSlotBROM:
  case ID::SufamiTurboSlotBRAM:
    return 5;
  }

  throw;
}

void Interface::load(unsigned id) {
  if(id == ID::SuperFamicom) cartridge.load();
  if(id == ID::SuperGameBoy) cartridge.load_super_game_boy();
  if(id == ID::Satellaview) cartridge.load_satellaview();
  if(id == ID::SufamiTurboSlotA) cartridge.load_sufami_turbo_a();
  if(id == ID::SufamiTurboSlotB) cartridge.load_sufami_turbo_b();
}

void Interface::save() {
  for(auto& memory : cartridge.memory) {
    saveRequest(memory.id, memory.name);
  }
}

void Interface::load(unsigned id, const stream& stream) {
  if(id == ID::IPLROM) {
    stream.read(smp.iplrom, min(64u, stream.size()));
  }

  if(id == ID::Manifest) cartridge.information.markup.cartridge = stream.text();
  if(id == ID::ROM) cartridge.rom.read(stream);
  if(id == ID::RAM) cartridge.ram.read(stream);

  if(id == ID::EventROM0) event.rom[0].read(stream);
  if(id == ID::EventROM1) event.rom[1].read(stream);
  if(id == ID::EventROM2) event.rom[2].read(stream);
  if(id == ID::EventROM3) event.rom[3].read(stream);
  if(id == ID::EventRAM) event.ram.read(stream);

  if(id == ID::SA1ROM) sa1.rom.read(stream);
  if(id == ID::SA1IRAM) sa1.iram.read(stream);
  if(id == ID::SA1BWRAM) sa1.bwram.read(stream);

  if(id == ID::SuperFXROM) superfx.rom.read(stream);
  if(id == ID::SuperFXRAM) superfx.ram.read(stream);

  if(id == ID::ArmDSPPROM) {
    for(unsigned n = 0; n < 128 * 1024; n++) armdsp.programROM[n] = stream.read();
  }
  if(id == ID::ArmDSPDROM) {
    for(unsigned n = 0; n <  32 * 1024; n++) armdsp.dataROM[n] = stream.read();
  }
  if(id == ID::ArmDSPRAM) {
    for(unsigned n = 0; n <  16 * 1024; n++) armdsp.programRAM[n] = stream.read();
  }

  if(id == ID::HitachiDSPROM) hitachidsp.rom.read(stream);
  if(id == ID::HitachiDSPRAM) hitachidsp.ram.read(stream);
  if(id == ID::HitachiDSPDROM) {
    for(unsigned n = 0; n < 1024; n++) hitachidsp.dataROM[n] = stream.readl(3);
  }
  if(id == ID::HitachiDSPDRAM) {
    for(unsigned n = 0; n < 3072; n++) hitachidsp.dataRAM[n] = stream.readl(1);
  }

  if(id == ID::Nec7725DSPPROM) {
    for(unsigned n = 0; n <  2048; n++) necdsp.programROM[n] = stream.readl(3);
  }
  if(id == ID::Nec7725DSPDROM) {
    for(unsigned n = 0; n <  1024; n++) necdsp.dataROM[n]    = stream.readl(2);
  }
  if(id == ID::Nec7725DSPRAM) {
    for(unsigned n = 0; n <   256; n++) necdsp.dataRAM[n]    = stream.readl(2);
  }
  if(id == ID::Nec96050DSPPROM) {
    for(unsigned n = 0; n < 16384; n++) necdsp.programROM[n] = stream.readl(3);
  }
  if(id == ID::Nec96050DSPDROM) {
    for(unsigned n = 0; n <  2048; n++) necdsp.dataROM[n]    = stream.readl(2);
  }
  if(id == ID::Nec96050DSPRAM) {
    for(unsigned n = 0; n <  2048; n++) necdsp.dataRAM[n]    = stream.readl(2);
  }

  if(id == ID::EpsonRTC) {
    uint8 data[16] = {0};
    stream.read(data, min(stream.size(), sizeof data));
    epsonrtc.load(data);
  }

  if(id == ID::SharpRTC) {
    uint8 data[16] = {0};
    stream.read(data, min(stream.size(), sizeof data));
    sharprtc.load(data);
  }

  if(id == ID::SPC7110PROM) spc7110.prom.read(stream);
  if(id == ID::SPC7110DROM) spc7110.drom.read(stream);
  if(id == ID::SPC7110RAM) spc7110.ram.read(stream);

  if(id == ID::SDD1ROM) sdd1.rom.read(stream);
  if(id == ID::SDD1RAM) sdd1.ram.read(stream);

  if(id == ID::OBC1RAM) obc1.ram.read(stream);

  if(id == ID::SuperGameBoyBootROM) {
    stream.read(GameBoy::system.bootROM.sgb, min(stream.size(), 256u));
  }

  if(id == ID::BsxROM) bsxcartridge.rom.read(stream);
  if(id == ID::BsxRAM) bsxcartridge.ram.read(stream);
  if(id == ID::BsxPSRAM) bsxcartridge.psram.read(stream);

  if(id == ID::SuperGameBoyManifest) cartridge.information.markup.gameBoy = stream.text();

  if(id == ID::SuperGameBoyROM) {
    stream.read(GameBoy::cartridge.romdata, min(GameBoy::cartridge.romsize, stream.size()));
  }

  if(id == ID::SuperGameBoyRAM) {
    stream.read(GameBoy::cartridge.ramdata, min(GameBoy::cartridge.ramsize, stream.size()));
  }

  if(id == ID::SatellaviewManifest) cartridge.information.markup.satellaview = stream.text();
  if(id == ID::SatellaviewROM) satellaviewcartridge.memory.read(stream);

  if(id == ID::SufamiTurboSlotAManifest) cartridge.information.markup.sufamiTurboA = stream.text();
  if(id == ID::SufamiTurboSlotAROM) sufamiturboA.rom.read(stream);
  if(id == ID::SufamiTurboSlotBROM) sufamiturboB.rom.read(stream);

  if(id == ID::SufamiTurboSlotBManifest) cartridge.information.markup.sufamiTurboB = stream.text();
  if(id == ID::SufamiTurboSlotARAM) sufamiturboA.ram.read(stream);
  if(id == ID::SufamiTurboSlotBRAM) sufamiturboB.ram.read(stream);
}

void Interface::save(unsigned id, const stream& stream) {
  if(id == ID::RAM) stream.write(cartridge.ram.data(), cartridge.ram.size());
  if(id == ID::EventRAM) stream.write(event.ram.data(), event.ram.size());
  if(id == ID::SA1IRAM) stream.write(sa1.iram.data(), sa1.iram.size());
  if(id == ID::SA1BWRAM) stream.write(sa1.bwram.data(), sa1.bwram.size());
  if(id == ID::SuperFXRAM) stream.write(superfx.ram.data(), superfx.ram.size());

  if(id == ID::ArmDSPRAM) {
    for(unsigned n = 0; n < 16 * 1024; n++) stream.write(armdsp.programRAM[n]);
  }

  if(id == ID::HitachiDSPRAM) stream.write(hitachidsp.ram.data(), hitachidsp.ram.size());
  if(id == ID::HitachiDSPDRAM) {
    for(unsigned n = 0; n < 3072; n++) stream.writel(hitachidsp.dataRAM[n], 1);
  }

  if(id == ID::Nec7725DSPRAM) {
    for(unsigned n = 0; n <  256; n++) stream.writel(necdsp.dataRAM[n], 2);
  }
  if(id == ID::Nec96050DSPRAM) {
    for(unsigned n = 0; n < 2048; n++) stream.writel(necdsp.dataRAM[n], 2);
  }

  if(id == ID::EpsonRTC) {
    uint8 data[16] = {0};
    epsonrtc.save(data);
    stream.write(data, sizeof data);
  }

  if(id == ID::SharpRTC) {
    uint8 data[16] = {0};
    sharprtc.save(data);
    stream.write(data, sizeof data);
  }

  if(id == ID::SPC7110RAM) stream.write(spc7110.ram.data(), spc7110.ram.size());
  if(id == ID::SDD1RAM) stream.write(sdd1.ram.data(), sdd1.ram.size());
  if(id == ID::OBC1RAM) stream.write(obc1.ram.data(), obc1.ram.size());

  if(id == ID::SuperGameBoyRAM) stream.write(GameBoy::cartridge.ramdata, GameBoy::cartridge.ramsize);

  if(id == ID::BsxRAM) stream.write(bsxcartridge.ram.data(), bsxcartridge.ram.size());
  if(id == ID::BsxPSRAM) stream.write(bsxcartridge.psram.data(), bsxcartridge.psram.size());

  if(id == ID::SufamiTurboSlotARAM) stream.write(sufamiturboA.ram.data(), sufamiturboA.ram.size());
  if(id == ID::SufamiTurboSlotBRAM) stream.write(sufamiturboB.ram.data(), sufamiturboB.ram.size());
}

void Interface::unload() {
  save();
  cartridge.unload();
  tracerEnable(false);
}

void Interface::connect(unsigned port, unsigned device) {
  input.connect(port, (Input::Device)device);
}

void Interface::power() {
  system.power();
}

void Interface::reset() {
  system.reset();
}

void Interface::run() {
  system.run();
}

bool Interface::rtc() {
  if(cartridge.has_epsonrtc()) return true;
  if(cartridge.has_sharprtc()) return true;
  return false;
}

void Interface::rtcsync() {
  if(cartridge.has_epsonrtc()) epsonrtc.sync();
  if(cartridge.has_sharprtc()) sharprtc.sync();
}

serializer Interface::serialize() {
  system.runtosave();
  return system.serialize();
}

bool Interface::unserialize(serializer& s) {
  return system.unserialize(s);
}

void Interface::cheatSet(const lstring& list) {
  cheat.reset();

  //Super Game Boy
  if(cartridge.has_gb_slot()) {
    GameBoy::cheat.reset();
    for(auto& codeset : list) {
      lstring codes = codeset.split("+");
      for(auto& code : codes) {
        lstring part = code.split("/");
        if(part.size() == 2) GameBoy::cheat.append(hex(part[0]), hex(part[1]));
        if(part.size() == 3) GameBoy::cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
      }
    }
    return;
  }

  //Super Famicom, Broadcast Satellaview, Sufami Turbo
  for(auto& codeset : list) {
    lstring codes = codeset.split("+");
    for(auto& code : codes) {
      lstring part = code.split("/");
      if(part.size() == 2) cheat.append(hex(part[0]), hex(part[1]));
      if(part.size() == 3) cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
    }
  }
}

void Interface::paletteUpdate(PaletteMode mode) {
  video.generate_palette(mode);
}

bool Interface::tracerEnable(bool trace) {
  string pathname = {path(group(ID::ROM)), "debug/"};
  if(trace == true) directory::create(pathname);

  if(trace == true && !tracer.open()) {
    for(unsigned n = 0; n <= 999; n++) {
      string filename = {pathname, "trace-", format<3, '0'>(n), ".log"};
      if(file::exists(filename)) continue;
      tracer.open(filename, file::mode::write);
      return true;
    }
  }

  if(trace == false && tracer.open()) {
    tracer.close();
    return true;
  }

  return false;
}

void Interface::exportMemory() {
  string pathname = {path(group(ID::ROM)), "debug/"};
  directory::create(pathname);

  file::write({pathname, "work.ram"}, cpu.wram, 128 * 1024);
  file::write({pathname, "video.ram"}, ppu.vram, 64 * 1024);
  file::write({pathname, "sprite.ram"}, ppu.oam, 544);
  file::write({pathname, "palette.ram"}, ppu.cgram, 512);
  file::write({pathname, "apu.ram"}, smp.apuram, 64 * 1024);
}

Interface::Interface() {
  interface = this;
  system.init();

  information.name        = "Super Famicom";
  information.width       = 256;
  information.height      = 240;
  information.overscan    = true;
  information.aspectRatio = 8.0 / 7.0;
  information.resettable  = true;
  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::SuperFamicom, "Super Famicom",    "sfc", true });
  media.append({ID::SuperFamicom, "Game Boy",         "gb",  false});
  media.append({ID::SuperFamicom, "BS-X Satellaview", "bs",  false});
  media.append({ID::SuperFamicom, "Sufami Turbo",     "st",  false});

  {
    Device device{0, ID::Port1 | ID::Port2, "Controller"};
    device.input.append({ 0, 0, "B"     });
    device.input.append({ 1, 0, "Y"     });
    device.input.append({ 2, 0, "Select"});
    device.input.append({ 3, 0, "Start" });
    device.input.append({ 4, 0, "Up"    });
    device.input.append({ 5, 0, "Down"  });
    device.input.append({ 6, 0, "Left"  });
    device.input.append({ 7, 0, "Right" });
    device.input.append({ 8, 0, "A"     });
    device.input.append({ 9, 0, "X"     });
    device.input.append({10, 0, "L"     });
    device.input.append({11, 0, "R"     });
    device.order = {4, 5, 6, 7, 0, 8, 1, 9, 10, 11, 2, 3};
    this->device.append(device);
  }

  {
    Device device{1, ID::Port1 | ID::Port2, "Multitap"};
    for(unsigned p = 1, n = 0; p <= 4; p++, n += 12) {
      device.input.append({n +  0, 0, {"Port ", p, " - ", "B"     }});
      device.input.append({n +  1, 0, {"Port ", p, " - ", "Y"     }});
      device.input.append({n +  2, 0, {"Port ", p, " - ", "Select"}});
      device.input.append({n +  3, 0, {"Port ", p, " - ", "Start" }});
      device.input.append({n +  4, 0, {"Port ", p, " - ", "Up"    }});
      device.input.append({n +  5, 0, {"Port ", p, " - ", "Down"  }});
      device.input.append({n +  6, 0, {"Port ", p, " - ", "Left"  }});
      device.input.append({n +  7, 0, {"Port ", p, " - ", "Right" }});
      device.input.append({n +  8, 0, {"Port ", p, " - ", "A"     }});
      device.input.append({n +  9, 0, {"Port ", p, " - ", "X"     }});
      device.input.append({n + 10, 0, {"Port ", p, " - ", "L"     }});
      device.input.append({n + 11, 0, {"Port ", p, " - ", "R"     }});
      device.order.append(n + 4, n + 5, n +  6, n +  7, n + 0, n + 8);
      device.order.append(n + 1, n + 9, n + 10, n + 11, n + 2, n + 3);
    }
    this->device.append(device);
  }

  {
    Device device{2, ID::Port1 | ID::Port2, "Mouse"};
    device.input.append({0, 1, "X-axis"});
    device.input.append({1, 1, "Y-axis"});
    device.input.append({2, 0, "Left"  });
    device.input.append({3, 0, "Right" });
    device.order = {0, 1, 2, 3};
    this->device.append(device);
  }

  {
    Device device{3, ID::Port2, "Super Scope"};
    device.input.append({0, 1, "X-axis" });
    device.input.append({1, 1, "Y-axis" });
    device.input.append({2, 0, "Trigger"});
    device.input.append({3, 0, "Cursor" });
    device.input.append({4, 0, "Turbo"  });
    device.input.append({5, 0, "Pause"  });
    device.order = {0, 1, 2, 3, 4, 5};
    this->device.append(device);
  }

  {
    Device device{4, ID::Port2, "Justifier"};
    device.input.append({0, 1, "X-axis" });
    device.input.append({1, 1, "Y-axis" });
    device.input.append({2, 0, "Trigger"});
    device.input.append({3, 0, "Start"  });
    device.order = {0, 1, 2, 3};
    this->device.append(device);
  }

  {
    Device device{5, ID::Port2, "Justifiers"};
    device.input.append({0, 1, "Port 1 - X-axis" });
    device.input.append({1, 1, "Port 1 - Y-axis" });
    device.input.append({2, 0, "Port 1 - Trigger"});
    device.input.append({3, 0, "Port 1 - Start"  });
    device.order.append(0, 1, 2, 3);
    device.input.append({4, 1, "Port 2 - X-axis" });
    device.input.append({5, 1, "Port 2 - Y-axis" });
    device.input.append({6, 0, "Port 2 - Trigger"});
    device.input.append({7, 0, "Port 2 - Start"  });
    device.order.append(4, 5, 6, 7);
    this->device.append(device);
  }

  {
    Device device{6, ID::Port1, "Serial USART"};
    this->device.append(device);
  }

  {
    Device device{7, ID::Port1 | ID::Port2, "None"};
    this->device.append(device);
  }

  port.append({0, "Port 1"});
  port.append({1, "Port 2"});

  for(auto& device : this->device) {
    for(auto& port : this->port) {
      if(device.portmask & (1 << port.id)) {
        port.device.append(device);
      }
    }
  }
}

}
