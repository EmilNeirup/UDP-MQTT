// Compile this example with: gcc -lmosquitto -li2c displayMessage.c -o displayMessage

	//Imports
        #include <stdio.h>
        #include <unistd.h>
        #include <stdint.h>
        #include <fcntl.h>
        #include <sys/ioctl.h>
        #include <stdlib.h>
        #include <errno.h>
        #include <string.h>
        #include <linux/i2c-dev.h>
        #include <i2c/smbus.h>
        #include <byteswap.h>
        #include <mosquitto.h>
        #include <stdlib.h>
        #include <unistd.h>

        //Bus path
        #define MPC9808_BUS     "/dev/i2c-2"

	//Initialize i2c device
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


	//Connection error handling
        void on_connect1(struct mosquitto *mosq, void *obj, int result)
        {
        	//int rc = MOSQ_ERR_SUCCESS;
        	if(!result){
                	mosquitto_subscribe(mosq, NULL, "#", 0);  // NOTE: Change to published topic
        	} else {
                	fprintf(stderr, "%s\n", mosquitto_connack_string(result));
       		}
        }

	//Update display text
        void display_text(char buf[32])
        {
                int file;
                file = i2c_init(MPC9808_BUS, 0x3e); 
                i2c_smbus_write_byte_data(file, 0x00, 0x01); //Clear
                i2c_smbus_write_byte_data(file, 0x00, 0x06); //Entry mode 
		i2c_smbus_write_byte_data(file, 0x00, 0x80); //First char

                for (int i = 0; ; i++){
                        if (buf[i] == 0x00 ) //Last char
                                break;
                        if (i == 16) //Second line
                                i2c_smbus_write_byte_data(file, 0x00, 0xc0);
                        i2c_smbus_write_byte_data(file, 0x40, buf[i]);
                }
                close(file);
        }

	//Handle message
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
                struct mosquitto *mosq1;

		//Initialization
                mosquitto_lib_init();
                mosq1 = mosquitto_new(NULL, 1, NULL);

		//Callbacks
                mosquitto_connect_callback_set(mosq1, on_connect1);
                mosquitto_message_callback_set(mosq1, on_message1);

		//Connect to broker
                mosquitto_connect(mosq1, "192.168.150.73", 1883, 60);

		//Keep recieving
                mosquitto_loop_forever(mosq1, -1, 1);

		//Terminate
                mosquitto_destroy(mosq1);
                mosquitto_lib_cleanup();

                return(0);
        }
