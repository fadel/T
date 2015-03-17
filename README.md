# T #

T is a lean **T**erminal emulator. The project aims to keep it as simple as
possible, while still being usable. Configuration is done by modifying the file
`config.h` and recompiling.

## Compilation ##
T has the following compilation dependencies. The versions are the ones used to
develop T, but it will *probably* work with any API-compatible versions.

* VTE 0.28
* GTK+ 2.24

To compile T, run:

    make

## Installation ##
To install T, run (needs superuser privileges):

    make install
