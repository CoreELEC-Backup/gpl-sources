# TXTH FORMAT

TXTH is a simple text file that uses text commands to simulate a header for files unsupported by vgmstream, mainly headerless audio.

When an unsupported file is loaded (for instance "bgm01.snd"), vgmstream tries to find a TXTH header in the same dir, in this order:
- `(filename.ext).txth`
- `.(ext).txth`
- `.txth`

If found and parsed correctly (the .txth may be rejected if incorrect commands are found) vgmstream will try to play the file as described. Extension must be accepted/added to vgmstream (plugins like foobar2000 only load extensions from a whitelist in formats.c), or one could rename to any supported extension (like .vgmstream), or leave the file extensionless.

You can also use `.(sub).(ext).txth` (if the file is `filename.sub.ext`), to allow mixing slightly different files in the same folder. The `sub` part doesn't need to be an extension, for example:
- `001.1ch.str`, `001.1ch.str` may use `.1ch.txth`
- `003.2ch.str`, `003.2ch.str` may use `.2ch.txth`
- etc

## Example of a TXTH file
For an unsupported `bgm01.vag` this would be a simple TXTH for it:
```
codec = PSX                 #data uses PS-ADPCM
sample_rate = @0x10$2       #get sample rate at offset 0x10, 16 bit value
channels = @0x14            #get number of channels at offset 14
interleave = 0x1000         #fixed value
start_offset = 0x100        #data starts after exactly this value
num_samples = data_size     #find automatically number of samples in the file
loop_flag = auto            #find loop points in PS-ADPCM
```
A text file with the above commands must be saved as `.vag.txth` or `.txth` (preferably the former), notice it starts with a "." (dot). On some Windows versions files starting with a dot need to be created by appending a dot at the end when renaming: `.txth.`

While the main point is playing the file, many of TXTH's features are aimed towards keeping original data intact, for documentation and preservation purposes; try leaving data as untouched as possible and consider how the game plays the file, as there is a good chance some feature can mimic it.


## Available commands

The file is made of lines with `key = value` commands describing a header. Commands are all case sensitive and spaces are optional: `key=value`, `key  =   value`, and so on are all ok. Comments start with # and can be inlined.

The parser is fairly simple and may be buggy or unexpected in some cases. The order of keys is variable but some things won't work if others aren't defined (ex. bytes-to-samples may not work without channels or interleave) or need to be done in a certain order (due to technical reasons) as explained below.

To get a file playing you need to correctly set, at least: `codec` and sometimes `interleave`, `sample_rate`, `channels` and `num_samples`, or use the "subfile" feature.

### VALUES
The following can be used in place of `(value)` for `(key) = (value)` commands.
- `(number)`: constant number in dec/hex, unsigned (no +10 or -10).
  * Examples: `44100, 40, 0x40 (decimal=64)`
- `(offset)`: read a value at offset inside the file, format being `@(number)[:LE|BE][$1|2|3|4]`
  * `@(number)`: offset of the value (required)
    * if `base_offset` is defined this value is modified (see later)
  * `:LE|BE`: value is little/big endian (optional, defaults to LE)
  * `$1|2|3|4`: value has size of 8/16/24/32 bit (optional, defaults to 4)
  * Example: `@0x10:BE$2` means `get big endian 16b value at 0x10`
- `(field)`: uses current value of some fields. Accepted strings:
  - `interleave, interleave_last, channels, sample_rate, start_offset, data_size, num_samples, loop_start_sample,  loop_end_sample, subsong_count, subsong_offset, subfile_offset, subfile_size, base_offset, name_valueX`
- `(other)`: other special values for certain keys, described per key


The above may be combined with math operations (+-*/&): `(key) = (number) (op) (offset) (op) (field) (...)`

### KEYS

#### CODEC [REQUIRED]
Sets codec used to encode the data. Some codecs need interleave or other config
as explained below, but often will use default values. Accepted codec strings:
```
# - PSX            PlayStation ADPCM
#   * For many PS1/PS2/PS3 games
#   * Interleave is multiple of 0x10 (default), often +0x1000
# - PSX_bf         PlayStation ADPCM with bad flags
#   * Variation with garbage data, for rare PS2 games
# - XBOX           Xbox IMA ADPCM (mono/stereo)
#   * For many XBOX games, and some PC games
#   * Special interleave is multiple of 0x24 (mono) or 0x48 (stereo)
# - DSP|NGC_DSP    Nintendo GameCube ADPCM
#   * For many GC/Wii/3DS games
#   * Interleave is multiple of 0x08 (default), often +0x1000
#   * Must set decoding coefficients (coef_offset/spacing/etc)
#   * Should set ADPCM state (hist_offset/spacing/etc)
# - DTK|NGC_DTK    Nintendo ADP/DTK ADPCM
#   * For rare GC games
# - PCM16LE        PCM 16-bit little endian
#   * For many games (usually on PC)
#   * Interleave is multiple of 0x2 (default)
# - PCM16BE        PCM 16-bit big endian
#   * Variation for certain consoles (GC/Wii/PS3/X360/etc)
# - PCM8           PCM 8-bit signed
#   * For some games (usually on PC)
#   * Interleave is multiple of 0x1 (default)
# - PCM8_U         PCM 8-bit unsigned
#   * Variation with modified encoding
# - PCM8_U_int     PCM 8-bit unsigned (interleave block)
#   * Variation with modified encoding
# - IMA            IMA ADPCM (mono/stereo)
#   * For some PC games, and rarely consoles
#   * Special interleave is multiple of 0x1, often +0x80
# - DVI_IMA        IMA ADPCM (DVI order)
#   * Variation with modified encoding
# - AICA           Yamaha AICA ADPCM (mono/stereo)
#   * For some Dreamcast games, and some arcade (Naomi) games
#   * Special interleave is multiple of 0x1
# - APPLE_IMA4     Apple Quicktime IMA ADPCM
#   * For some Mac/iOS games
# - MS_IMA         Microsoft IMA ADPCM
#   * For some PC games
#   * Interleave (frame size) varies, often multiple of 0x100 [required]
# - MSADPCM        Microsoft ADPCM (mono/stereo)
#   * For some PC games
#   * Interleave (frame size) varies, often multiple of 0x100 [required]
# - SDX2           Squareroot-delta-exact 8-bit DPCM
#   * For many 3DO games
# - MPEG           MPEG Audio Layer file (MP1/2/3)
#   * For some games (usually PC/PS3)
#   * May set skip_samples
# - ATRAC3         Sony ATRAC3
#   * For some PS2 and PS3 games
#   * Interleave (frame size) can be 0x60/0x98/0xC0 * channels [required]
#   * Should set skip_samples (more than 1024+69 but varies)
# - ATRAC3PLUS     Sony ATRAC3plus
#   * For many PSP games and rare PS3 games
#   * Interleave (frame size) can be: [required]
#     Mono: 0x0118|0178|0230|02E8
#     Stereo: 0x0118|0178|0230|02E8|03A8|0460|05D0|0748|0800
#     6/8 channels: multiple of one of the above
#   * Should set skip_samples (more than 2048+184 but varies)
# - XMA1           Microsoft XMA1
#   * For early X360 games
# - XMA2           Microsoft XMA2
#   * For later X360 games
# - FFMPEG         Any headered FFmpeg format
#   * For uncommon games
#   * May set skip_samples
# - AC3            AC3/SPDIF
#   * For few PS2 games
#   * Should set skip_samples (around 256 but varies)
# - PCFX           PC-FX ADPCM
#   * For many PC-FX games
#   * Interleave is multiple of 0x1, often +0x8000
#   * Sample rate may be ~31468/~15734/~10489/~7867
# - PCM4           PCM 4-bit signed
#   * For early consoles
# - PCM4_U         PCM 4-bit unsigned
#   * Variation with modified encoding
# - OKI16          OKI ADPCM with 16-bit output (not VOX/Dialogic 12-bit)
#   * For rare PS2 games (Sweet Legacy, Hooligan)
# - AAC            Advanced Audio Coding (raw without .mp4)
#   * For some 3DS games and many iOS games
#   * Should set skip_samples (around 1024 but varies)
# - TGC            Tiger Game.com 4-bit ADPCM
#   * For Tiger Game.com
# - ASF            Argonaut ASF ADPCM
#   * For rare Argonaut games [Croc (SAT)]
# - EAXA           Electronic Arts EA-XA ADPCM
#   * For rare EA games [Harry Potter and the Chamber of Secrets (PC)]
codec = (codec string)
```

#### CODEC VARIATIONS
Changes the behavior of some codecs:
```
# - NGC_DSP: 0=normal interleave, 1=byte interleave, 2=no interleave
# - XMA1|XMA2: 0=dual multichannel (2ch xN), 1=single multichannel (1ch xN)
# - XBOX|EAXA: 0=standard (mono or stereo interleave), 1=force mono interleave mode
# - PCFX: 0=standard, 1='buggy encoder' mode, 2/3=same as 0/1 but with double volume
# - PCM4|PCM4_U: 0=low nibble first, 1=high nibble first
# - others: ignored
codec_mode = (variation)
```

#### (deprecated) VALUE MODIFIERS
*Use inline math instead of this.*

Changes next read to: `(key) = (value) */+- value_(op)`. Set to 0 when done using, as it affects ANY value. Priority is as listed.
```
value_mul|value_* = (value)
value_div|value_/ = (value)
value_add|value_+ = (value)
value_sub|value_- = (value)
```

#### INTERLEAVE / FRAME SIZE [REQUIRED depending on codec]
This value changes how data is read depending on the codec:
- For mono/interleaved codecs it's the amount of data between channels, and while optional (defaults described in the "codec" section) you'll often need to set it to get proper sound.
- For codecs with custom frame sizes (MSADPCM, MS-IMA, ATRAC3/plus) means frame size and is required.
- Interleave 0 means "stereo mode" for codecs marked as "mono/stereo", and setting it will usually force mono-interleaved mode.

Special values:
- `half_size`: sets interleave as data_size / channels automatically
```
interleave = (value)|half_size
```

#### INTERLEAVE IN THE LAST BLOCK
In some files with interleaved data the last block (`interleave * channels`) of data is smaller than normal, so `interleave` is smaller for that block. Setting this fixes decoding glitches at the end.

Note that this doesn't affect files with padding data in the last block (as the `interleave` itself is constant).

Special values:
- `auto`: calculate based on channels, interleave and data_size/start_offset
```
interleave_last = (value)|auto
```

#### ID VALUES
Validates that `id_value` (normally set as constant value) matches value read at `id_offset`. The file will be rejected and won't play if values don't match.

Can be redefined several times, it's checked whenever a new id_offset is found.
```
id_value = (value)
id_offset = (value)
```

#### NUMBER OF CHANNELS [REQUIRED]
```
channels = (value)
```

#### MUSIC FREQUENCY [REQUIRED]
```
sample_rate = (value)
```

#### DATA START
Where encoded data actually starts, after the header part. Defaults to 0.
```
start_offset = (value)
```

#### DATA SIZE
Special variable that can be used in sample values. Defaults to `(file_size - start_offset)`, re-calculated when `start_offset` is set. With multiple subsongs, `block_size` or padding are set this it's recalculated as well.

If data_size is manually set it stays constant and won't be auto changed.
```
data_size = (value)
```

#### DATA PADDING
Some files have extra padding at the end that is meant to be ignored. This adjusts the padding in `data_size`, manually or auto-calculated.

Special values (for PS-ADPCM only):
- `auto`: discards null frames
- `auto-empty`: discards null and 'empty' frames (for games with weird padding)
```
padding_size = (value)|auto|auto-empty
```

#### SAMPLE MEANINGS
Modifies the meaning of sample fields when set *before* them.

Accepted values:
- `samples`: exact sample (default)
- `bytes`: automatically converts bytes/offset to samples (applies after */+-& modifiers)
- `blocks`: same as bytes, but value is given in blocks/frames
  * Value is internally converted from blocks to bytes first: `bytes = (value * interleave*channels)`

Some codecs can't convert bytes-to-samples at the moment: `FFMPEG`. For XMA1/2, bytes does special parsing, with loop values being bit offsets within data (as XMA has a peculiar way to loop).
```
sample_type = samples|bytes|blocks
```

#### SAMPLE VALUES [REQUIRED (num_samples)]
Special values:
- `data_size`: automatically converts bytes-to-samples
```
num_samples         = (value)|data_size
loop_start_sample   = (value)
loop_end_sample     = (value)|data_size
```

#### LOOP SETTING
Force loop on or off, as loop start/end may be defined but not used. If not set, by default it loops when loop_end_sample is defined and less than num_samples.

Special values:
- `auto`: tries to autodetect loop points for PS-ADPCM data using data loop flags.

Sometimes games give loop flags different meaning, so behavior can be tweaked by defining `loop_behavior` before `loop_flag`:
- `default`: values 0 or 0xFFFF/0xFFFFFFFF (-1) disable looping, but not 0xFF (loop endlessly)
- `negative`: values 0xFF/0xFFFF/0xFFFFFFFF (-1) enable looping
- `positive`: values 0xFF/0xFFFF/0xFFFFFFFF (-1) disable looping
- `inverted`: values not 0 disable looping

```
loop_behavior = default|negative|positive|inverted
loop_flag = (value)|auto
```

#### LOOP START/END MODIFIER
For XMA1/2 + sample_type=bytes it means loop subregion, if read after loop values.

For other codecs its added to loop start/end, if read before loop values (a format may rarely have rough loop offset/bytes, then a loop adjust in samples).

```
loop_adjust = (value)
```

#### ENCODER DELAY
Beginning samples to skip, a.k.a. priming samples or encoder delay, that some codecs use to "warm up" the decoder. This is needed for proper gapless support.

Supported codecs: `ATRAC3/ATRAC3PLUS/XMA/FFMPEG/AC3/AAC`
```
skip_samples = (value)
```

#### DSP DECODING COEFFICIENTS [REQUIRED for DSP]
DSP needs a "coefs" list to decode correctly. These are 8*2 16-bit values per channel, starting from `coef_offset`.

Usually each channel uses its own list, so we may need to set separation per channel, usually 0x20 (16 values * 2 bytes). So channel N coefs are read at `coef_offset + coef_spacing * N`

Those 16-bit coefs can be little or big endian (usually BE), set `coef_endianness` directly or in an offset value where `0=LE, >0=BE`.

While the coef table is almost always included per-file, some games have their coef table in the executable or precalculated somehow. You can set inline coefs instead of coef_offset. Format is a long string of bytes (optionally space-separated) like `coef_table = 0x1E02DE01 3C0C0EFA ...`. You still need to set `coef_spacing` and `coef_endianness` though.
```
coef_offset = (value)
coef_spacing = (value)
coef_endianness = BE|LE|(value)
coef_table = (string)
```

#### ADPCM STATE
Some ADPCM codecs need to set up their initial or "history" state, normally one or two 16-bit PCM samples per channel, starting from `hist_offset`.

Usually each channel uses its own state, so we may need to set separation per channel.

State values can be little or big endian (usually BE for DSP), set `hist_endianness` directly or in an offset value where ´0=LE, >0=BE´.

Normally audio starts with silence or hist samples are set to zero and can be ignored, but it does affect a bit resulting output.

Currently used by DSP.
```
hist_offset = (value)
hist_spacing = (value)
hist_endianness = BE|LE|(value)
```

#### HEADER/BODY SETTINGS
Changes internal header/body representation to external files.

TXTH commands are done on a "header", and decoding on "body". When loading an unsupported file it becomes the "base" file
that loads the .txth, and is both header and body.

You can alter those, mainly for files that split header and body in separate files (load base file and txth sets header on another file). It's also possible to load the .txth directly with a set body, as a sort of "reverse TXTH" (useful with bigfiles, as you could have one .txth per song).

Allowed values:
- (filename): open any file, subdirs also work (dir/filename)
- *.(extension): opens with same name as the "base" file (the one you open, not the .txth) plus another extension
- null: unloads file and goes back to defaults (body/header = base file).
```
header_file = (filename)|*.(extension)|null
body_file = (filename)|*.(extension)|null
```

#### SUBSONGS
Sets the number of subsongs in the file, adjusting reads per subsong N: `value = @(offset) + subsong_offset*N`. number/constants values aren't adjusted though.

Mainly for bigfiles with consecutive headers per subsong, set subsong_offset to 0 when done as it affects any reads. The current subsong number is handled externally by plugins or TXTP.
```
subsong_count = (value)
subsong_offset = (value)
```

#### NAMES
Sets the name of the stream, most useful when used with subsongs. TXTH will read a string at `name_offset`, with `name_size characters`.

`name_size` defaults to 0, which reads until null-terminator or a non-ascii character is found.

`name_offset` can be a (number) value, but being an offset it's also adjusted by subsong_offset.
```
name_offset = (value)
name_size = (value)
```

#### SUBFILES
Tells TXTH to parse a full file (ex. an Ogg) at `subfile_offset`, with size of `subfile_size` (defaults to `file size - subfile_offset` if not set). This is useful for files that are just container of other files, so you don't have to remove the extra data (since it could contain useful stuff like loop info).

Internal subfile extension can be changed to `subfile_extension` if needed, as vgmstream won't accept unknown extensions (for example if your file uses .vgmstream or .pogg you may need to set subfile_extension = ogg).

Setting any of those three will trigger this mode (it's ok to set offset 0). Once triggered most fields are ignored, but not all, explained later. This will also set some values like `channels` or `sample_rate` if not set for calculations/convenience.
```
subfile_offset = (value)
subfile_size = (value)
subfile_extension = (string)
```

#### CHUNK DEINTERLEAVING
Some files interleave data chunks, for example 3 stereo songs pasted together, alternating 0x10000 bytes of data each. These settings allow vgmstream to play one of the chunks while ignoring the rest (read 0x10000 data, skip 0x10000*2).
File is first "dechunked" then played with using other settings (`start_offset` would point within the internal  dechunked" file). It can be used to remove garbage data that affects decoding, too.


You need to set:
- `chunk_count`: total number of interleaved chunks (ex. 3=3 interleaved songs)
- `chunk_number`: first chunk to start (ex. 1=0x00000, 2=0x10000, 3=0x20000...)
  * If you set `subsong_count` first `chunk_number` will be auto-set per subsong (subsong 1 starts from chunk number 1, subsong 2 from chunk 2, etc)
- `chunk_start`: absolute offset where chunks start (normally 0x00)
- `chunk_size`: amount of data in a single chunk (ex. 0x10000)
For fine-tuning you can optionally set (before `chunk_size`, for reasons):
- `chunk_header_size`: header to skip before chunk data (part of chunk_size)
- `chunk_data_size`: actual data size (part of chunk_size, rest is header/padding)

So, if you set size to 0x1000, header_size 0x100, data_size is implicitly 0xF00, or if size is 0x1000 and data_size 0x800 last 0x200 is ignored padding. Use combinations of the above to make vgmstream "see" only actual codec data.
```
chunk_count = (value)
chunk_number = (value)
chunk_start = (value)
chunk_header_size = (value)
chunk_data_size = (value)
chunk_size = (value)
```

#### NAME TABLE
Some games have headers for all files pasted together separate from the actual data, but this order may be hard-coded or even alphabetically ordered by filename. In those cases you can set a "name table" that assigns constant values (one or many) to filenames. This table is loaded from an external text file (for clarity) and can be set to any name, for example `name_table = .names.txt`
```
name_table = (filename)
```

Inside the table you define lines mapping a filename to a bunch of values, in this format:
```
# base definition
(filename1): (value)
...
# may put multiple comma-separated values, spaces are ok
(filenameN)    : (value1), (...)   ,   (valueN)  # inline comments too

# put no name before the : to set default values
 : (value1), (...), (valueN)
```
Then I'll find your current file name, and you can then reference its numbers from the list as a `name_value` field, like `base_offset = name_value`, `start_offset = 0x1000 + name_value1`, `interleave = name_value5`, etc. `(filename)` can be with or without extension (like `bgm01.vag` or just `bgm01`), and if the file's name isn't found it'll use default values, and if those aren't defined you'll get 0 instead. Being "values" they can use math or offsets too (`bgm05: 5*0x010`).

You can use wildcards to match multiple names too (it stops on first name that matches), and UTF-8 names should work, case insensitive even.
```
bgm_??_4: 4 # 4ch: files like bgm_00_4, bgm_01_4, etc
bgm*_M: 1   # 1ch: some files end with _M for mono
bgm*: 2     # 2ch: all other files, notice order matters
```

While you can put anything in the values, this feature is meant to be used to store some number that points to the actual data inside a real multi-header, that could be set with `header_file`. If you feel the need to store many constant values per file, there is good chance it can be done in some better, simpler way.


#### BASE OFFSET MODIFIER
You can set a default offset that affects next `@(offset)` reads making them `@(offset + base_offset)`, for cleaner parsing (particularly interesting when combined with the `name_table`).

For example instead of `channels = @0x714` you could set `base_offset = 0x710, channels = @0x04`. Set to 0 when you want to disable it.
```
base_offset = (value)
```


## Complex usages

### Temporary values
Most commands are evaluated and calculated immediatedly, every time they are found. This is by design, as it can be used to adjust and trick for certain calculations.

It makes TXTHs a bit harder to follow, as they are order dependant, but otherwise it's hard to accomplish some things or others become ambiguous.


For example, normally you are given a data_size in bytes, that can be used to calculate num_samples for all channels.
```
channels = 2
sample_type = bytes
num_samples = @0x10     #calculated from data_size
```

But sometimes this size is for a single channel only (even though the file may be stereo). You can set temporally change the channel number to force a correct calculation.
```
channels = 1            #not the actual number of channels
sample_type = bytes
num_samples = @0x10     #calculated from channel_size
channels = 2            #change once calculations are done
```
You can also use:
```
channels = 2
sample_type = bytes
num_samples = @0x10 * channels  # resulting bytes is transformed to samples
```

Do note when using special values/strings like `data_size` in `num_samples` and `loop_end_samples` they must be alone to trigger.
```
data_size = @0x100
num_samples = data_size * 2 # doesn't tranform bytes-to-samples (do it before? after?)
```
```
data_size = @0x100 * 2
num_samples = data_size     # ok
```
Also beware of order:
```
start_offset = 0x200        # recalculated data_size
num_samples = data_size     # transforms bytes-to-samples
data_size = @0x100          # useless as num_samples is already transformed
```


### Redefining values
Some commands alter the function of all next commands and can be redefined as needed:
```
samples_type = bytes
num_samples = @0x10

samples_type = sample
loop_end_sample = @0x14
```

### External files
When setting external files all commands are done on the "header" file, but with some creativity you can read in multiple files.
```
body_file = bgm01.bdy
header_file = bgm01.hdr
channels = @0x10        #base info in bgm01.hdr
header_file = bgm01.bdy
coef_offset = 0x00      #DSP coefs in bgm01.bdy
```
Note that DSP coefs are special in that aren't read immediately, and will use *last* header_file set.


### Resetting values
Values may need to be reset (to 0 or other sensible value) when done. Subsong example:
```
subsong_count = 5
subsong_offset = 0x20   # there are 5 subsong headers, 0x20 each
channel_count = @0x10   # reads channels at 0x10+0x20*subsong
# 1st subsong: 0x10+0x20*0: 0x10
# 2nd subsong: 0x10+0x20*1: 0x30
# 2nd subsong: 0x10+0x20*2: 0x50
# ...
start_offset = @0x14    # reads offset within data at 0x14+0x20*subsong

subsong_offset = 0      # reset value
sample_rate = 0x04      # sample rate is the same for all subsongs
# Nth subsong ch: 0x04+0x00*N: 0x08
```

### Math
Sometimes header values are in "sectors" or similar concepts (typical in DVD games), and need to be adjusted to a real value using some complex math:
```
sample_type   = bytes
start_offset  = @0x10 * 0x800    # 0x15 * DVD sector size, for example
```

You can use `+-*/&` operators, and also certain fields' values:
```
num_samples = @0x10 * channels  # byte-to-samples of channel_size
```
`data_size` is a special value for `num_samples` and `loop_end_sample` and will always convert as bytes-to-samples, though.


Priority is left-to-right. Do add brackets though, they are accounted for and if they are implemented in the future your .txth *will* break with impunity.
```
# normal priority
data_size = @0x10 * 0x800 + 0x800
# also works
data_size = (@0x10 + 1) * 0x800
# same as above but don't do this
# (may become @0x10 + (1 * 0x800) in the future
data_size = @0x10 + 1 * 0x800
# doesn't work at the moment, so reorder as (1 * 0x800) + @0x10
data_size = @0x10 + (1 * 0x800)
# fails, wrong bracket count
data_size = (@0x10 + 1 * 0x800
# fails, wrong bracket count
data_size = )@0x10 + 1 * 0x800
```

If a TXTH needs too many calculations it may be better to implement directly in vgmstream though, consider reporting.


### Modifiers
Remnant of simpler math (priority is fixed to */+-), *shouldn't be needed anymore*.

```
value_multiply = 0x800
start_offset   = @0x10
value_multiply = 0
```

```
value_add = 1
channels = @0x08
value_add = 0

value_multiply = channels
sample_type = bytes
num_samples = @0x10
value_multiply = 0
```

```
value_add       = 0x10
value_mul       = 0x800
start_offset    = @0x10
```

### Subfiles
Sometimes a file is just a wrapper for another common format. In those cases you can tell TXTH to just play the internal format:
```
subfile_offset = 0x20   # tell TXTH to parse a full file (ex. .ogg) at this offset
subfile_size = @0x10    # defaults to (file size - subfile_offset) if not set
subfile_extension = ogg # may be ommited if subfile extension is the same

# many fields are ignored
codec = PCM16LE
interleave = 0x1000
channels = 2

# a few fields are applied
sample_rate = @0x08
num_samples = @0x10
loop_start_sample = @0x14
loop_end_sample = @0x18
```
Most fields can't be changed after parsing since doesn't make much sense technically, as the parsed subfile should supply them. You can set them to use bytes-to-samples conversions, though.
```
# parses subfile at start with some num_samples
subfile_offset = 0x20
# force recalculation of num_samples
codec = PSX
start_offset = 0x40
num_samples = data_size
```

### Chunks
Chunks affect some values (padding size, data size, etc) and are a bit sensitive to order at the moment, due to technical complexities:
```
# Street Fighter EX3 (PS2)

# base config is defined normally
codec       = PSX
sample_rate = 44100
channels    = 2
interleave  = 0x8000

# set subsong number instead of chunk_number for subsongs
subsong_count = 26
#chunk_number = 1
chunk_start = 0
chunk_size = 0x10000
chunk_count = 26

# after setting chunks (sizes vary when 'dechunking')
start_offset = 0x00
padding_size = auto-empty
num_samples = data_size
```

Subfiles and chunks can coexist:
```
# Gitaroo Man (PSP)

# 3 interleaved RIFF files
subsong_count = 3
chunk_start   = 0
chunk_size    = 0x2800
chunk_count   = 3

# the 3 de-interleaved chunks are treated and parsed as a subsong
subfile_offset = 0
subfile_size = @0x04 + 0x08  #RIFF size
subfile_extension = at3
```

It can be used to make blocks with padding playable:
```
# Mortal Kombat: Deception (PS2)
codec = PSX
interleave = 0x3F40
sample_rate = 32000
channels = 2

chunk_number    = 1
chunk_count     = 1
chunk_start     = 0x00
chunk_data_size = interleave * channels
chunk_size      = 0x8000

num_samples = data_size
```

### Base offset chaining
Some formats read an offset to another part of the file, then another offset, then other, etc.

You can simulate this chaining multiple `base_offset`
```
base_offset = @0x10                 #sets current at 0x1000
channels    = @0x04                 #reads at 0x1004 (base_offset + 0x04)
base_offset = base_offset + @0x10   #sets current at 0x1000 + 0x200 = 0x1200
sample_rate = @0x04                 #reads at 0x1204
...
```


## Examples

#### Colin McRae DiRT (PC) .wip.txth
```
id_value = 0x00000000   #check that value at 0x00 is really 0x00000000
id_offset = @0x00:BE

codec = PCM16LE
channels = 2
sample_rate = 32000
start_offset = 0x04
num_samples = data_size
loop_start_sample = 0
loop_end_sample = data_size
```

#### Kim Possible: What's the Switch (PS2) .str.txth
```
codec = PSX
interleave = 0x2000
channels = 2
sample_rate = 48000
num_samples = data_size
interleave_last = auto
```

#### Manhunt (Xbox) .rib.txth
```
codec = XBOX
codec_mode = 1 #interleaved XBOX
interleave = 0xD800

channels = 12
sample_rate = 44100
start_offset = 0x00
num_samples = data_size
```

#### Pitfall The Lost Expedition (PC) .txth
```
codec = DVI_IMA
interleave = 0x80
start_offset = 0x00
channels = 2
sample_rate = 44100
num_samples = data_size
```

#### Spy Hunter (GC) .pcm.txth
```
codec = PCM8
sample_rate = 32000
channels = 1
start_offset = 0
num_samples = data_size
```

#### Ultimate Board Game Collection (Wii) .dsp.txth
```
codec = NGC_DSP
interleave = 0x10000

channels = 2
start_offset = 0x00

num_samples = @0x00:BE
sample_rate = @0x08:BE
loop_flag   = @0x0C:BE$2
sample_type = bytes
loop_start_sample = @0x10:BE
loop_end_sample   = @0x14:BE

coef_offset = 0x1c
coef_spacing = 0x10000
coef_endianness = BE
```

#### Aladdin in Nasira's Revenge (PS1) .cvs.txth
```
codec = PSX
interleave = 0x10
sample_rate = 24000
channels = 1
padding_size = auto-empty
num_samples = data_size
```

#### Shikigami no Shiro - Nanayozuki Gensoukyoku (PS2) bgm.txth
```
codec = PSX
interleave = 0x1000

# this .txth is meant to be loaded directly
header_file = data/SLPM_660.69
body_file = data/BGM.BIN

channels = 2

# subsong headers at 0x1A5A40, entry size 0x14, total 58 * 0x14 = 0x488
subsong_count     = 58
subsong_offset    = 0x14
base_offset       = 0x1A5A40

sample_rate       = @0x00
start_offset      = @0x04 * 0x800  #in sectors

sample_type       = bytes
num_samples       = @0x08 * channels  #in 1ch sizes
loop_start_sample = @0x0c * channels
loop_end_sample   = @0x10 * channels

data_size         = @0x08 * channels  #for bitrate
```

#### Dragon Poker (Mobile) .snd.txth
```
# parse MP3 inside the .snd
subfile_extension = mp3
subfile_offset = 0x14
#subfile_size = @0x10

# manually set looping
codec = MPEG
start_offset = 0x14
num_samples = data_size
loop_start_sample = 0
loop_end_sample = data_size
```

#### Simple 2000 Series Vol. 120 - The Saigo no Nihonhei (PS2) .xag.txth
```
header_file = TSNDDRVC.IRX

name_table = .names.txt
base_offset = 0xAC3c + name_value

codec = PSX
interleave = @0x10
sample_rate = @0x0A$2 * 48000 / 4096  #pitch value
channels = @0x0D$1
loop_start_sample = @0x0E$1 * interleave / 2 / 0x10 * 28
loop_flag = @0x0F$1

padding_size = auto-empty
loop_end_sample = data_size
num_samples = data_size
```
*.names.txt*
```
# offset-to-header within TSNDDRVC.IRX at around 0xAC3C + position * 0x18
BGM001.XAG: 0x00
BGM002.XAG: 0x18
BGM000.XAG: 0x30
BGM003.XAG: 0x48
BGM008.XAG: 0xA8
BGM010.XAG: 0xD8
BGM011.XAG: 0xF0
BGM012.XAG: 0x108
PAD.XAG   : 0x150
JIN002.XAG: 0x168
JIN003.XAG: 0x180
```


#### Grandia (PS1) bgm.txth
```
header_file       = GM1.IDX
body_file         = GM1.STZ

subsong_count     = 394  #last doesn't have size though
subsong_offset    = 0x04

subfile_offset    = (@0x00 & 0xFFFFF) * 0x800
subfile_extension = seb
subfile_size      = ((@0x04 - @0x00) & 0xFFFFF) * 0x800
```

#### Zack & Wiki (Wii) .ssd.txth
```
header_file = bgm_S01.srt
name_table = .names.txt

base_offset = @0x0c:BE
base_offset = base_offset + @0x08:BE + name_value
base_offset = base_offset + @0x00:BE - name_value

codec = NGC_DSP
channels = 2
interleave = half_size
sample_rate = @0x08:BE
loop_flag = @0x04:BE

sample_type = bytes
loop_start_sample = @0x10:BE
loop_end_sample = @0x14:BE
num_samples = @0x18:BE

coef_offset = 0x20
coef_spacing = 0x40
coef_endianness = BE
```
*.names.txt*
```
st_s01_00a.ssd: 0*0x04
st_s01_00b.ssd: 1*0x04
st_s01_00c.ssd: 2*0x04
st_s01_01a.ssd: 3*0x04
st_s01_01b.ssd: 4*0x04
st_s01_02a.ssd: 5*0x04
st_s01_02b.ssd: 6*0x04
st_s01_02c.ssd: 7*0x04
```


#### Zack & Wiki (Wii) st_s01_00a.txth
```
#alt from above with untouched folders
header_file = Sound/BGM/bgm_S01.srt
body_file = snd/stream/st_s01_00a.ssd
name_table = .names.txt

base_offset = @0x0c:BE
base_offset = base_offset + @0x08:BE + name_value
base_offset = base_offset + @0x00:BE - name_value

codec = NGC_DSP
channels = 2
interleave = half_size
sample_rate = @0x08:BE
loop_flag = @0x04:BE

sample_type = bytes
loop_start_sample = @0x10:BE
loop_end_sample = @0x14:BE
num_samples = @0x18:BE

coef_offset = 0x20
coef_spacing = 0x40
coef_endianness = BE
```
*.names.txt*
```
*snd/stream/st_s01_00a.ssd: 0*0x04
*snd/stream/st_s01_00b.ssd: 1*0x04
*snd/stream/st_s01_00c.ssd: 2*0x04
*snd/stream/st_s01_01a.ssd: 3*0x04
*snd/stream/st_s01_01b.ssd: 4*0x04
*snd/stream/st_s01_02a.ssd: 5*0x04
*snd/stream/st_s01_02b.ssd: 6*0x04
*snd/stream/st_s01_02c.ssd: 7*0x04
# uses wildcards for full paths from plugins
```

####  Croc (SAT) .asf.txth
```
codec = ASF
sample_rate = 22050
channels = 2
num_samples = data_size
```
