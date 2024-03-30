# Control "SwitchBox-USB" USB plug

Sold under various brand names, such as https://www.antrax.de/produkt/switchbox-usb-type-c/.

## Prerequisites

 - A Linux system
 - HIDAPI and headers (install with e.g., `sudo apt install libhidapi-dev`)
 - `gcc`, `make`, `libusb` which should already be present on your system

## Building

Just `make`.  

Step-by-step:

 - Ensure all required packages listed above are installed on your system
 - Open a terminal and navigate to wherever your copy of this repository should live
 - Clone this repository: `git clone https://github.com/tnibler/switchbox-usb`
 - Navigate into the created directory: `cd switchbox-usb`
 - Execute `make`. If everything worked, basically nothing happens.
 - List files in the current directory with `ls -l`. There should be a new file
 called `switchbox`

## Usage

`./switchbox on <VendorID>:<ProductID>` or `./switchbox off <VendorID>:<ProductID>`.

To find the correct Vendor and Product ID, use `lsusb`. 
There will be a line corresponding the Switchbox similar to:

```
Bus 001 Device 002: ID 0001:0000 Fry's Electronics MCP2200 USB Serial Port Emulator
             VendorID --^     ^--ProductID
```

Accessing USB devices typically requires root privileges, which means you either
have to run the `switchbox` program with `sudo` or create `udev` rule to allow
users to access the device.

A command to create a rule that works on most systems is printed by the program:

```
# /etc/udev/rules.d/99-hid-switchbox.rules
SUBSYSTEM=="usb", ATTR{idVendor}=="<VendorID>", ATTR{idProduct}=="<ProductID>", MODE="0666"
```

[MCP2200 datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/SupportingCollateral/93066A.pdf)
