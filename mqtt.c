#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
    	printf("%s\n ", argv[1]);
	return 0
	
}

int setText(unsigned int file, char *text) {
        //Function set 0010 1000 // 2 line mode and 5x8
        i2c_smbus_write_byte_data(file, 0x00, 0x28);
        i2c_smbus_write_byte_data(file, 0x00, 0x0D);
        i2c_smbus_write_byte_data(file, 0x00, 0x01);
        i2c_smbus_write_byte_data(file, 0x00, 0x06);
        i2c_smbus_write_byte_data(file, 0x00, 0x02);

        i2c_smbus_write_byte_data(file, 0x00, 0x80);
        for(int i = 0; ; i++) {
                if(text[i] == 0x00)
                        break;
                if(i == 15)
                        i2c_smbus_write_byte_data(file, 0x00, 0xc0);
                i2c_smbus_write_byte_data(file, 0x40, text[i]);
        }

}

