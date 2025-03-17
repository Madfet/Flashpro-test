#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/libftdi1/ftdi.h"

#define MCHP_FP5_VID                (0x1514)
#define MCHP_FP5_PID                (0x2008)

#define DD0_CTRL0                   (1)
#define DD1_CTRL1                   (2)
#define DD2_CTRL2                   (4)
#define DD3_CTRL3                   (8)
#define DD4_PROG_MODE               (0x10)
#define DD5_VJTAG_SPI_PWR_ENB_OFF   (0x20) /* (Low - on, High - Off */
#define DD6_VJTAG_DEBUG_PWR_ENB_OFF (0x40) /* (Low - on, High - Off */
#define DD7_VPUMP_PWR_ENB_OFF       (0x80) /* (Low - on, High - Off */

#define ERROR_OK                    (0)
#define ERROR_NO_CONFIG_FILE        (-2)
#define ERROR_BUF_TOO_SMALL         (-3)
#define ERROR_FAIL                  (-4)
#define ERROR_WAIT                  (-5)

int main(int argc, char *argv[])
{
	struct ftdi_device_list *devlist, *curdev;
	struct ftdi_context ftdic;
	unsigned char pins = ( DD0_CTRL0 | DD1_CTRL1 | DD2_CTRL2 | DD3_CTRL3 | DD6_VJTAG_DEBUG_PWR_ENB_OFF );
	uint16_t ftdi_vid  = MCHP_FP5_VID;
	uint16_t ftdi_pid  = MCHP_FP5_PID;
	int err = ERROR_OK;

	err = ftdi_init(&ftdic);
    if(err)
    {
		printf("ftdi_init() failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
		return err;
	}
    else
    {
        err = ftdi_usb_find_all(&ftdic, &devlist, ftdi_vid, ftdi_pid);
        if (err < 0)
        {
            printf("ftdi_usb_find_all() failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
            return err;
        } else if (err == 0) {
            printf("Found no FP5 devices\r\n");
            return err;
        } else {
            printf("Found %d FP5 device%s\r\n", err, err > 1 ? "s" : "");
            curdev = devlist;
            while (curdev) {
                err = ftdi_set_interface(&ftdic, INTERFACE_D);
                if (err < 0) {
                    printf("ftdi_set_interface failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }

                err = ftdi_usb_open_dev(&ftdic, curdev->dev);
                if (err < 0) {
                    printf("ftdi_usb_open_dev failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }
                printf("Opened FP5's interface D\r\n");

                err = ftdi_set_bitmode(&ftdic, 0xff, 0x01);
                if (err) {
                    printf("ftdi_set_bitmode failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }

                err = ftdi_write_data(&ftdic, &pins, sizeof(pins));
                if (err != sizeof(pins)) {
                    printf("ftdi_write_data failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }

                err = ftdi_read_data(&ftdic, &pins, sizeof(pins));
                if (err != sizeof(pins))
                {
                    printf("ftdi_read_data failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }

                printf("Set pins on interface D to 0x%x\r\n", pins);

                err = ftdi_usb_close(&ftdic);
                if (err) 
                {
                    printf("ftdi_usb_close failed: %d (%s)\r\n", err, ftdi_get_error_string(&ftdic));
                    exit(0);
                }
                printf("Closed FP5's interface D\r\n");

                curdev = curdev->next;
            }
            ftdi_list_free(&devlist);
        }
    }

	return ERROR_OK;
}
