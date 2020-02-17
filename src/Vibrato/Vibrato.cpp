// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"
#include "Lfo.h"
#include "Vibrato.h"

CVibrato::CVibrato () :
    m_bIsInitialized(false),
    m_fSampleRate(0),  
    m_iNumChannels(0),
    m_fWidthInSec(0),
    m_fFrequency(0),
    m_fMaxWidthInSec(0),
    m_ppCRingBuffer(0),
    m_pCLfo(0)
{
    this->reset ();
}

CVibrato::~CVibrato ()
{
    this->reset ();
}

Error_t CVibrato::create (CVibrato*& pCVibrato)
{
    pCVibrato = new CVibrato ();

    if (!pCVibrato)
        return kUnknownError;

    return kNoError;
}

Error_t CVibrato::destroy (CVibrato*& pCVibrato)
{
    if (!pCVibrato)
        return kUnknownError;
    
    pCVibrato->reset ();
    
    delete pCVibrato;
    pCVibrato = 0;

    return kNoError;
}

Error_t CVibrato::init (float fMaxWidthInSec, float fSampleRateInHz, int iNumChannels)
{
    m_fSampleRate = fSampleRateInHz;
    m_iNumChannels = iNumChannels;
    m_fMaxWidthInSec = fMaxWidthInSec;
    
    m_ppCRingBuffer = new CRingBuffer<float>*[m_iNumChannels];
    for(int i=0; i<iNumChannels; i++)
    {
        m_ppCRingBuffer[i] = new CRingBuffer<float>(CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate*2+1));
        m_ppCRingBuffer[i]->setWriteIdx(CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate));
    }
    
    m_bIsInitialized = true;
    
    return kNoError;
}

Error_t CVibrato::initLfo ()
{
    m_pCLfo = new CLfo(m_fSampleRate, m_fWidthInSec , m_fFrequency);
    return kNoError;
}

Error_t CVibrato::reset ()
{
    for (int i=0; i<m_iNumChannels; i++)
        delete m_ppCRingBuffer[i];
    delete [] m_ppCRingBuffer;
    m_ppCRingBuffer = 0;
    
    delete m_pCLfo;
    m_pCLfo = 0;
    
    m_fSampleRate = 0;
    m_iNumChannels = 0;
    m_fWidthInSec = 0;
    m_fFrequency = 0;
    
    m_bIsInitialized = false;

    return kNoError;
}

Error_t CVibrato::setParam (VibratoParam_t eParam, float fParamValue)
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    
    switch (eParam)
    {
    case kParamWidthInSec:
        m_fWidthInSec = fParamValue;
        if(m_fWidthInSec>m_fMaxWidthInSec){
            return kFunctionInvalidArgsError;
        }
            break;
           
    case kParamFrequency:
        m_fFrequency = fParamValue;
        break;
    }
    return kNoError;
}

float   CVibrato::getParam (VibratoParam_t eParam) const
{
    switch (eParam)
    {
    case kParamWidthInSec:
            return m_fWidthInSec;
    case kParamFrequency:
            return m_fFrequency;
    }
    return kNoError;
}

Error_t CVibrato::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for(int i=0; i<iNumberOfFrames; i++)
    {
        float fOffset = m_pCLfo->getNextVal();
        for (int j=0; j<m_iNumChannels; j++)
        {
            m_ppCRingBuffer[j]->putPostInc(ppfInputBuffer[j][i]);
            ppfOutputBuffer[j][i] = m_ppCRingBuffer[j]->get(-fOffset);
            m_ppCRingBuffer[j]->getPostInc();
            
        }
    }
    return kNoError;
}

