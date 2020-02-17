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
        //static Error_t generateSine (float *pfOutBuf, float fFreqInHz, float fSampleFreqInHz, int iLength, float fAmplitude = 1.F, float fStartPhaseInRad = 0.F)
        CSynthesis::generateSine(pSineBuffer, 1.f, 1.f*m_iBufferLength, m_iBufferLength);
        m_pCRingBuff->put(pSineBuffer, m_iBufferLength);
        delete [] pSineBuffer;
    }

    ~CLfo()
    {
        delete m_pCRingBuff;
    }

    float getNextVal()
    {
        float fVal = m_fWidthInSec * m_fSampleRate * m_pCRingBuff->get(m_fReadIdx);
        float fReadIdxNext = m_fReadIdx + m_fFrequency / m_fSampleRate * m_iBufferLength;
        if(fReadIdxNext >= m_iBufferLength)
            m_fReadIdx = fReadIdxNext - m_iBufferLength;
        else
            m_fReadIdx = fReadIdxNext;

        return fVal;
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
