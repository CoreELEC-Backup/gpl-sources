struct Cartridge : property<Cartridge> {
  enum class Region : unsigned {
    NTSC,
    PAL,
  };

  enum class Slot : unsigned {
    Base,
    Bsx,
    SufamiTurbo,
    SufamiTurboA,
    SufamiTurboB,
    GameBoy,
  };

  MappedRAM rom;
  MappedRAM ram;

  readonly<bool> loaded;
  readonly<string> sha256;

  readonly<Region> region;

  readonly<bool> has_gb_slot;
  readonly<bool> has_bs_cart;
  readonly<bool> has_bs_slot;
  readonly<bool> has_st_slots;
  readonly<bool> has_nss_dip;
  readonly<bool> has_event;
  readonly<bool> has_sa1;
  readonly<bool> has_superfx;
  readonly<bool> has_armdsp;
  readonly<bool> has_hitachidsp;
  readonly<bool> has_necdsp;
  readonly<bool> has_epsonrtc;
  readonly<bool> has_sharprtc;
  readonly<bool> has_spc7110;
  readonly<bool> has_sdd1;
  readonly<bool> has_obc1;
  readonly<bool> has_hsu1;
  readonly<bool> has_msu1;
  readonly<bool> has_dsp1;
  readonly<bool> has_dsp2;
  readonly<bool> has_dsp3;
  readonly<bool> has_dsp4;
  readonly<bool> has_cx4;
  readonly<bool> has_st0010;
  readonly<bool> has_sgbexternal;

  struct Mapping {
    function<uint8 (unsigned)> reader;
    function<void (unsigned, uint8)> writer;
    string addr;
    unsigned size;
    unsigned base;
    unsigned mask;

    enum fastmode_t { fastmode_slow, fastmode_readonly, fastmode_readwrite } fastmode;
    uint8* fastptr;

    Mapping();
    Mapping(const function<uint8 (unsigned)>&, const function<void (unsigned, uint8)>&);
    Mapping(SuperFamicom::Memory&);
  };
  vector<Mapping> mapping;

  struct Memory {
    unsigned id;
    string name;
  };
  vector<Memory> memory;

  struct Information {
    struct Markup {
      string cartridge;
      string gameBoy;
      string satellaview;
      string sufamiTurboA;
      string sufamiTurboB;
    } markup;

    struct Title {
      string cartridge;
      string gameBoy;
      string satellaview;
      string sufamiTurboA;
      string sufamiTurboB;
    } title;
  } information;

  string title();

  void load();
  void unload();

  void serialize(serializer&);
  Cartridge();
  ~Cartridge();

private:
  void load_super_game_boy();
  void load_satellaview();
  void load_sufami_turbo_a();
  void load_sufami_turbo_b();

  void parse_markup(const char*);
  void parse_markup_map(Mapping&, Markup::Node);
  void parse_markup_memory(MappedRAM&, Markup::Node, unsigned id, bool writable);

  void parse_markup_cartridge(Markup::Node);
  void parse_markup_icd2(Markup::Node);
  void parse_markup_bsx(Markup::Node);
  void parse_markup_satellaview(Markup::Node);
  void parse_markup_sufamiturbo(Markup::Node, bool slot);
  void parse_markup_nss(Markup::Node);
  void parse_markup_event(Markup::Node);
  void parse_markup_sa1(Markup::Node);
  void parse_markup_superfx(Markup::Node);
  void parse_markup_armdsp(Markup::Node);
  void parse_markup_hitachidsp(Markup::Node, unsigned roms);
  void parse_markup_necdsp(Markup::Node);
  void parse_markup_epsonrtc(Markup::Node);
  void parse_markup_sharprtc(Markup::Node);
  void parse_markup_spc7110(Markup::Node);
  void parse_markup_sdd1(Markup::Node);
  void parse_markup_obc1(Markup::Node);
  void parse_markup_hsu1(Markup::Node);
  void parse_markup_msu1(Markup::Node);
  
  void parse_markup_hitachidsp_hle(Markup::Node);
  void parse_markup_necdsp_hle(Markup::Node);
  bool parse_markup_icd2_external(Markup::Node);

  friend class Interface;
};

extern Cartridge cartridge;
