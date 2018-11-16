# HIP I-V measurement tool

Tool to help I-V measurements with a given setup.

Currently designed for:

- *Keithley 2410* as voltage source, and
- *Keithley 6487* for current measurement.

## Dependencies

- [NI-488.2](http://www.ni.com/download/ni-488.2---linux/6902/en/) OR [Linux GPIB](https://linux-gpib.sourceforge.io/) driver;
- gcc-c++ version ≥ 4.8 to build everything;
- CMake version ≥ 2.8 for the automatic generation of makefiles (see the installation part);
- python-devel for the parsing of configuration files.

Optionally:
- Doxygen for the generation of the documentation (try `make doc_doxygen` and point your browser to <file:///path/to/your/ivutils/folder/doc/html/index.html>)

## Installation

- `cd` to your cloned repository,
- `mkdir build && cd build` to define the location of the library and tests,
- `cmake ..` to generate a personalised Makefile for your system,
- `make` to build the `.so` library. You may link it against several tests (see the `test/` directory).