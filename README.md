# T #
T is a lean **T**erminal emulator. The project aims to keep it as simple as
possible, while still being usable. Configuration is done by modifying the file
`config.h` and recompiling. It is actually two applications: one acts as a
daemon (`Td`) while the other (`Tc`) is invoked to request new windows or kill
the daemon. This was done so that the resource usage is (somewhat) minimized by
keeping all terminals under the same process while new windows are opened
(somewhat) faster.

## Compilation ##
T has the following compilation dependencies. The versions are the ones used to
develop T, but it will *probably* work with any API-compatible versions.

* VTE 0.28
* GTK+ 2.24

To compile T, run

    make

## Installation ##
To install T, run (needs superuser privileges)

    make install

## Usage ##
`Td` has no invocation arguments. You just keep it running and it will listen on
a local socket file (see `config.h`) for connections from `Tc`. `Tc` is invoked
as

    Tc [ARG]

where `ARG` is one of the arguments in the following table.

Argument | Description
---------|-------------------------------------------------------------------------
`-n`     | Requests a new window; default if omitted
`-x`     | Request daemon termination; only successful if there are no open windows
