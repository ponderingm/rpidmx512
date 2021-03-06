/**
 * @file params.cpp
 *
 */
/* Copyright (C) 2017 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#if defined(__linux__)
 #define ALIGNED
#elif defined(__circle__)
 #define ALIGNED
#else
 #include "util.h"
#endif

#include "params.h"

#include "pca9685.h"

#include "readconfigfile.h"
#include "sscan.h"

#define DMX_START_ADDRESS_MASK	1<<0
#define I2C_SLAVE_ADDRESS_MASK	1<<1
#define BOARD_INSTANCES_MASK	1<<2

static const char PARAMS_DMX_START_ADDRESS[]= "dmx_start_address";
static const char PARAMS_I2C_SLAVE_ADDRESS[]= "i2c_slave_address";
static const char PARAMS_BOARD_INSTANCES[]= "board_instances";

#define PARAMS_BOARD_INSTANCES_MAX	32

Params::Params(const char *pFileName): m_bSetList(0) {
	assert(pFileName != 0);

	m_nI2cAddress = PCA9685_I2C_ADDRESS_DEFAULT;
	m_nDmxStartAddress = 1;
	m_nBoardInstances = 1;

	ReadConfigFile configfile(Params::staticCallbackFunction, this);
	configfile.Read(pFileName);
}

Params::~Params(void) {
}

void Params::staticCallbackFunction(void *p, const char *s) {
	assert(p != 0);
	assert(s != 0);

	((Params *) p)->callbackFunction(s);
}

void Params::callbackFunction(const char *pLine) {
	assert(pLine != 0);

	uint8_t value8;
	uint16_t value16;

	if (Sscan::Uint16(pLine, PARAMS_DMX_START_ADDRESS, &value16) == SSCAN_OK) {
		if ((value16 != 0) && (value16 <= 512)) {
			m_nDmxStartAddress = value16;
			m_bSetList |= DMX_START_ADDRESS_MASK;
		}
		return;
	}

	if (Sscan::I2cAddress(pLine, PARAMS_I2C_SLAVE_ADDRESS, &value8) == SSCAN_OK) {
		if ((value8 >= PCA9685_I2C_ADDRESS_DEFAULT) && (value8 != PCA9685_I2C_ADDRESS_FIXED)) {
			m_nI2cAddress = value8;
			m_bSetList |= I2C_SLAVE_ADDRESS_MASK;
		return;
		}
	}

	if (Sscan::Uint8(pLine, PARAMS_BOARD_INSTANCES, &value8) == SSCAN_OK) {
		if ((value8 != 0) && (value8 <= PARAMS_BOARD_INSTANCES_MAX)) {
			m_nBoardInstances = value8;
			m_bSetList |= BOARD_INSTANCES_MASK;
		}
		return;
	}

}

uint8_t Params::GetI2cAddress(bool *pIsSet) const {
	if (pIsSet != 0) {
		*pIsSet = IsMaskSet(I2C_SLAVE_ADDRESS_MASK);
	}

	return m_nI2cAddress;
}

uint16_t Params::GetDmxStartAddress(bool *pIsSet) const {
	if (pIsSet != 0) {
		*pIsSet = IsMaskSet(DMX_START_ADDRESS_MASK);
	}

	return m_nDmxStartAddress;
}

uint8_t Params::GetBoardInstances(bool *pIsSet) const {
	if (pIsSet != 0) {
		*pIsSet = IsMaskSet(BOARD_INSTANCES_MASK);
	}

	return m_nBoardInstances;
}

void Params::Dump(void) {
	if (m_bSetList == 0) {
		return;
	}

	if(IsMaskSet(DMX_START_ADDRESS_MASK)) {
		printf("%s=%d\n", PARAMS_DMX_START_ADDRESS, m_nDmxStartAddress);
	}

	if(IsMaskSet(I2C_SLAVE_ADDRESS_MASK)) {
		printf("%s=0x%2x\n", PARAMS_I2C_SLAVE_ADDRESS, m_nI2cAddress);
	}

	if(IsMaskSet(BOARD_INSTANCES_MASK)) {
		printf("%s=%d\n", PARAMS_BOARD_INSTANCES, m_nBoardInstances);
	}

}

bool Params::IsMaskSet(uint16_t mask) const {
	return (m_bSetList & mask) == mask;
}

bool Params::GetSetList(void) const {
	return m_bSetList != 0;
}
