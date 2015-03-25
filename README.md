# C++14 simulator

These 69 commits were written between 2015-01-11 and 2015-01-27.

This was my last (and best) attempt at creating a framework for rigorous backtesting of trading algorithms.

This was my first time using modern C++ for a project.  C++14 is actually very nice.  The last time I had used C++ was C++03 at my first startup, WeGame.

### explorer

This project is focused on being a playground for algorithm discovery using Lua.

### simulator

This is the beef.  All the code shared between the explorer and wfa projects lives here.

### wfa

This project actually implements WFA (Walk Forward Analysis) for testing algorithms discovered using the "explorer" project above.

## Usage

Each directory has a makefile.  Just `cd` and type `make`.  You need to build the **simulator** directory first.

## License

BSD
