#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "CombFilterIf.h"
#include "ErrorDef.h"

class Buffer;

// base class
class CCombFilterBase
{
public:
    CCombFilterBase (int maxDelayLength, int iNumChannels);
    virtual ~CCombFilterBase ();
    
    Error_t setParam (int eParam, float fParamValue);
    float   getParam (int eParam) const;
    
    // pure virtual function, overwritten in derived class
    virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) = 0;

protected:
    int maxDelayLength;
    int iNumChannels;
    int test;
    float gain;
    Buffer *circularBuffer;
    
};

// CCombFilterFIR inherits CCombFilterBase
class CCombFilterFIR : public CCombFilterBase
{
public:
    // do the constructor explicitly
    CCombFilterFIR (int maxDelayLength, int iNumChannels):CCombFilterBase(maxDelayLength, iNumChannels){};
    virtual ~CCombFilterFIR (){};

    // implements FIR process
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};

// CCombFilterIIR inherits CCombFilterBase
class CCombFilterIIR : public CCombFilterBase
{
public:
    // do the constructor explicitly
    CCombFilterIIR (int maxDelayLength, int iNumChannels):CCombFilterBase(maxDelayLength, iNumChannels){};
    virtual ~CCombFilterIIR (){};
    
    // implements IIR process
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};


// circular buffer class

class Buffer{
public:
    Buffer(int size, int numChannels);
    ~Buffer();
    void init();
    void read(float *sample);
    void write(float **input, int index);
    
private:
    int size;
    int numChannels;
    int head;
    float **buffer;
};


#endif  //__CombFilter_hdr__

