# hs100

A tool to control TP-Link HS100/HS103/HS105/HS110/HS300 wi-fi smart plug power strips. The tool allows turning them on, off, reboot, and performing set up without using TP-Link's app. Supports Linux, OSX, IRIX, and Windows under WSL. Loosely based on [pyHS100](https://github.com/GadgetReactor/pyHS100) and
[research from softScheck](https://www.softscheck.com/en/reverse-engineering-tp-link-hs110/).

## Usage

From the command-line:

    hs100 <ip> <command>

Where `<ip>` is the IP address of the smart plug and the list of `<command>`s include:

- `associate <ssid> <key> <key_type>`: set wifi AP to connect to. get your
key\_type by doing a scan
- `info`: get device information including device IDs
- `emeter`: realtime power consumption (works with HS110 and HS300)
- `factory-reset`: reset the plug to factory settings
- `off`: turn the power off
- `on`: turn the power on
- `reboot`: reboot the plug
- `scan`: scan for nearby wifi APs (probably only 2.4 GHz ones)
- `set_server <url>`: set cloud server to \<url\> instead of TP-Link's
- Alternatively, you can supply a JSON string to be sent directly to the
device. Note that the JSON string must be quoted, like so:
`hs100 <ip> '{"system":{"set_relay_state":{"state":1}}}'`

### Usage with outlet strips

When using with outlet strips (such as the HS300 or KP303), the `emeter`, `off` and `on` commands can be used on the individual outlets of multiple outlet strips.

- use the `info` command to get the ID string of the outlet you want to control
- append the ID string to the command line for `emeter`, `off` or `on` like so:
`hs100 <ip> on 80062947BE0A1339DC7914B75B24A6A51FB6977E02`

## Initial setup

Follow the instructions in this section to avoid installing the "Kasa" app on
your phone.

1. Get the plug into the "blinking amber and blue" state, in which it will
spin up its own Acess Point (AP) and await commands. A brand new plug should do
this automatically. Otherwise, hold down one of the buttons (depending on the
model) for about five seconds, until its light blinks amber and blue.
1. Wait for a wi-fi Access Point will be available. The name will be similar to
`"TP-Link_Smart Plug_XXXX`.
1. The device IP address will be 192.168.0.100, with the plug at 192.168.0.1.

Issue the following commands to the plug:

- Factory reset the plug to get rid of any settings from a previous owner:
`hs100 192.168.0.1 factory-reset`. You will be disconnected from its wifi AP.
Once the factory reset is done (usually a few seconds), reconnect to the
plug's AP.
- Disable cloud nonsense by setting a bogus server URL: `hs100 192.168.0.1 set_server localhost`
- Scan for your wi-fi AP using `hs100 192.168.0.1 scan`. Find your AP in the
list and note its `key_type`; you will need this to associate.
- Associate with your AP using `hs100 192.168.0.1 associate <ssid> <password> <key_type>`
. Your `key_type` is a number that indicates the kind of wi-fi security that
your AP is using. You can find it by doing a wi-fi scan (see previous step).

If the light turns solid amber, then it was unable to associate-- factory
reset the plug and try again. Otherwise, the light on your plug will change
first to blinking blue, then to solid blue indicating that it has successfully
connected to your AP.

## Build

To build:

1. Install `build-essential`.
1. Run `make`.

The binary `hs100` is built and ready for use. For example:

    ./hs100 192.168.0.1 off

## To do

Future features:

- better error checking
- plug discovery

Improvements welcome!

