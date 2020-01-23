
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath,
                            sFilterType;

    static const int        kBlockSize = 1024;
    
    float                   fDelayTime = 0, fGain = 0;
    float                   fMaxDelayTime = 1.F;

    clock_t                 time = 0;

    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput = 0;

    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;

    CAudioFileIf::FileSpec_t stFileSpec;
    
    CCombFilterIf           *phCombFilter = 0;
    
    CCombFilterIf::CombFilterType_t combFilterType;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc == 1)
    {
        cout << "Running tests." << endl;
        // TODO
        return -1;
    }
    else if (argc < 6)
    {
        cout << "Missing arguments!" << endl;
        cout << "Usage: InputFilePath OutputFilePath FilterType DelayTime Gain" << endl;
        return -1;
    }
    else
    {
    sInputFilePath = argv[1];
    sOutputFilePath = argv[2];
    sFilterType = strcmp(argv[3], "FIR") == 0 ? CCombFilterIf::kCombFIR : CCombFilterIf::kCombIIR;
    fDelayTime = atof(argv[4]);
    fGain = atof(argv[5]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFileInput->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFileInput->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phAudioFileOutput->isOpen())
    {
        cout << "Output wave file open error!";
        return -1;
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fMaxDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    // input audio
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    // output audio
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];

    time = clock();
    
    //////////////////////////////////////////////////////////////////////////////
    // process audio
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean up
    CAudioFileIf::destroy(phAudioFileInput);
    CAudioFileIf::destroy(phAudioFileOutput);
    CCombFilterIf::destroy(phCombFilter);

    for (int i = 0; i < stFileSpec.iNumChannels; i++){
        delete[] ppfAudioDataInput[i];
        delete[] ppfAudioDataOutput[i];
    }
    delete[] ppfAudioDataInput;
    delete[] ppfAudioDataOutput;
    ppfAudioDataInput = 0;
    ppfAudioDataOutput = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

