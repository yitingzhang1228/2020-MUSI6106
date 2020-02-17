
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"
// #include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioDataInput = 0;
    float                   **ppfAudioDataOutput = 0;

    CAudioFileIf            *phAudioFileInput = 0;
    CAudioFileIf            *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    CVibrato                *pCVibrato = 0;
    float                   fWidthInSec = 0;
    float                   fFrequency = 0;
    const float             fMaxDelayInSec = 0.5;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 5)
    {
        cout << "Missing arguments!" << endl;
        cout << "Usage: InputFilePath OutputFilePath ModWidthInSec ModFrequency" << endl;
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
        fWidthInSec = atof(argv[3]);
        fFrequency = atof(argv[4]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFileInput->isOpen())
    {
        cout << "Input wave file open error!";
        return -1;
    }
    phAudioFileInput->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wav file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phAudioFileOutput->isOpen())
    {
        cout << "Output wave file open error!";
        return -1;
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // instantiate CVibrato
    CVibrato::create(pCVibrato);
    pCVibrato->init(fMaxDelayInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    pCVibrato->setParam(CVibrato::kParamWidthInSec, fWidthInSec);
    pCVibrato->setParam(CVibrato::kParamFrequency, fFrequency);
    pCVibrato->initLfo();
    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];

    time = clock();
    
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        cout << "\r" << "reading and writing";
        pCVibrato->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);

    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFileInput);
    CAudioFileIf::destroy(phAudioFileOutput);
    CVibrato::destroy(pCVibrato);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
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
