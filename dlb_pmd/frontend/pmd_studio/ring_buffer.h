/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <mutex>
#include <vector>
#include <memory>
#include <cstring>
#include <stdexcept>

#include "pmd_studio_common_defs.h"

extern "C"{
#include "pmd_smpte_337m.h"
}

static const unsigned int numBuffers = 3; // Active, queued and editable

class PMDStudioRingBufferList
{
    class PMDStudioRingBuffer
    {

        bool enabled;
        uint32_t *buffers[numBuffers]; 
        std::mutex ringBufferMutex;

        unsigned int startChannel;
        unsigned int numChannels;
        unsigned int nextSampleIndex;
        unsigned int maxPcmBufSamples;
        unsigned int frameRateCadenceIndex;
        pmd_studio_video_frame_rate frameRate;
        unsigned int numSamples;
        unsigned int bufSizeBytes;
        unsigned int activeBufSizeBytes;        
        // Three references
        int active;
        int queued;
        int edited;

    public:

        PMDStudioRingBuffer(
            unsigned int newstartChannel): // Number of SMPTE frames (samples / channels)
            enabled(false),
            startChannel(newstartChannel),
            numChannels(0),
            nextSampleIndex(0),
            maxPcmBufSamples(0),
            frameRateCadenceIndex(0),
            frameRate(INVALID_FRAME_RATE),
            numSamples(0),
            bufSizeBytes(0),
            activeBufSizeBytes(0),
            active(-1),
            queued(-1),
            edited(-1)
        {
            for (unsigned int i = 0 ; i < numBuffers ; i++)
            {
                buffers[i] = nullptr;
            }
        }

        ~PMDStudioRingBuffer()
        {
            for (unsigned int i = 0 ; i < numBuffers ; i++)
            {
                delete[] buffers[i];
            }
        }

        void Enable( unsigned int newNumChannels,
                     pmd_studio_video_frame_rate newFrameRate)
        {
            unsigned int maxNumSamples;

            // frameRate = INVALID signals data mode and not wrapping
            // frame rate timing will be handled by the stream
            if (newFrameRate == INVALID_FRAME_RATE)
            {
                maxNumSamples = MAX_DATA_BYTES / sizeof(uint32_t);
                numSamples = maxNumSamples;
            }
            else
            {
                maxNumSamples = newNumChannels * pmd_studio_video_frame_rate_max_frames[newFrameRate];
                numSamples = pmd_studio_video_frame_rate_cadence[newFrameRate][frameRateCadenceIndex++] * newNumChannels;
           }

            if ((newNumChannels == 0) ||
                (newNumChannels == 0))
            {
                throw std::runtime_error("number of ring buffer channels or frames can't be zero");
            }
            ringBufferMutex.lock();
            // Rescale buffers if required
            ;
            if (maxNumSamples > maxPcmBufSamples)
            {
                for (unsigned int i = 0 ; i < numBuffers ; i++)
                {
                    if (buffers[i] != nullptr)
                    {
                        delete[] buffers[i];
                    }
                    buffers[i] = new uint32_t[maxNumSamples];
                }
                maxPcmBufSamples = maxNumSamples;
            }
            numChannels = newNumChannels;
            bufSizeBytes = numSamples * sizeof(uint32_t);
            activeBufSizeBytes = bufSizeBytes;
            nextSampleIndex = 0;
            frameRateCadenceIndex = 0;
            frameRate = newFrameRate;
            enabled = true;
            active = queued = edited = -1;
            ringBufferMutex.unlock();
        }

        bool isEnabled()
        {
            return(enabled);
        }

        void Disable(void)
        {
            ringBufferMutex.lock();
            active = queued = edited = -1;
            enabled = false;
            ringBufferMutex.unlock();
        }


        unsigned int GetNumChannels(void)
        {
            return(numChannels);
        }

        unsigned int GetStartChannel(void)
        {
            return(startChannel);
        }

        unsigned int GetWordIndex(void)
        {
            return(nextSampleIndex);
        }

        unsigned int GetBufferFrames(void)
        {
            return(numSamples / numChannels);
        }

        pmd_studio_video_frame_rate GetFrameRate(void)
        {
            return frameRate;
        }

        void *GetBufferForUpdate(unsigned int &retBufSizeBytes)
        {
            ringBufferMutex.lock();
            if (!enabled)
            {
                ringBufferMutex.unlock();
                throw std::runtime_error("Tried to get buffer from disabled ring buffer");
            }
            // Create new buffer 
            int newForEdited = 0;
            while (((newForEdited == active) || (newForEdited == queued)) && (newForEdited < (int)numBuffers))
            {
                newForEdited++;
            }
            if (newForEdited == numBuffers)
            {
                ringBufferMutex.unlock();                
                throw std::runtime_error("Can't find free buffer");
            }
            edited = newForEdited;
            ringBufferMutex.unlock();
            retBufSizeBytes = bufSizeBytes;             
            return (buffers[edited]);
        }

        // This commits the edits that have taken place
        // The buffer is queued for update at the end of the next frame
        void CommitUpdate(void)
        {
            ringBufferMutex.lock();
            if (edited < 0)
            {
                ringBufferMutex.unlock();        
                return;
            }
            queued = edited;
            edited = -1;
            ringBufferMutex.unlock();        
        }

        // This commits the edits that have taken place
        // The buffer is queued for update at the end of the next frame
        // This version provides an option to shrink the buffer to fit
        // the data so eliminating the padding normally associated with the ring buffer
        void CommitUpdate(unsigned int newBufSizeBytes)
        {
            ringBufferMutex.lock();
            if (edited < 0)
            {
                ringBufferMutex.unlock();        
                return;
            }
            queued = edited;
            edited = -1;
            unsigned int newPcmBufSamples = ceil(newBufSizeBytes / (float)sizeof(int32_t));
            activeBufSizeBytes = newBufSizeBytes; // avoids round up to sample
            // Check that requested size is not more than allocated size
            if (newPcmBufSamples > maxPcmBufSamples)
            {
                throw std::runtime_error("Trying to commit larger buffer than exists");

            }
            else if (newPcmBufSamples < maxPcmBufSamples)
            {
                numSamples = newPcmBufSamples;
                bufSizeBytes = numSamples * sizeof(int32_t);
            }
            ringBufferMutex.unlock();        
        }


        // lock
        void Lock()
        {
            ringBufferMutex.lock();
        }

        void Unlock()
        {
            ringBufferMutex.unlock();
            // Now see if there are any queued updates
        }

        // Get exact bytes in the buffer with no sample padding
        unsigned int GetEntireActiveBufferBytes(uint32_t*& ptr)
        {
            if (active < 0)
            {
                if (queued >= 0)
                {
                    active = queued;
                    queued = -1;                
                }
                else
                {
                    ptr = NULL;
                    return(0);
                }
            }
            ptr = &buffers[active][0];

            if (queued >= 0)
            {
                active = queued;
                queued = -1;
            }

            return(activeBufSizeBytes);
        }

        uint32_t GetNextWord()
        {
            if (frameRate == INVALID_FRAME_RATE)
            {
                throw std::runtime_error("INVALID_FRAME_RATE");
            }
            if (active < 0)
            {
                if (queued >= 0)
                {
                    active = queued;
                    queued = -1;                
                }
                else
                {
                    return(0);
                }
            }
            uint32_t nextWord = buffers[active][nextSampleIndex++];
            if (nextSampleIndex >= numSamples)
            {
                nextSampleIndex = 0;
                numSamples = pmd_studio_video_frame_rate_cadence[frameRate][frameRateCadenceIndex++] * numChannels;
                if (frameRateCadenceIndex >= NUM_VIDEO_FRAME_RATE_CADENCE)
                {
                    frameRateCadenceIndex = 0;
                }
                if (queued >= 0)
                {
                    active = queued;
                    queued = -1;
                }
            }
            return(nextWord);
        }

        uint32_t PeekNextWord()
        {
            if (active < 0)
            {
                return(0);
            }
            return(buffers[active][nextSampleIndex]);
        }

        void PrintDebug(void)
        {
            ringBufferMutex.lock();
            printf("\tStart Channel: %u\n", GetStartChannel());
            printf("\tNumber of Channel: %u\n", GetNumChannels());
            printf("\tFrame Rate: %ffps\n", pmd_studio_video_frame_rate_floats[frameRate]);
            printf("\tFrame Rate Cadence Index: %u\n", frameRateCadenceIndex);
            printf("\tCurrent buffer wrap point: %u\n", pmd_studio_video_frame_rate_cadence[frameRate][frameRateCadenceIndex]);
            printf("\tindex: %u\n", GetWordIndex());
            printf("\tBuffer Size in Frames: %u\n", GetBufferFrames());
            ringBufferMutex.unlock();
        }
    };

    std::vector<std::shared_ptr<PMDStudioRingBuffer>> bufferVector;
    std::mutex listMutex;


public:
    PMDStudioRingBufferList( unsigned int numChannels) // Maximum number of channels in the system    
    {
        if (numChannels == 0)
        {
            throw std::runtime_error("Can't have zero length ring buffer list");
        }

        // Create a set of mono 
        for (unsigned int i = 0 ; i < numChannels ; i++)
        {
            std::shared_ptr<PMDStudioRingBuffer> tmpPtr = std::make_shared<PMDStudioRingBuffer>(i);
            bufferVector.push_back(tmpPtr);
        }
    }

    void AddRingBuffer(
        unsigned int startChannel,
        unsigned int numChannels,
        pmd_studio_video_frame_rate frameRate)
    {
        listMutex.lock();
        bufferVector[startChannel]->Enable(numChannels, frameRate);
        listMutex.unlock();
    }


    void DeleteRingBuffer(unsigned int startChannel)
    {
        listMutex.lock();
        bufferVector[startChannel]->Disable();
        listMutex.unlock();
    }

    void CommitUpdate(unsigned int startChannel)
    {
        bufferVector[startChannel]->CommitUpdate();
    }

    void CommitUpdate(unsigned int startChannel, unsigned int newBufSizeBytes)
    {
        bufferVector[startChannel]->CommitUpdate(newBufSizeBytes);        
    }

    void *GetBufferForUpdate(unsigned int startChannel, unsigned int &bufSizeBytes)
    {
        return(bufferVector[startChannel]->GetBufferForUpdate(bufSizeBytes));
    }

    void WriteRingBuffers(int32_t *outputBuffer, unsigned int outBufChannelCount, unsigned int framesToWrite)
    {
        // This lock is required to protect iterator from list deletion
        listMutex.lock();
        for (std::vector<std::shared_ptr<PMDStudioRingBuffer>>::iterator i = bufferVector.begin() ; i < bufferVector.end() ; i++)
        {   
            (*i)->Lock();
            // If frame rate is invalid then this is a -41 stream and shoud not be included in the outgoing multiplex
            if ((*i)->isEnabled() && ((*i)->GetFrameRate() != INVALID_FRAME_RATE))
            {
                int32_t *writePtr = (int32_t *) outputBuffer;
                writePtr += (*i)->GetStartChannel();
                unsigned int numRingBufferChannels = (*i)->GetNumChannels();
                for( unsigned int j = 0; j < framesToWrite ; j++ )
                {
                    for (unsigned int k = 0 ; k < numRingBufferChannels ; k++)
                    {
                        *writePtr++ = (*i)->GetNextWord();
                    }
                    writePtr += outBufChannelCount - numRingBufferChannels;
                }
            }
            (*i)->Unlock();
        }
        listMutex.unlock();
    }

    unsigned int CopyEntireActiveBufferBytes(unsigned int startChannel, uint32_t *destPtr)
    {
        uint32_t *srcPtr;
        unsigned int numBytes = bufferVector[startChannel]->GetEntireActiveBufferBytes(srcPtr);
        memcpy(destPtr, srcPtr, numBytes);
        return(numBytes);
    }


    void Reset(void)
    {
        for (std::vector<std::shared_ptr<PMDStudioRingBuffer>>::iterator i = bufferVector.begin() ; i < bufferVector.end() ; i++)
        {
            (*i)->Disable();
        }
    }

    void PrintDebug(void)
    {
        listMutex.lock();
        printf("Number of ring buffers: %lu\n", bufferVector.size());
        unsigned int j = 1;
        for (std::vector<std::shared_ptr<PMDStudioRingBuffer>>::iterator i = bufferVector.begin() ; i < bufferVector.end() ; i++)
        {
            printf("Ring Buffer#%u\n---- --------\n", j++);
            (*i)->PrintDebug();
        }
        listMutex.unlock();
    }


};

#endif // __RING_BUFFER_H__

