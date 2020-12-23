#include <fc/fc.hpp>

namespace Famicom {

Interface* interface = nullptr;

string Interface::title() {
  return cartridge.title();
}

double Interface::videoFrequency() {
  return 21477272.0 / (262.0 * 1364.0 - 4.0);
}

double Interface::audioFrequency() {
  return 21477272.0 / 12.0;
}

bool Interface::loaded() {
  return cartridge.loaded();
}

string Interface::sha256() {
  return cartridge.sha256();
}

unsigned Interface::group(unsigned id) {
  switch(id) {
  case ID::Manifest:
  case ID::ProgramROM:
  case ID::ProgramRAM:
  case ID::CharacterROM:
  case ID::CharacterRAM:
    return 1;
  }

  throw;
}

void Interface::load(unsigned id) {
  cartridge.load();
}

void Interface::save() {
  for(auto& memory : cartridge.memory) {
    saveRequest(memory.id, memory.name);
  }
}

void Interface::load(unsigned id, const stream& stream) {
  if(id == ID::Manifest) cartridge.information.markup = stream.text();

  if(id == ID::ProgramROM) {
    stream.read(cartridge.board->prgrom.data, min(cartridge.board->prgrom.size, stream.size()));
  }

  if(id == ID::ProgramRAM) {
    stream.read(cartridge.board->prgram.data, min(cartridge.board->prgram.size, stream.size()));
  }

  if(id == ID::CharacterROM) {
    stream.read(cartridge.board->chrrom.data, min(cartridge.board->chrrom.size, stream.size()));
  }

  if(id == ID::CharacterRAM) {
    stream.read(cartridge.board->chrram.data, min(cartridge.board->chrram.size, stream.size()));
  }
}

void Interface::save(unsigned id, const stream& stream) {
  if(id == ID::ProgramRAM) {
    stream.write(cartridge.board->prgram.data, cartridge.board->prgram.size);
  }

  if(id == ID::CharacterRAM) {
    stream.write(cartridge.board->chrram.data, cartridge.board->chrram.size);
  }
}

void Interface::unload() {
  save();
  cartridge.unload();
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

serializer Interface::serialize() {
  system.runtosave();
  return system.serialize();
}

bool Interface::unserialize(serializer& s) {
  return system.unserialize(s);
}

void Interface::cheatSet(const lstring& list) {
  cheat.reset();
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

Interface::Interface() {
  interface = this;

  information.name        = "Famicom";
  information.width       = 256;
  information.height      = 240;
  information.overscan    = true;
  information.aspectRatio = 8.0 / 7.0;
  information.resettable  = true;
  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::Famicom, "Famicom", "fc", true});

  {
    Device device{0, ID::Port1 | ID::Port2, "Controller"};
    device.input.append({0, 0, "A"     });
    device.input.append({1, 0, "B"     });
    device.input.append({2, 0, "Select"});
    device.input.append({3, 0, "Start" });
    device.input.append({4, 0, "Up"    });
    device.input.append({5, 0, "Down"  });
    device.input.append({6, 0, "Left"  });
    device.input.append({7, 0, "Right" });
    device.order = {4, 5, 6, 7, 1, 0, 2, 3};
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
