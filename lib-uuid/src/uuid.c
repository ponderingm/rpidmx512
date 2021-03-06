/**
 * @file uuid.c
 *
 */
/* Copyright (C) 2016 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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

#include <stdint.h>

#include "bcm2835_rng.h"
#include "util.h"

#include "uuid/uuid.h"

typedef union pcast32 {
	uuid_t uuid;
	uint32_t u32[4];
} _pcast32;

/**
 *
 * @param out
 */
void uuid_generate_random(uuid_t out) {
	_pcast32 cast;

	cast.u32[0] = bcm2835_rng_get_number();
	cast.u32[1] = bcm2835_rng_get_number();
	cast.u32[2] = bcm2835_rng_get_number();
	cast.u32[3] = bcm2835_rng_get_number();

	cast.uuid[6] = 0x40 | (cast.uuid[6] & 0xf);
	cast.uuid[8] = 0x80 | (cast.uuid[8] & 0x3f);

	memcpy(out, cast.uuid, sizeof(uuid_t));
}


