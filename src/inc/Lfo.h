#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Synthesis.h"

class CLfo
{
public:
    CLfo(float fSampleRate, float fWidthInSec, float fFrequency) :
        m_fSampleRate(fSampleRate),
        m_iBufferLength(int(fSampleRate)),
        m_fReadIdx(0),
        m_fWidthInSec(fWidthInSec),
        m_fFrequency(fFrequency),
        m_pCRingBuff(0)
    {
        m_pCRingBuff = new CRingBuffer<float>(m_iBufferLength);
        float *pSineBuffer = new float [m_iBufferLength];
        // generate a wavetable with one period of sine wave
        CSynthesis::generateSine(pSineBuffer, 1.f, 1.f*m_iBufferLength, m_iBufferLength);
        m_pCRingBuff->put(pSineBuffer, m_iBufferLength);
        delete [] pSineBuffer;
        pSineBuffer = 0;
    }

    ~CLfo()
    {
        delete m_pCRingBuff;
        m_pCRingBuff = 0;
    }

    float getNextValue()
    {
        // offset for the vibrato delay line by lookuping up the wavetable
        float fWavetableValue = m_fWidthInSec * m_fSampleRate * m_pCRingBuff->get(m_fReadIdx);
        // update the read index
        float fReadIdxNext = m_fReadIdx + m_fFrequency * (m_iBufferLength / m_fSampleRate) ;
        if(fReadIdxNext >= m_iBufferLength)
            m_fReadIdx = fReadIdxNext - m_iBufferLength;
        else
            m_fReadIdx = fReadIdxNext;

        return fWavetableValue;
    }


private:
    float               m_fSampleRate;
    int                 m_iBufferLength;
    float               m_fReadIdx;
    float               m_fWidthInSec;
    float               m_fFrequency;
    CRingBuffer<float>  *m_pCRingBuff;
};

#endif // #if !defined(__Lfo_hdr__)
