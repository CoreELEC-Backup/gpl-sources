#include <sfc/sfc.hpp>

#define SYSTEM_CPP
namespace SuperFamicom {

System system;
Configuration configuration;
Random random;

#include "video.cpp"
#include "audio.cpp"
#include "input.cpp"
#include "serialization.cpp"

#include <sfc/scheduler/scheduler.cpp>

void System::run() {
  scheduler.sync = Scheduler::SynchronizeMode::None;

  scheduler.enter();
  if(scheduler.exit_reason() == Scheduler::ExitReason::FrameEvent) {
    video.update();
  }
}

void System::runtosave() {
  if(CPU::Threaded == true) {
    scheduler.sync = Scheduler::SynchronizeMode::CPU;
    runthreadtosave();
  }

  if(SMP::Threaded == true) {
    scheduler.thread = smp.thread;
    runthreadtosave();
  }

  if(PPU::Threaded == true) {
    scheduler.thread = ppu.thread;
    runthreadtosave();
  }

  if(DSP::Threaded == true) {
    scheduler.thread = dsp.thread;
    runthreadtosave();
  }

  for(unsigned i = 0; i < cpu.coprocessors.size(); i++) {
    auto& chip = *cpu.coprocessors[i];
    scheduler.thread = chip.thread;
    runthreadtosave();
  }
}

void System::runthreadtosave() {
  while(true) {
    scheduler.enter();
    if(scheduler.exit_reason() == Scheduler::ExitReason::SynchronizeEvent) break;
    if(scheduler.exit_reason() == Scheduler::ExitReason::FrameEvent) {
      video.update();
    }
  }
}

void System::init() {
  assert(interface != nullptr);

  satellaviewbaseunit.init();
  icd2.init();
  bsxcartridge.init();
  nss.init();
  event.init();
  sa1.init();
  superfx.init();
  armdsp.init();
  hitachidsp.init();
  necdsp.init();
  epsonrtc.init();
  sharprtc.init();
  spc7110.init();
  sdd1.init();
  obc1.init();
  hsu1.init();
  msu1.init();
  satellaviewcartridge.init();

  dsp1.init();
  dsp2.init();
  dsp3.init();
  dsp4.init();
  cx4.init();
  st0010.init();
  sgbExternal.init();

  video.init();
  audio.init();

  input.connect(0, configuration.controller_port1);
  input.connect(1, configuration.controller_port2);
}

void System::term() {
}

void System::load() {
#ifdef __LIBRETRO__
  interface->loadRequest(ID::IPLROM, "");
#else
  string manifest = string::read({interface->path(ID::System), "manifest.bml"});
  auto document = Markup::Document(manifest);

  interface->loadRequest(ID::IPLROM, document["system/smp/rom/name"].data);
  if(!file::exists({interface->path(ID::System), document["system/smp/rom/name"].data})) {
    interface->notify("Error: required Super Famicom firmware ipl.rom not found.\n");
  }
#endif

  region = configuration.region;
  expansion = configuration.expansion_port;
  if(region == Region::Autodetect) {
    region = (cartridge.region() == Cartridge::Region::NTSC ? Region::NTSC : Region::PAL);
  }

  cpu_frequency = region() == Region::NTSC ? 21477272 : 21281370;
  apu_frequency = 24607104;

  audio.coprocessor_enable(false);

  bus.map_reset();
  bus.map_xml();

  cpu.enable();
  ppu.enable();

  if(expansion() == ExpansionPortDevice::Satellaview) satellaviewbaseunit.load();
  if(cartridge.has_gb_slot()) icd2.load();
  if(cartridge.has_bs_cart()) bsxcartridge.load();
  if(cartridge.has_nss_dip()) nss.load();
  if(cartridge.has_event()) event.load();
  if(cartridge.has_sa1()) sa1.load();
  if(cartridge.has_superfx()) superfx.load();
  if(cartridge.has_armdsp()) armdsp.load();
  if(cartridge.has_hitachidsp()) hitachidsp.load();
  if(cartridge.has_necdsp()) necdsp.load();
  if(cartridge.has_epsonrtc()) epsonrtc.load();
  if(cartridge.has_sharprtc()) sharprtc.load();
  if(cartridge.has_spc7110()) spc7110.load();
  if(cartridge.has_sdd1()) sdd1.load();
  if(cartridge.has_obc1()) obc1.load();
  if(cartridge.has_hsu1()) hsu1.load();
  if(cartridge.has_msu1()) msu1.load();
  if(cartridge.has_bs_slot()) satellaviewcartridge.load();
  if(cartridge.has_st_slots()) sufamiturboA.load(), sufamiturboB.load();
  if(cartridge.has_dsp1()) dsp1.load();
  if(cartridge.has_dsp2()) dsp2.load();
  if(cartridge.has_dsp3()) dsp3.load();
  if(cartridge.has_dsp4()) dsp4.load();
  if(cartridge.has_cx4()) cx4.load();
  if(cartridge.has_st0010()) st0010.load();
  if(cartridge.has_sgbexternal()) sgbExternal.load();

  serialize_init();
}

void System::unload() {
  if(expansion() == ExpansionPortDevice::Satellaview) satellaviewbaseunit.unload();
  if(cartridge.has_gb_slot()) icd2.unload();
  if(cartridge.has_bs_cart()) bsxcartridge.unload();
  if(cartridge.has_nss_dip()) nss.unload();
  if(cartridge.has_event()) event.unload();
  if(cartridge.has_sa1()) sa1.unload();
  if(cartridge.has_superfx()) superfx.unload();
  if(cartridge.has_armdsp()) armdsp.unload();
  if(cartridge.has_hitachidsp()) hitachidsp.unload();
  if(cartridge.has_necdsp()) necdsp.unload();
  if(cartridge.has_epsonrtc()) epsonrtc.unload();
  if(cartridge.has_sharprtc()) sharprtc.unload();
  if(cartridge.has_spc7110()) spc7110.unload();
  if(cartridge.has_sdd1()) sdd1.unload();
  if(cartridge.has_obc1()) obc1.unload();
  if(cartridge.has_hsu1()) hsu1.unload();
  if(cartridge.has_msu1()) msu1.unload();
  if(cartridge.has_bs_slot()) satellaviewcartridge.unload();
  if(cartridge.has_st_slots()) sufamiturboA.unload(), sufamiturboB.unload();

  if(cartridge.has_dsp1()) dsp1.unload();
  if(cartridge.has_dsp2()) dsp2.unload();
  if(cartridge.has_dsp3()) dsp3.unload();
  if(cartridge.has_dsp4()) dsp4.unload();
  if(cartridge.has_cx4()) cx4.unload();
  if(cartridge.has_st0010()) st0010.unload();
  if(cartridge.has_sgbexternal()) sgbExternal.unload();
}

void System::power() {
  random.seed((unsigned)time(0));

  cpu.power();
  smp.power();
  dsp.power();
  ppu.power();

  if(expansion() == ExpansionPortDevice::Satellaview) satellaviewbaseunit.power();
  if(cartridge.has_gb_slot()) icd2.power();
  if(cartridge.has_bs_cart()) bsxcartridge.power();
  if(cartridge.has_nss_dip()) nss.power();
  if(cartridge.has_event()) event.power();
  if(cartridge.has_sa1()) sa1.power();
  if(cartridge.has_superfx()) superfx.power();
  if(cartridge.has_armdsp()) armdsp.power();
  if(cartridge.has_hitachidsp()) hitachidsp.power();
  if(cartridge.has_necdsp()) necdsp.power();
  if(cartridge.has_epsonrtc()) epsonrtc.power();
  if(cartridge.has_sharprtc()) sharprtc.power();
  if(cartridge.has_spc7110()) spc7110.power();
  if(cartridge.has_sdd1()) sdd1.power();
  if(cartridge.has_obc1()) obc1.power();
  if(cartridge.has_hsu1()) hsu1.power();
  if(cartridge.has_msu1()) msu1.power();
  if(cartridge.has_bs_slot()) satellaviewcartridge.power();

  if(cartridge.has_dsp1()) dsp1.power();
  if(cartridge.has_dsp2()) dsp2.power();
  if(cartridge.has_dsp3()) dsp3.power();
  if(cartridge.has_dsp4()) dsp4.power();
  if(cartridge.has_cx4()) cx4.power();
  if(cartridge.has_st0010()) st0010.power();
  if(cartridge.has_sgbexternal()) sgbExternal.power();

  reset();
}

void System::reset() {
  cpu.reset();
  smp.reset();
  dsp.reset();
  ppu.reset();

  if(expansion() == ExpansionPortDevice::Satellaview) satellaviewbaseunit.reset();
  if(cartridge.has_gb_slot()) icd2.reset();
  if(cartridge.has_bs_cart()) bsxcartridge.reset();
  if(cartridge.has_nss_dip()) nss.reset();
  if(cartridge.has_event()) event.reset();
  if(cartridge.has_sa1()) sa1.reset();
  if(cartridge.has_superfx()) superfx.reset();
  if(cartridge.has_armdsp()) armdsp.reset();
  if(cartridge.has_hitachidsp()) hitachidsp.reset();
  if(cartridge.has_necdsp()) necdsp.reset();
  if(cartridge.has_epsonrtc()) epsonrtc.reset();
  if(cartridge.has_sharprtc()) sharprtc.reset();
  if(cartridge.has_spc7110()) spc7110.reset();
  if(cartridge.has_sdd1()) sdd1.reset();
  if(cartridge.has_obc1()) obc1.reset();
  if(cartridge.has_hsu1()) hsu1.reset();
  if(cartridge.has_msu1()) msu1.reset();
  if(cartridge.has_bs_slot()) satellaviewcartridge.reset();

  if(cartridge.has_gb_slot()) cpu.coprocessors.append(&icd2);
  if(cartridge.has_event()) cpu.coprocessors.append(&event);
  if(cartridge.has_sa1()) cpu.coprocessors.append(&sa1);
  if(cartridge.has_superfx()) cpu.coprocessors.append(&superfx);
  if(cartridge.has_armdsp()) cpu.coprocessors.append(&armdsp);
  if(cartridge.has_hitachidsp()) cpu.coprocessors.append(&hitachidsp);
  if(cartridge.has_necdsp()) cpu.coprocessors.append(&necdsp);
  if(cartridge.has_epsonrtc()) cpu.coprocessors.append(&epsonrtc);
  if(cartridge.has_sharprtc()) cpu.coprocessors.append(&sharprtc);
  if(cartridge.has_spc7110()) cpu.coprocessors.append(&spc7110);
  if(cartridge.has_msu1()) cpu.coprocessors.append(&msu1);

  if(cartridge.has_dsp1()) dsp1.reset();
  if(cartridge.has_dsp2()) dsp2.reset();
  if(cartridge.has_dsp3()) dsp3.reset();
  if(cartridge.has_dsp4()) dsp4.reset();
  if(cartridge.has_cx4()) cx4.reset();
  if(cartridge.has_st0010()) st0010.reset();
  if(cartridge.has_sgbexternal()) sgbExternal.reset();
  if(cartridge.has_sgbexternal()) cpu.coprocessors.append(&sgbExternal);

  scheduler.init();
  input.connect(0, configuration.controller_port1);
  input.connect(1, configuration.controller_port2);
}

void System::scanline(bool& frame_event_performed) {
  video.scanline();
  if(cpu.vcounter() == 241) {
    if(!frame_event_performed) {
      scheduler.exit(Scheduler::ExitReason::FrameEvent);
    }
    frame_event_performed = true;
  }
}

void System::frame() {
}

System::System() {
  region = Region::Autodetect;
  expansion = ExpansionPortDevice::Satellaview;
}

}
