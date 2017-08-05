/*
 * ArduinoWire.cpp
 *
 * Implementation of the ArduinoWire class
 *
 *  Created on: Apr 4, 2016
 *      Author: mario
 */

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#include "ArduinoWire.h"

extern "C" {
//	__s32 i2c_smbus_read_byte(int);
//	__s32 i2c_smbus_write_byte(int, __u8);
//	__s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length, __u8 *values);
//	__s32 i2c_smbus_write_block_data(int file, __u8 command, __u8 length, const __u8 *values);
//	struct i2c_msg;
//
//	__s32 i2c_smbus_read_word_data(int file, __u8 command); //works with repetition restart
//	__s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value);
//	__s32 i2c_smbus_write_i2c_block_data(int file, __u8 command, __u8 length,
//						    const __u8 *values);
//
//	__s32 i2c_smbus_access(int file, char read_write, __u8 command,
//	           int size, union i2c_smbus_data *data);
}

ArduinoWire::ArduinoWire(): fd(0), txAddress(0), txBufferIndex(0), txBufferLength(0), rxBufferIndex(0), rxBufferLengh(0), releaseBus(true) {
	this->fd = open("/dev/i2c-0", O_RDWR);

	if (this->fd == -1) {
		std::cerr<<"Could not open i2c device."<<std::endl;
		exit(1);
	}
}

void ArduinoWire::becomeBusMaster() {
	if (txAddress == 0) {
		std::cerr<<"Could not communicate with slave device at address: "<<txAddress<<std::endl;
		exit(1);
	}
	if (ioctl(this->fd, I2C_SLAVE, txAddress) < 0) {
			std::cerr<<"Could not communicate with slave device at address: "<<txAddress<<std::endl;
			exit(1);
	}

}
void ArduinoWire::begin(){
	txBufferIndex = 0;
	txBufferLength = 0;

	rxBufferIndex = 0;
	rxBufferLengh = 0;
}

//void ArduinoWire::begin(uint8_t address) {
//	txAddress = address;
//
//	begin();
//}

void ArduinoWire::beginTransmission(const uint8_t address){
	txAddress = address;
}

size_t ArduinoWire::write(const uint8_t data){
	if (txBufferLength >= BUFFER_LENGTH) {
		std::cerr<<"txBuffer is full"<<std::endl;
		return 0;
	}

	txBuffer[txBufferIndex] = data;
	txBufferIndex++;
	txBufferLength = txBufferIndex;

	return 1;
}

size_t ArduinoWire::write(const uint8_t * data, size_t size){
	int res = 0;
	for (uint8_t i=0; i<size; i++) {
		res += this->write(data[i]);
	}

	return res;
}

//FIXME not really sending the STOP bit.check if there are bugs in i2c device...
// We simply restart the connection as master next time
// this function is called
int ArduinoWire::endTransmission(uint8_t sendStop){
	int res = 0;

	if (releaseBus) {
		becomeBusMaster();
		releaseBus = false;
	}

	for (int i=0; i< txBufferLength; i++) {
		res += i2c_smbus_write_byte(this->fd, txBuffer[i]);
                // usleep(1000);
	}
	if (res != txBufferLength) {
		//std::cerr<<"Transmission fail: txBufferLength: "<<txBufferLength<<"\t transmitted: "<<res<<std::endl;
	}
	txBufferIndex = 0;
	txBufferLength = 0;

	releaseBus = sendStop;

	return res;
}

int ArduinoWire::requestFrom(int address, int read_len, bool releaseBus){
	int read = 0;
	read = i2c_smbus_read_i2c_block_data(fd, address, read_len, rxBuffer);
	rxBufferIndex = 0;
	rxBufferLengh = read;

	if (read == read_len) {
		std::cout<<"read OK"<<std::endl;
	} else {
		std::cerr<<"read not OK"<<std::endl;
	}

	this->releaseBus = releaseBus;

	return read;
}

int ArduinoWire::available(){
	if (rxBufferLengh - rxBufferIndex >= 1) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t ArduinoWire::read(){
	if (rxBufferIndex >= rxBufferLengh) {
		std::cerr<<"No more data."<<std::endl;
		return 0;
	}
	return rxBuffer[rxBufferIndex++];
}




