/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

#ifndef __AM824_FRAMER_H__
#define __AM824_FRAMER_H__

#include <stdint.h>

#define CHANNEL_STATUS_BYTES 24

#define WIDTH  (8)
#define BOTTOMBIT 1
#define REFLECTED_POLYNOMIAL 0xb8 // Unreflected is 0x1d

typedef enum { AM824_ERR_OK = 0, AM824_ERR_BAD_SAMPLING_FREQUENCY = -1, AM824_ERR_UNSUPPORTED_BITDEPTH = -2 } AM824ErrorCode;

typedef enum { FS_NOT_INDICATED = 0, FS_44100_HZ = 1, FS_48000_HZ = 2, FS_32000_HZ = 3 } AM824SamplingFrequency;

typedef enum { AM824_BIG_ENDIAN, AM824_LITTLE_ENDIAN } AM824Endianess;

typedef struct
{
	uint8_t channelStatusIndex;
	uint8_t channelStatusMask;
	uint8_t channelStatus[CHANNEL_STATUS_BYTES];
	uint8_t subFrameCounter;
	uint8_t numChannels;
	uint8_t bitDepth;
	uint8_t crcTable[256];
	AM824Endianess endian;
} AM824Framer;


/* Helper functions */

static inline uint8_t getParity(unsigned int n) 
{ 
    uint8_t parity = 0; 
    while (n) 
    { 
        parity = 1 - parity; 
        n     = n & (n - 1); 
    }      
    return parity; 
}

static inline void crcTableInit(AM824Framer *framer)
{
    uint8_t remainder;

    for (uint16_t dividend = 0; dividend < 256; ++dividend)
    {
    	remainder = dividend << (WIDTH - 8);
    	for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (remainder & BOTTOMBIT)
            {
                remainder = (remainder >> 1) ^ REFLECTED_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder >> 1);
            }
        }
        framer->crcTable[dividend] = remainder;
    }
}

static inline void setCRC(AM824Framer *framer)
{
   uint8_t data;
   uint8_t remainder = 0xff;

   for (int byte = 0; byte < 23; byte++)
    {
        data = framer->channelStatus[byte] ^ remainder;
        remainder = framer->crcTable[data];
    }
    framer->channelStatus[23] = remainder;
}

/* Public Interface */
/* Input number of channels and the bitdepth of the input samples
	Note that the output bit depth is always 24 bit */

static inline AM824ErrorCode AM824Framer_init(AM824Framer *framer, 
			uint8_t newNumChannels, 		/* Input - Number of channels of input/output audio */
		    uint8_t newBitDepth, 			/* Input - Bit depth of input audio, output is always 32 bit */ 
		    AM824Endianess outputEndianess  /* Input = Endianess of output samples, input is always machine order */
		    )                               /* Returns - error code */
{
	uint8_t i;
	framer->channelStatusIndex = 0;
	framer->channelStatusMask = 1;
	framer->numChannels = newNumChannels;
	framer->subFrameCounter = 0;

	framer->bitDepth = newBitDepth;
	framer->endian = outputEndianess;
	// Set certain channel status bits
	// Clear it first
	for (i = 0 ; i < CHANNEL_STATUS_BYTES ; i++)
	{
		framer->channelStatus[i] = 0;
	}
	// Default Channel Status
	// Professional Bit
	framer->channelStatus[0] |= 1;
	// Non-audio PCM
	framer->channelStatus[0] |= 1 << 1;
	// 48kHz
	framer->channelStatus[0] |= 2 << 6;

	switch(framer->bitDepth)
	{
		case 16:
			framer->channelStatus[2] |= 1 << 3;
			break;
		case 20:
			// Use of Auxillary bits
			framer->channelStatus[2] |= 4;
			// 20 bit data
			framer->channelStatus[2] |= 1 << 3;
			break;
		case 24:
			// Use of Auxillary bits
			framer->channelStatus[2] |= 4;
			// 24 bit data
			framer->channelStatus[2] |= 5 << 3;
			break;
		default:
			return(AM824_ERR_UNSUPPORTED_BITDEPTH);
	}
	crcTableInit(framer);
	setCRC(framer);
	return(AM824_ERR_OK);
}

static inline AM824ErrorCode setSamplingFrequency(AM824Framer *framer, AM824SamplingFrequency fs_code)
{
	if (fs_code > FS_32000_HZ)
	{
		return(AM824_ERR_BAD_SAMPLING_FREQUENCY);
	}
	// Reset top two bits and set accordingly
	framer->channelStatus[0] &= 0x3f; 
	framer->channelStatus[0] |= fs_code << 6;
	setCRC(framer);
	return(AM824_ERR_OK);
}

static inline void setProfessionalMode(AM824Framer *framer)
{
	framer->channelStatus[0] |= 1;
	setCRC(framer);
}

static inline void setConsumerMode(AM824Framer *framer)
{
	framer->channelStatus[0] &= 0xfe;
	setCRC(framer);
}

static inline void setAudioMode(AM824Framer *framer)
{
	framer->channelStatus[0] &= 0xfd;
	setCRC(framer);
}

static inline void setDataMode(AM824Framer *framer)
{
	framer->channelStatus[0] |= 2;
	setCRC(framer);
}

static inline void getAM824Sample(AM824Framer *framer, uint32_t inputSample, uint8_t *outputBytes)
{
	uint32_t outputSample;
	unsigned int channelStatusBit = framer->channelStatus[framer->channelStatusIndex] & framer->channelStatusMask;
	unsigned int userBit = 0;
	unsigned int validityBit = 0;

	// Input samples are MSB justified as per AES3
	if (framer->bitDepth == 16)
	{
		outputSample = inputSample << 8;
	}
	else
	{
		outputSample = inputSample;
	}

	// Detect block start
	if ((framer->channelStatusIndex == 0) && (framer->channelStatusMask == 1))
	{
		outputSample |= 1 << 29;
	}
	// Detect frame start
	if (framer->subFrameCounter == 0)
	{
		outputSample |= 1 << 28;			
	}

	outputSample |= channelStatusBit << 26;
	outputSample |= userBit << 25;
	outputSample |= validityBit << 24;
	
	// Now complete except for parity so calculate that
	outputSample |= getParity(outputSample) << 27;

	// Now complete all the wraparound checks
	// Note that channel status chan be different for the different subframes (channels)
	// but in this example channel status is set to be the same for all subframes (channels)
	framer->subFrameCounter++;
	if (framer->subFrameCounter == framer->numChannels)
	{
		framer->subFrameCounter = 0;
		// Move to next channel status bit
		if (framer->channelStatusMask == 0x80)
		{
			framer->channelStatusMask = 1;
			framer->channelStatusIndex++;
			if (framer->channelStatusIndex == CHANNEL_STATUS_BYTES)
			{
				framer->channelStatusIndex = 0;
			}
		}
		else
		{
			framer->channelStatusMask = framer->channelStatusMask << 1;
		}
	}
	if (framer->endian == AM824_BIG_ENDIAN)
	{
		// Return in 32 bit Big Endian format
		*outputBytes++ = (uint8_t)((outputSample & 0xff000000) >> 24);
		*outputBytes++ = (uint8_t)((outputSample & 0x00ff0000) >> 16);
		*outputBytes++ = (uint8_t)((outputSample & 0x0000ff00) >> 8);
		*outputBytes++ = (uint8_t)((outputSample & 0x000000ff) >> 0);
	}
	else
	{
		// Return in 32 bit Little Endian format
		*outputBytes++ = (uint8_t)((outputSample & 0x000000ff) >> 0);
		*outputBytes++ = (uint8_t)((outputSample & 0x0000ff00) >> 8);
		*outputBytes++ = (uint8_t)((outputSample & 0x00ff0000) >> 16);
		*outputBytes++ = (uint8_t)((outputSample & 0xff000000) >> 24);
	}
}

/* Simple test code to check CRC implementation */
/* See EBU Tech 3250 or AES3 for the reference for these examples */
static inline void testCRC(AM824Framer *framer)
{
	unsigned int i;
	for (i = 0 ; i < CHANNEL_STATUS_BYTES ; i++)
	{
		framer->channelStatus[i] = 0;
	}
	// From AES3-2-2009-r2019 - Example 1
	framer->channelStatus[0] = 0x3d;
	framer->channelStatus[1] = 2;
	framer->channelStatus[4] = 2;
	setCRC(framer);
	if (framer->channelStatus[23] == 0x9b)
	{
		printf("Example 1 - passed\n");
	}
	else
	{
		printf("Example 1 - failed, expecting 0x9b, got 0x%x\n",framer->channelStatus[23]);
	}
	framer->channelStatus[0] = 0x01;
	framer->channelStatus[1] = 0;
	framer->channelStatus[4] = 0;
	setCRC(framer);
	if (framer->channelStatus[23] == 0x32)
	{
		printf("Example 2 - passed\n");
	}
	else
	{
		printf("Example 2 - failed, expecting 0x32, got 0x%x\n",framer->channelStatus[23]);
	}		
}

#endif
