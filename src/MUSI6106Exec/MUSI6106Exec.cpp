
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"

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

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    // Usage: ./MUSI6106Exec sweep.wav sweep.txt
    sInputFilePath = argv[1];
    sOutputFilePath = argv[2];
    
    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    
    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath, std::fstream::out);
 
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = (float**) malloc (sizeof(float*) * 2);
    ppfAudioData[0] = (float*) malloc (sizeof(float) * kBlockSize);
    ppfAudioData[1] = (float*) malloc (sizeof(float) * kBlockSize);
 
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    while (!phAudioFile->isEof()){
        long long int iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        for (int i = 0; i < iNumFrames; i++){
            hOutputFile << ppfAudioData[0][i] << '\t' << ppfAudioData[1][i] << endl;
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    free(ppfAudioData[0]);
    free(ppfAudioData[1]);
    free(ppfAudioData);
    hOutputFile.close();
    CAudioFileIf::destroy(phAudioFile);

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

