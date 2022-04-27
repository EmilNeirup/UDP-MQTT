/*
        * Module...: MCP9808example1.c
        * Version..: 0.1 (Beta - unfinished)
        * Author...: Henrik Thomsen/Mercantec <heth@mercantec.dk
        * Date.....: 24. nov 2020
        *
        * Abstract.: Implementation of Seeed Grove Sensor "I2C High
        *            Accuracy Temperature sensor" based on MCP-9808
        *            Implementet in user space using libi2c
        * Mod. log.: 01-06-2021: Could not compile. Got i2c_smbus_read_word_data undefined.
                                Include <i2c/smbus.h> should be include. (Worked in 2020)
        * License..: Free open software but WITHOUT ANY WARRANTY.
        * Terms....: see http://www.gnu.org/licenses
        *
        * Documentation - see https://mars.merhot.dk/w/index.php/Grove_I2C_High_Accuracy_Temerature_Sensor_-_Seeed
        *
        * REMEMBER: Install the libi2c-dev package to compile this file
        * Compile this example with: gcc -lmosquitto -li2c displayMessage.c -o displayMessage
        */
        #include <stdio.h>
        #include <unistd.h>
        #include <stdint.h>
        #include <fcntl.h>
        #include <sys/ioctl.h>
        #include <stdlib.h>
        #include <errno.h>
        #include <string.h>
        #include <linux/i2c-dev.h>
        #include <i2c/smbus.h>        // Added 01062021 HeTh
        #include <byteswap.h>
        #include <mosquitto.h>
        #include <stdlib.h>
        #include <unistd.h>

        /* The MCP9808 Digital temperature sensor IC from Microchip is a i2c/SMBus compatible
        * device. To understand this code you need access to MCP9808 Datasheet from Microchip
        *
        * The device is implementet on Seeed Studios Grove I2C high accuracy Temperature Sensor
        *
        * Links and supplemental information can be found on tthe documentation link in this
        * files header.
        *
        * The MCP9808 contains severel registers:
        *
        * Register addresses:
        *  0x00: RFU    - (Reserved for future use) Read only
        *  0x01: CONFIG - Configuration register
        *  0x02: Tupper - Alert temperature Upper boundary trip register
        *  0x03: Tlower - Alert temperature Lower boundary trip register
        *  0x04: Tcrit  - Critical temperature trip register
        *  0x05: Ta     - Temperature register
        *  0x06:        - Manufacurer ID register (Reads 0x54)
        *  0x07:        - Device ID/Revision register
        *  0x08:        - Resolution register
        *  0x09-0x0F    - Reserved....
        */

        #define CONFIG_REG      0x01
        #define TUPPER_REG      0x02
        #define TLOWER_REG      0x03
        #define TCRIT_REG       0x04
        #define TA_REG          0x05
        #define MANID_REG       0x06
        #define DEVID_REG       0x07
        #define RES_REG         0x08

        //Device specific information. (Perhaps better as command options or config file)
        #define MPC9808_BUS     "/dev/i2c-2"
        #define MPC9808_ADR     0x18



        int i2c_init(char *bus, unsigned int address) {
                int file;

                file = open(bus, O_RDWR);
                if (file < 0) { // If error
                        fprintf(stderr, "ERROR: opening %s - %s\n", bus, strerror(errno));
                        exit(1);
                }

                if (ioctl(file, I2C_SLAVE, address) == -1 ) { // If error
                fprintf(stderr, "ERROR: setting  address %d on i2c bus %s with ioctl() - %s", address, bus, strerror(errno) );
                exit(1);
                }
                return(file);
        }


        void on_connect1(struct mosquitto *mosq, void *obj, int result)
        {
        //int rc = MOSQ_ERR_SUCCESS;

        if(!result){
                mosquitto_subscribe(mosq, NULL, "#", 0);  // NOTE: Change to published topic
        }else{
                fprintf(stderr, "%s\n", mosquitto_connack_string(result));
        }
        }

        void display_text(char buf[32])
        {
                int file;
                file = i2c_init(MPC9808_BUS, 0x3e);
                i2c_smbus_write_byte_data(file, 0x00, 0x01); //clear display
                i2c_smbus_write_byte_data(file, 0x00, 0x80); //clear display
                i2c_smbus_write_byte_data(file, 0x00, 0x28); //2 line mode
                i2c_smbus_write_byte_data(file, 0x00, 0x06); //entry mode


                for (int i = 0; ; i++){
                        if (buf[i] == 0x00 )
                                break;
                        if (i == 16)
                                i2c_smbus_write_byte_data(file, 0x00, 0xc0);
                        i2c_smbus_write_byte_data(file, 0x40, buf[i]);
                }
                close(file);
        }

        void on_message1(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
        {
        struct mosquitto *mosq2 = (struct mosquitto *)obj;
                char txtpoi[32];
                sprintf(txtpoi, message->payload);
                if ( txtpoi == 0 ) {
                        fprintf( stderr, "Malloc error\n");
                } else {
                        display_text(txtpoi);
                }
        }

        int main( void ) {
                char revision; //To store MCP9808 revision ID - Perhaps useful in the future
                char buf[32];
                int32_t reg32;
                uint16_t * const reg16poi = (uint16_t *) &reg32; //Address reg32 wordwise
                uint8_t  * const reg8poi  = (uint8_t *)  &reg32; //Address reg32 bytewise
                int file1;
                file1 = i2c_init(MPC9808_BUS, 0x3e);
                i2c_smbus_write_byte_data(file1, 0x00, 0x28); //2 line mode
                i2c_smbus_write_byte_data(file1, 0x00, 0x01); //clear display
                i2c_smbus_write_byte_data(file1, 0x00, 0x06); //entry mode
                close(file1);
                struct mosquitto *mosq1;
                int version[3];

                mosquitto_lib_init();
                mosq1 = mosquitto_new(NULL, 1, NULL);


                mosquitto_connect_callback_set(mosq1, on_connect1);
                mosquitto_message_callback_set(mosq1, on_message1);

                mosquitto_connect(mosq1, "192.168.150.73", 1883, 60);  // NOTE:  Replace localhost with IP address of broker


                mosquitto_loop_forever(mosq1, -1, 1);

                mosquitto_destroy(mosq1);

                mosquitto_lib_cleanup();

                return(0);
        }

