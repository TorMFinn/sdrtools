#include <rtl-sdr.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define TUNER_BANDWIDTH 200000
#define SAMPLE_RATE 1000000

rtlsdr_dev_t *rtl_device = NULL;

void sighandler(int signum) {
  (void)signum;

  if (rtl_device) {
    rtlsdr_cancel_async(rtl_device);
  }
}

/**
 * Open the first found SDR device
 * Useful when there is only one device attached
 */
bool open_first_device(rtlsdr_dev_t **dev) {
  int num_devices = rtlsdr_get_device_count();
  if (num_devices == 0) {
    // No known devices available
    return false;
  }

  int ret = rtlsdr_open(dev, 0);
  if (ret < 0) {
    printf("Failed to open RTL-SDR device, reason: %s\n", strerror(errno));
    return false;
  }

  printf("Found %d devices\n", num_devices);
  printf("Device name: %s\n", rtlsdr_get_device_name(0));

  return true;
}

bool tune_rtl_device(rtlsdr_dev_t **device, float frequency_mhz) {
  if (device == NULL) {
    printf("device is not initialized\n");
    return false;
  }

  int ret;

  // Set tuner to auto gain.
  ret = rtlsdr_set_tuner_gain_mode(*device, 0);
  if (ret < 0) {
    printf("failed to set tuner gain mode: %s\n", strerror(errno));
    return false;
  }

  // Set RTL to auto gain.
  ret = rtlsdr_set_agc_mode(*device, 1);
  if (ret != 0) {
    printf("failed to set RTLSDR gain mode: %s\n", strerror(errno));
    return false;
  }

  // Set 200 khz as default bandwidth.
  // Which is the maximum standard bandwith for FM transmissions.
  // Currently automatic bandwith control.
  ret = rtlsdr_set_tuner_bandwidth(*device, 0);
  if (ret != 0) {
    printf("failed to set tuner bandwith\n");
    return false;
  }

  ret = rtlsdr_set_sample_rate(*device, SAMPLE_RATE);
  if (ret != 0) {
    printf("failed to set sample rate\n");
    return false;
  }

  ret = rtlsdr_set_center_freq(*device, (uint32_t) frequency_mhz * 10e6);
  if (ret != 0) {
    printf("failed to tune to frequency: %f mhz\n", frequency_mhz);
    return false;
  }

  return true;
}

static void read_async_callback(unsigned char *buf, uint32_t len, void *ctx) {
  printf("Received data :), size: %d\n", len);
}

void print_usage() { printf("fm-radio <frequency_in_mhz>\n"); }

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Missing command line argument\n");
    print_usage();
    return EXIT_FAILURE;
  }

  if (!open_first_device(&rtl_device)) {
    printf("No devices found :(\n");
    return EXIT_FAILURE;
  }

  float frequency_mhz = atof(argv[1]);
  if (frequency_mhz == 0) {
    printf("Invalid frequency %s\n", argv[1]);
    goto exit;
  }

  if (!tune_rtl_device(&rtl_device, frequency_mhz)) {
    printf("Failed to setup SDR device\n");
    goto exit;
  }

  // Start async reception
  rtlsdr_reset_buffer(rtl_device);
  rtlsdr_read_async(rtl_device, read_async_callback, NULL, 0, 0);

 exit:

  rtlsdr_close(rtl_device);

  return EXIT_SUCCESS;
}
