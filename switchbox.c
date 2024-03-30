#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <hidapi/hidapi.h>

const unsigned short DEFAULT_VENDOR = 0x0001;
const unsigned short DEFAULT_PRODUCT = 0x0000;

const char usage[] = "Usage:\n\
%s on|off|get [vendor:product]\n\
vendor and product id in format 12ab:cd45 as output by lsusb\n\
Default: %04x:%04x\n\n\
Unless you have udev rules set up, this program must probably run as root.\n";

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    goto print_help;
  }

  int set = 0;
  if (!strcmp("on", argv[1]) || !strcmp("ON", argv[1])) {
    set = 1;
  } else if (!strcmp("off", argv[1]) || !strcmp("OFF", argv[1])) {
    set = 0;
  } else if (!strcmp("get", argv[1]) || !strcmp("GET", argv[1])) {
    set = -1;
  } else {
    goto print_help;
  }

  unsigned short vendor = DEFAULT_VENDOR;
  unsigned short product = DEFAULT_PRODUCT;
  if (argc == 3) {
    char *p = argv[2];
    int ok = sscanf(p, "%hx:%hx", &vendor, &product);
    if (ok != 2) {
      goto print_help;
    }
  }

  int res;
  hid_device *handle;
  int i;

  // Initialize the hidapi library
  res = hid_init();
  if (res < 0) {
    fprintf(stderr, "Error initializing HIDAPI:\n%ls\n", hid_error(NULL));
    return 1;
  }

  handle = hid_open(vendor, product, NULL);
  if (!handle) {
    fprintf(stderr,
            "Unable to open device.\nThis program needs privileges to access "
            "USB devices."
            " Try re-running as root or set up udev rules to allow user acess "
            "to the USB device.\n"
            "The command below creates a udev rule that should work on many "
            "systems. Paste into your terminal, at your own risk:\n\n"
            "echo \"SUBSYSTEM==\\\"usb\\\", ATTR{idVendor}==\\\"%04x\\\", "
            "ATTR{idProduct}==\\\"%04x\\\", MODE=\\\"0666\\\"\" | sudo tee "
            "/etc/udev/rules.d/99-hid-switchbox.rules",
            vendor, product);
    hid_exit();
    return 1;
  }

  unsigned char buf[16] = {0};

  if (set == -1) {
    // READ_ALL command
    buf[0] = 0x80;
    res = hid_write(handle, buf, 16);
    if (res < 0) {
      fprintf(stderr, "Error sending READ_ALL command to HID device:\n%ls\n", hid_error(handle));
      return 1;
    }
    res = hid_read(handle, buf, 16);
    if (res < 0) {
      fprintf(stderr, "Error reading response from to HID device:\n%ls\n", hid_error(handle));
      return 1;
    }
    // byte 10 of response contains bitmap of IO port values
    // we care about pin 3
    int state = buf[10] & 0x8;
    printf("%s\n", state ? "on" : "off");
  } else if (set == 0 || set == 1) {
    // CONFIGURE command opcode
    buf[0] = 0x10;
    // the rest of the buffer is all 0 i.e., configure all pins as output.
    res = hid_write(handle, buf, 16);
    if (res < 0) {
      fprintf(stderr,
              "Error sending initial CONFIGURE command to HID device:\n%ls\n",
              hid_error(handle));
      return 1;
    }

    // SET_CLEAR_OUTPUT command opcode
    buf[0] = 0x08;
    if (set) {
      // set pin 3 high
      buf[11] = 0x08;
    } else {
      // clear all pins
      buf[12] = 0xff;
    }
    res = hid_write(handle, buf, 16);
    if (res < 0) {
      fprintf(stderr,
              "Error sending SET_CLEAR_OUTPUT command to HID device:\n%ls\n",
              hid_error(handle));
      return 1;
    }
  }

  hid_close(handle);
  res = hid_exit();

  return 0;

print_help:
  printf(usage, argv[0], DEFAULT_VENDOR, DEFAULT_PRODUCT);
  return 1;
}
