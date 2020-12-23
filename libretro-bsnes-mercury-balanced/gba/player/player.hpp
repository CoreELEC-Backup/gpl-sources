struct Player {
  struct Status {
    bool enable;
    bool rumble;

    bool logoDetected;
    unsigned logoCounter;

    unsigned packet;
    uint32 send;
    uint32 recv;
  } status;

  void power();
  void frame();

  optional<uint16> keyinput();
  optional<uint32> read();
  void write(uint8 byte, uint2 addr);

  void serialize(serializer& s);
};

extern Player player;
