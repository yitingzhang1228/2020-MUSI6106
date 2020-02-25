#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Lfo.h"


/*
 * The vibrato class has ring buffer objects for each channel of the audio and a LFO object.
 * The delay length of vibrato is set to maximum LFO width allowed.
 *
 * To use the Vibrato class, use the following procedures:
 * 1. create(CVibrato*& pCVibrato): create a vibrato instance and init the ring buffer
 * 2. init(float fMaxWidthInSec, float fSampleRateInHz, int iNumChannels): initialize the vibrato instance
 * 3. setParam(VibratoParam_t eParam, float fParamValue): set modulating width in seconds and modulating frequency in Hz
 * 4. initLfo(): initialize the Lfo after setting the LFO parameters
 * 5. process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames): process audio block by block
 * 6. destroy (CVibrato*& pCVibrato): destroy the vibrato class
 * You can also use reset() to reset the vibrato instance.
 * You can also use getParam(VibratoParam_t eParam) to access parameter values.
 */


class CVibrato
{
public:

    /*! list of parameters for vibrato*/
    enum VibratoParam_t
    {
        kParamWidthInSec,              //!< LFO modulating amplitude in seconds
        kParamFrequency,               //!< LFO modulating frequency in Hz
    };

    /*! creates a new vibrato instance
    \param pCVibrato pointer to the new class
    \return Error_t
    */
    static Error_t create (CVibrato*& pCVibrato);
    
    /*! destroys a vibrato instance
    \param pCVibrato pointer to the class to be destroyed
    \return Error_t
    */
    static Error_t destroy (CVibrato*& pCVibrato);
    
    /*! initializes a vibrato instance
    \param fMaxWidthInSec maximum allowed LFO width in seconds
    \param fSampleRateInHz sample rate in Hz
    \param iNumChannels number of audio channels
    \return Error_t
    */
    Error_t init (float fMaxWidthInSec, float fSampleRateInHz, int iNumChannels);
    
    /*! initializes LFO
    \return Error_t
    */
    Error_t initLfo ();
    
    /*! resets the internal variables (requires new call of init)
    \return Error_t
    */
    Error_t reset ();

    /*! sets a vibrato parameter
    \param eParam what parameter (see ::VibratoParam_t)
    \param fParamValue value of the parameter
    \return Error_t
    */
    Error_t setParam (VibratoParam_t eParam, float fParamValue);
    
    /*! return the value of the specified parameter
    \param eParam
    \return float
    */
    float   getParam (VibratoParam_t eParam) const;
    
    /*! processes one block of audio
    \param ppfInputBuffer input buffer [numChannels][iNumberOfFrames]
    \param ppfOutputBuffer output buffer [numChannels][iNumberOfFrames]
    \param iNumberOfFrames buffer length (per channel)
    \return Error_t
    */
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
protected:
    CVibrato ();
    virtual ~CVibrato ();
    
private:
    bool                m_bIsInitialized;   //!< internal bool to check whether the init function has been called

    float               m_fSampleRate;      //!< audio sample rate in Hz
    int                 m_iNumChannels;     //!< number of channels
    
    float               m_fWidthInSec;      //!< LFO modulation width in seconds
    float               m_fFrequency;       //!< LFO modulation frequency in Hz
    float               m_fMaxWidthInSec;   //!< FLO max modulation width in seconds
    
    CRingBuffer<float>  **m_ppCRingBuffer;  //!< ring buffer
    CLfo                *m_pCLfo;           //!< LFO
    
};

#endif // #if !defined(__Vibrato_hdr__)
