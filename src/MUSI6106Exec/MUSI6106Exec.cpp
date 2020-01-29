
#include <iostream>
#include <ctime>
#include <cmath>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

// test functions
int test1(); // FIR: Output is zero if input freq matches feedforward
int test2(); // IIR: amount of magnitude increase/decrease if input freq matches feedback
int test3(); // FIR/IIR: correct result for VARYING input block size
int test4(); // FIR/IIR: correct processing for zero input signal
int test5(); // FIR/IIR: delay length = 0

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;
    
    float                   fDelayTime = 1, fGain = 0;
//    float                   fMaxDelayTime = 1.F;

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
        test1();
        test2();
        test3();
        test4();
        test5();
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
    combFilterType = strcmp(argv[3], "FIR") == 0 ? CCombFilterIf::kCombFIR : CCombFilterIf::kCombIIR;
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
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
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

// test function definitions

int test1(){
    cout << "Test1: " << endl << "FIR: Output is zero if input freq matches feedforward" << endl;
    
    // variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;
    static const int        kBlockSize = 1024;
    float                   fDelayTime = 1, fGain = -1;
    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput = 0;
    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf           *phCombFilter = 0;
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::kCombFIR;
    int                     counter = 0; // for testing
    
    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/sine_440_FIR.wav", CAudioFileIf::kFileWrite, &stFileSpec);
    
    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
    
    // allocate memory
    // input audio
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    // output audio
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];
    
    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
        
        // skip the beginning delaylength chunk
        if (counter > int(fDelayTime * stFileSpec.fSampleRateInHz / kBlockSize)){
            for (int i = 0; i < stFileSpec.iNumChannels; i++){
                for (int j = 0; j < kBlockSize; j++) {
                    if (abs(ppfAudioDataOutput[i][j]) > 0){
                        cout << "Test 1 failed." << endl;
                        return -1;
                    }
                }
            }
        }
        counter++;
    }
    
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
    
    cout << "Test 1 passed." << endl << endl;
    
    return 0;
}


int test2(){
    cout << "Test2: " << endl << "IIR: amount of magnitude increase/decrease if input freq matches feedback" << endl;
    
    // variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;
    static const int        kBlockSize = 1024;
    float                   fDelayTime = 0.1, fGain = 0.5;
    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput = 0;
    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf           *phCombFilter = 0;
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::kCombIIR;
    int                     counter = 0; // for testing
    
    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/sine_440_IIR_increase.wav", CAudioFileIf::kFileWrite, &stFileSpec);
    
    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
    
    // allocate memory
    // input audio
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    // output audio
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];
    
    // increasing amplitude test
    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
        
        // skip the beginning delaylength chunk
        if (counter > int(fDelayTime * stFileSpec.fSampleRateInHz / kBlockSize)){
            for (int i = 0; i < stFileSpec.iNumChannels; i++){
                for (int j = 0; j < kBlockSize; j++) {
                    if (abs(ppfAudioDataInput[i][j])!= 0 && (abs(ppfAudioDataOutput[i][j]) <= abs(ppfAudioDataInput[i][j]))){
                        cout << "Test 2 magnitude increase failed." << endl;
                        return -1;
                    }
                }
            }
        }
        counter++;
    }
    
    CCombFilterIf::destroy(phCombFilter);
    CAudioFileIf::destroy(phAudioFileInput);
    CAudioFileIf::destroy(phAudioFileOutput);
    
    // decreasing amplitude test
    fDelayTime = 0.1;
    fGain = -0.5;
    counter = 0;

    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/sine_440_IIR_decrease.wav", CAudioFileIf::kFileWrite, &stFileSpec);

    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);

    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);

        // skip the beginning delaylength chunk
        if (counter > int(fDelayTime * stFileSpec.fSampleRateInHz / kBlockSize)){
            for (int i = 0; i < stFileSpec.iNumChannels; i++){
                for (int j = 0; j < kBlockSize; j++) {
                    if (abs(ppfAudioDataInput[i][j])!= 0 && (abs(ppfAudioDataOutput[i][j]) >= abs(ppfAudioDataInput[i][j]))){
                        cout << "Test 2 magnitude decrease failed." << endl;
                        return -1;
                    }
                }
            }
        }
        counter++;
    }
    
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
    
    cout << "Test 2 passed." << endl << endl;
    
    return 0;
}

int test3(){
    cout << "Test3: " << endl << "FIR/IIR: correct result for VARYING input block size" << endl;
    
    // variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;
    float                   fDelayTime = 0.1, fGain = 0.5;
    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput;
    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf           *phCombFilter = 0;
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::kCombFIR;
    int kBlockSize = 1024;
    
    for(int i = 0; i < 4; i++){
    
        switch(i){
            case 0:
                combFilterType = CCombFilterIf::kCombFIR;
                kBlockSize = 1024;
                sOutputFilePath = "/Users/Christine/Desktop/test/audio/sine_440_FIR_1024.wav";
                break;
            case 1:
                combFilterType = CCombFilterIf::kCombFIR;
                kBlockSize = 512;
                sOutputFilePath = "/Users/Christine/Desktop/test/audio/sine_440_FIR_512.wav";
                break;
            case 2:
                combFilterType = CCombFilterIf::kCombIIR;
                kBlockSize = 1024;
                sOutputFilePath = "/Users/Christine/Desktop/test/audio/sine_440_IIR_1024.wav";
                break;
            case 3:
                combFilterType = CCombFilterIf::kCombIIR;
                kBlockSize = 512;
                sOutputFilePath = "/Users/Christine/Desktop/test/audio/sine_440_IIR_512.wav";
                break;
                
        }
           
        // open the input sine wave file
        CAudioFileIf::create(phAudioFileInput);
        phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
        phAudioFileInput->getFileSpec(stFileSpec);
        phAudioFileInput->setPosition((long long) 0);

        
        // open the output wave file
        CAudioFileIf::create(phAudioFileOutput);
        phAudioFileOutput->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
        
        // instantiate CCombFilterIf
        CCombFilterIf::create(phCombFilter);
        phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
        phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
        phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
        
        // allocate memory
        // input audio
        ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++)
            ppfAudioDataInput[i] = new float[kBlockSize];
        
        // output audio
        ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];
        
        // process audio
        while (!phAudioFileInput->isEof())
        {
            long long iNumFrames = kBlockSize;
            phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
            phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
            phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
        }
        
        CCombFilterIf::destroy(phCombFilter);
        CAudioFileIf::destroy(phAudioFileInput);
        CAudioFileIf::destroy(phAudioFileOutput);
        }

        for (int i = 0; i < stFileSpec.iNumChannels; i++){
           delete[] ppfAudioDataInput[i];
           delete[] ppfAudioDataOutput[i];
        }
        delete[] ppfAudioDataInput;
        delete[] ppfAudioDataOutput;
        ppfAudioDataInput = 0;
        ppfAudioDataOutput = 0;
        
        cout << "Test 3 passed." << endl << endl;
        return 0;
}


int test4(){
    cout << "Test4: " << endl << "FIR/IIR: correct processing for zero input signal" << endl;
    
    // variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;
    static const int        kBlockSize = 1024;
    float                   fDelayTime = 0.1, fGain = 0.5;
    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput = 0;
    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf           *phCombFilter = 0;
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::kCombFIR;
    
    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/silence.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/silence_FIR.wav", CAudioFileIf::kFileWrite, &stFileSpec);
    
    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
    
    // allocate memory
    // input audio
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    // output audio
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];
    
    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
        
        for (int i = 0; i < stFileSpec.iNumChannels; i++){
            for (int j = 0; j < kBlockSize; j++) {
                if (ppfAudioDataOutput[i][j] != 0){
                    cout << "Test 4 FIR failed." << endl;
                    return -1;
                }
            }
        }
    }
    
    CCombFilterIf::destroy(phCombFilter);
    CAudioFileIf::destroy(phAudioFileInput);
    CAudioFileIf::destroy(phAudioFileOutput);
    
    // IIR test
    combFilterType = CCombFilterIf::kCombIIR;
    
    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/silence.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/silence_IIR.wav", CAudioFileIf::kFileWrite, &stFileSpec);

    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);

    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);

        for (int i = 0; i < stFileSpec.iNumChannels; i++){
            for (int j = 0; j < kBlockSize; j++) {
                if (ppfAudioDataOutput[i][j] != 0){
                    cout << "Test 4 IIR failed." << endl;
                    return -1;
                }
            }
        }
    }

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
    
    cout << "Test 4 passed." << endl << endl;
    
    return 0;
}

int test5(){
    cout << "Test5: " << endl << "FIR/IIR: gain = 0" << endl;
    
    // variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;
    static const int        kBlockSize = 1024;
    float                   fDelayTime = 1, fGain = 0;
    float                   **ppfAudioDataInput = 0, **ppfAudioDataOutput = 0;
    CAudioFileIf            *phAudioFileInput = 0, *phAudioFileOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf           *phCombFilter = 0;
    
    // FIR test
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::kCombIIR;
    
    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/sine_440_FIR_gain0.wav", CAudioFileIf::kFileWrite, &stFileSpec);
    
    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);
    
    // allocate memory
    // input audio
    ppfAudioDataInput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataInput[i] = new float[kBlockSize];
    
    // output audio
    ppfAudioDataOutput = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioDataOutput[i] = new float[kBlockSize];
    
    // increasing amplitude test
    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);
        
        for (int i = 0; i < stFileSpec.iNumChannels; i++){
            for (int j = 0; j < kBlockSize; j++) {
                if (ppfAudioDataInput[i][j] != ppfAudioDataOutput[i][j]){
                    cout << "Test 5 FIR failed." << endl;
                    return -1;
                }
            }
        }
    }
    
    CCombFilterIf::destroy(phCombFilter);
    CAudioFileIf::destroy(phAudioFileInput);
    CAudioFileIf::destroy(phAudioFileOutput);
    
    // IIR test
    combFilterType = CCombFilterIf::kCombFIR;

    // open the input sine wave file
    CAudioFileIf::create(phAudioFileInput);
    phAudioFileInput->openFile("/Users/Christine/Desktop/test/audio/sine_440.wav", CAudioFileIf::kFileRead);
    phAudioFileInput->getFileSpec(stFileSpec);
    
    // open the output wave file
    CAudioFileIf::create(phAudioFileOutput);
    phAudioFileOutput->openFile("/Users/Christine/Desktop/test/audio/sine_440_IIR_gain0.wav", CAudioFileIf::kFileWrite, &stFileSpec);

    // instantiate CCombFilterIf
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(combFilterType, fDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::kParamGain, fGain);
    phCombFilter->setParam(CCombFilterIf::kParamDelay, fDelayTime);

    // process audio and check
    while (!phAudioFileInput->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFileInput->readData(ppfAudioDataInput, iNumFrames);
        phCombFilter->process(ppfAudioDataInput, ppfAudioDataOutput, iNumFrames);
        phAudioFileOutput->writeData(ppfAudioDataOutput, iNumFrames);

        for (int i = 0; i < stFileSpec.iNumChannels; i++){
            for (int j = 0; j < kBlockSize; j++) {
                if (ppfAudioDataInput[i][j] != ppfAudioDataOutput[i][j]){
                    cout << "Test 5 IIR failed." << endl;
                    return -1;
                }
            }
        }
    }
    
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
    
    cout << "Test 5 passed." << endl << endl;
    
    return 0;
}

void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

