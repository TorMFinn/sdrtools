#include <rtl-sdr.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Open the first found SDR device
 * Useful when there is only one device attached
 */
bool open_first_device(rtlsdr_dev_t *dev) {
  int num_devices = rtlsdr_get_device_count();
  if (num_devices == 0) {
    // No known devices available
    return false;
  }

  int ret = rtlsdr_open(&dev, 0);
  if (ret < 0) {
    printf("Failed to open RTL-SDR device\n");
    return false;
  }

  printf("Found %d devices\n", num_devices);
  printf("Device name: %s\n", rtlsdr_get_device_name(0));

  return true;
}

void print_usage() { printf("fm-radio <frequency_in_mhz>\n"); }

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Missing command line argument\n");
    print_usage();
    return EXIT_FAILURE;
  }
  rtlsdr_dev_t *dev = NULL;

  if (!open_first_device(dev)) {
    printf("No devices found :(\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
