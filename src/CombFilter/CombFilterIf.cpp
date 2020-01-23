
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilter.h"
#include "CombFilterIf.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),
    m_pCCombFilter(0),
    m_fSampleRate(0)
{
    // this never hurts
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}

const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create( CCombFilterIf*& pCCombFilter)
{

    pCCombFilter   = new CCombFilterIf ();
    if (!pCCombFilter)
        return kMemError;
    
    return kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter  = 0;
    
    return kNoError;
}

Error_t CCombFilterIf::init( CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels )
{
    if (eFilterType == kCombFIR){
        m_pCCombFilter = new CCombFilterFIR ((int)fMaxDelayLengthInS * fSampleRateInHz, iNumChannels);
    }
    else if(eFilterType == kCombIIR){
        m_pCCombFilter = new CCombFilterIIR ((int)fMaxDelayLengthInS * fSampleRateInHz, iNumChannels);
    }
    
    m_fSampleRate       = fSampleRateInHz;
    m_bIsInitialized    = true;
    
    return kNoError;
}

Error_t CCombFilterIf::reset ()
{
    delete m_pCCombFilter;
    m_fSampleRate       = 0;
    m_bIsInitialized    = false;
    
    return kNoError;
}

Error_t CCombFilterIf::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    return m_pCCombFilter->process(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t CCombFilterIf::setParam( FilterParam_t eParam, float fParamValue )
{
    if (eParam == kParamDelay){
        m_pCCombFilter->setParam(eParam, fParamValue * m_fSampleRate);
    }
    else { // gain
        m_pCCombFilter->setParam(eParam, fParamValue);
    }
    return kNoError;
}

float CCombFilterIf::getParam( FilterParam_t eParam ) const
{
    if (eParam == kParamDelay){
        return m_pCCombFilter->getParam(eParam) / m_fSampleRate;
    }
    else{ // gain
        return m_pCCombFilter->getParam(eParam);
    }
}
