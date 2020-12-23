struct Interface : Emulator::Interface::Bind {
  void loadRequest(unsigned id, string name, string type);
  void loadRequest(unsigned id, string path);
  void saveRequest(unsigned id, string path);
  uint32_t videoColor(unsigned source, uint16_t alpha, uint16_t red, uint16_t green, uint16_t blue);
  void videoRefresh(const uint32_t* palette, const uint32_t* data, unsigned pitch, unsigned width, unsigned height);
  void audioSample(int16_t lsample, int16_t rsample);
  int16_t inputPoll(unsigned port, unsigned device, unsigned input);
  void inputRumble(unsigned port, unsigned device, unsigned input, bool enable);
  unsigned dipSettings(const Markup::Node& node);
  string path(unsigned group);
  string server();
  void notify(string text);
};

extern Interface* interface;
