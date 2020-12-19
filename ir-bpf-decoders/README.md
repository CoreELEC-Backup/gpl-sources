# ir-bpf-decoders

Compiled IR BPF protocol decoders from `utils/keytable/bpf_protocols` directory of
[v4l-utils](https://linuxtv.org/downloads/v4l-utils/)

Since linux kernel 4.18 additional IR protocol decoders can be added via BPF
programs. This works by `ir-keytable` loading the BPF code into the kernel and
attaching it to the rc device.

`v4l-utils` ships with several BPF decoders in C source code form which need
to be compiled into BPF code with `clang`.

As the LibreELEC toolchain doesn't include `clang` and the BPF code is target
independent this is done separately on a build host with `clang` installed.

## Build prerequisites

Install `libelf` development files and `clang`. eg:

```
apt-get install libelf-dev clang
```

## Building

Download, unpack and configure `v4l-utils` matching the version in LibreELEC. eg:
```
wget https://linuxtv.org/downloads/v4l-utils/v4l-utils-1.18.0.tar.bz2
tar xf v4l-utils-1.18.0.tar.bz2
cd v4l-utils-1.18.0
./configure
```
Compile BPF protocol decoders
```
make -C utils/keytable/bpf_protocols
```

Update BPF object files in this repo. eg:
```
cp utils/keytable/bpf_protocols/*.o ~/libreelec/ir-bpf-decoders
```

