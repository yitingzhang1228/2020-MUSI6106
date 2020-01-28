#include <cassert>
#include <iostream>

#include "Util.h"
#include "Vector.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

CCombFilterBase::CCombFilterBase(int maxDelayLength, int iNumChannels) :
    maxDelayLength(maxDelayLength),
    iNumChannels(iNumChannels)
{
    circularBuffer = new Buffer(maxDelayLength, iNumChannels);
    circularBuffer->init();
}

CCombFilterBase::~CCombFilterBase()
{
    delete circularBuffer;
}

Error_t CCombFilterBase::setParam (int eParam, float fParamValue)
{
    if (eParam == CCombFilterIf::kParamGain) {
        gain = fParamValue;
    }
    else{ //delay
        maxDelayLength = int(fParamValue);
    }
    return kNoError;
}

float   CCombFilterBase::getParam (int eParam) const
{
    if (eParam == CCombFilterIf::kParamGain) {
        return gain;
    }
    else{ //delay
        return (float)maxDelayLength;
    }
}

Error_t CCombFilterFIR::process(float **ppfAudioDataIn, float **ppfAudioDataOut, int iNumFrames)
{
    float *sample = new float[iNumChannels];
    // for each incoming sample in the block
    for (int i=0; i<iNumFrames; i++) {
        circularBuffer->read(sample);
        // for each channel
        for (int j=0; j<iNumChannels; j++) {
            ppfAudioDataOut[j][i] = ppfAudioDataIn[j][i] + gain * sample[j];
        }
        circularBuffer->write(ppfAudioDataIn, i);
    }
    return kNoError;
}

Error_t CCombFilterIIR::process(float **ppfAudioDataIn, float **ppfAudioDataOut, int iNumFrames)
{
    float *sample = new float[iNumChannels];
    for (int i=0; i<iNumFrames; i++) {
        circularBuffer->read(sample);
        for (int j=0; j<iNumChannels; j++) {
            ppfAudioDataOut[j][i] = ppfAudioDataIn[j][i] + gain * sample[j];
        }
        circularBuffer->write(ppfAudioDataOut, i);
    }
    return kNoError;
}


// circular buffer class

Buffer::Buffer(int size, int numChannels):
    size(size),
    numChannels(numChannels)
{
    head = 0;
    buffer = new float *[numChannels];
    for(int i = 0; i < numChannels; i++) {
        buffer[i] = new float[size];
    }
}

Buffer::~Buffer(){
    for(int i = 0; i < numChannels; i++){
        delete[] buffer[i];
    }
    delete[] buffer;
}
    
// initialize the buffer to zeros
void Buffer::init() {
        for(int i = 0; i < numChannels; i++) {
            for (int j=0; j<size; j++)
                buffer[i][j] = 0;
        }
}

void Buffer::read(float *sample) {
    for (int i = 0; i < numChannels; i++) {
        sample[i] = buffer[i][head];
    }
}

void Buffer::write(float **input, int index) {
    for(int i = 0; i < numChannels; i++) {
        buffer[i][head] = input[i][index];
    }
    head = (head + 1) % size;
}
