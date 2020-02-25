#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"
#include "Synthesis.h"
#include "Vector.h"
#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()  :
            m_pVibrato(0),
            m_ppfInputData(0),
            m_ppfOutputData(0),
            m_iDataLength(35131),
            m_fMaxWidthInSec(0.5),
            m_iBlockLength(171),
            m_iNumChannels(3),
            m_fSampleRate(8000),
            m_fWidthInSec(0.2),
            m_fFrequency(5)
        {
            CVibrato::create(m_pVibrato);
            m_pVibrato->init(m_fMaxWidthInSec,m_fSampleRate,m_iNumChannels);

            m_ppfInputData = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp = new float*[m_iNumChannels];
            m_ppfOutputTmp = new float*[m_iNumChannels];
            
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i] = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i] = new float [m_iDataLength];
                CVector::setZero(m_ppfOutputData[i], m_iDataLength);
            }

        }

        ~VibratoData()
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;

            CVibrato::destroy(m_pVibrato);
        }

        void Testprocess ()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c] = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        CVibrato    *m_pVibrato;
        float       **m_ppfInputData,
                    **m_ppfOutputData,
                    **m_ppfInputTmp,
                    **m_ppfOutputTmp;
        int         m_iDataLength;
        float       m_fMaxWidthInSec;
        int         m_iBlockLength;
        int         m_iNumChannels;
        float       m_fSampleRate;
        float       m_fWidthInSec;
        float       m_fFrequency;

    };

    //Output equals delayed input when modulation amplitude is 0.
    TEST_FIXTURE(VibratoData, ZeroModWidth)
    {
        m_pVibrato->setParam(CVibrato::kParamFrequency, 5);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0);
        m_pVibrato->initLfo();

        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 440.F, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));

        Testprocess();

        int iDelay = CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_iDataLength-iDelay, 1e-3);
    }
    
    //DC input stays DC ouput regardless of parametrization.
    TEST_FIXTURE(VibratoData, DCInput)
    {
        m_pVibrato->setParam(CVibrato::kParamFrequency, 5);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0.2);
        m_pVibrato->initLfo();
        
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CSynthesis::generateDc(m_ppfInputData[c], m_iDataLength, 0.5);
        }

        Testprocess();

        int iDelay = CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate);
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_iDataLength-iDelay, 1e-3);
        }
    }

    //Varying input block size.
    TEST_FIXTURE(VibratoData, VaryingBlockSize)
    {
        m_pVibrato->setParam(CVibrato::kParamFrequency, 5);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0.2);
        m_pVibrato->initLfo();

        Testprocess();

        m_pVibrato->reset();
        
        m_pVibrato->init(m_fMaxWidthInSec,m_fSampleRate,m_iNumChannels);
        m_pVibrato->setParam(CVibrato::kParamFrequency, 5);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0.2);
        m_pVibrato->initLfo();
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(static_cast<float>(iNumFramesRemaining), static_cast<float>(rand())/RAND_MAX*17000.F);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    }
    
    // Zero input signal.
    TEST_FIXTURE(VibratoData, ZeroInput)
    {
        m_pVibrato->setParam(CVibrato::kParamFrequency, 5);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0.2);
        m_pVibrato->initLfo();

        for (int c = 0; c < m_iNumChannels; c++)
            CVector::setZero(m_ppfInputData[c], m_iDataLength);

        Testprocess();

        int iDelay = CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_iDataLength-iDelay, 1e-3);
    }
    
    // Output equals delayed input when modulation frequency is 0.
    TEST_FIXTURE(VibratoData, ZeroModFreq)
    {
        m_pVibrato->setParam(CVibrato::kParamFrequency, 0);
        m_pVibrato->setParam(CVibrato::kParamWidthInSec, 0.2);
        m_pVibrato->initLfo();
        
        for (int c = 0; c < m_iNumChannels; c++)
        CSynthesis::generateSine (m_ppfInputData[c], 440.F, m_fSampleRate, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));

        Testprocess();

        int iDelay = CUtil::float2int<int>(m_fMaxWidthInSec*m_fSampleRate);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_iDataLength-iDelay, 1e-3);
    }

}

#endif //WITH_TESTS
