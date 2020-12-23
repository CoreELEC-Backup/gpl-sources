serializer System::serialize() {
  serializer s(serialize_size);

  unsigned signature = 0x31545342, version = Info::SerializerVersion, crc32 = 0;
  char description[512];
  memset(&description, 0, sizeof description);

  s.integer(signature);
  s.integer(version);
  s.integer(crc32);
  s.array(description);

  serialize_all(s);
  return s;
}

bool System::unserialize(serializer &s) {
  unsigned signature, version, crc32;
  char description[512];

  s.integer(signature);
  s.integer(version);
  s.integer(crc32);
  s.array(description);

  if(signature != 0x31545342) return false;
  if(version != Info::SerializerVersion) return false;
//if(crc32 != 0) return false;

  reset();
  serialize_all(s);
  return true;
}

void System::serialize(serializer &s) {
}

void System::serialize_all(serializer &s) {
  system.serialize(s);
  input.serialize(s);
  cartridge.serialize(s);
  cpu.serialize(s);
  apu.serialize(s);
  ppu.serialize(s);
}

void System::serialize_init() {
  serializer s;

  unsigned signature = 0, version = 0, crc32 = 0;
  char description[512];

  s.integer(signature);
  s.integer(version);
  s.integer(crc32);
  s.array(description);

  serialize_all(s);
  serialize_size = s.size();
}
