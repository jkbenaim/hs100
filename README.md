# hs100

A tool for using TP-Link HS100/HS105/HS110 wi-fi smart plugs. You can turn
them on or off, reboot them, and factory reset them.

Tested to work on Linux, OSX, IRIX, and Windows under WSL.

Loosely based on [pyHS100](https://github.com/GadgetReactor/pyHS100) and
[research from softScheck](https://www.softscheck.com/en/reverse-engineering-tp-link-hs110/).

## Usage

`hs100 <ip> <command>`

Command can be:
- `on`: turn the power on
- `off`: turn the power off
- `reboot`: reboot the plug
- `reset-yes-really`: factory reset the plug
- `scan`: scan for nearby wifi APs (probably only 2.4 GHz ones)
- Alternatively, you can supply a JSON string to be sent directly to the
device. Note that the JSON string must be quoted, like so:
`hs100 <ip> '{"system":{"set_relay_state":{"state":1}}}'`

## Todo

- better error checking
- plug discovery

This program is very basic. Patches welcome!
