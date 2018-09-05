# hs100

A small program for flipping TP-Link HS100/HS105/HS110 wi-fi smart plugs on
and off.

Loosely based on [pyHS100](https://github.com/GadgetReactor/pyHS100) and
[research from softScheck](https://www.softscheck.com/en/reverse-engineering-tp-link-hs110/).

Tested to work on Linux, OSX, and IRIX.

## Usage
```hs100 <ip of plug> <json command blob>```

As a convenience, you can supply the words 'on' or 'off' in place of a json
command blob.

## Todo

- better error checking
- plug discovery

This program is very basic. Patches welcome!
