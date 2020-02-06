#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>

/*! \brief implement a circular buffer of type T
*/
template <class T> 
class CRingBuffer
{
public:
    // explicit: no inexplicit type conversion
    explicit CRingBuffer (int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadIdx(0),
        m_iWriteIdx(0)
    {
        assert(iBufferLengthInSamples > 0);

        // allocate and init
        m_ptBuff = new T[iBufferLengthInSamples];
        reset();
        
    }

    virtual ~CRingBuffer ()
    {
        // free memory
        delete [] m_ptBuff;
        m_ptBuff = 0;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc (T tNewValue)
    {
        //write and increment index by one
        put(tNewValue);
        m_iWriteIdx = mod(m_iWriteIdx+1);
    }

    /*! add new values of type T to write index and increment write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void putPostInc (const T* ptNewBuff, int iLength)
    {
        //not now
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        //only write
        m_ptBuff[m_iWriteIdx] = tNewValue;
    }

    /*! add new values of type T to write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void put(const T* ptNewBuff, int iLength)
    {
        //not now
    }
    
    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc ()
    {
        T temp = get();
        m_iReadIdx = mod(m_iReadIdx+1);
        return temp;
    }

    /*! return the values starting at the current read index and increment the read pointer
    \param ptBuff: pointer to where the values will be written
    \param iLength: number of values
    \return void
    */
    void getPostInc (T* ptBuff, int iLength)
    {
        // not now
    }

    /*! return the value at the current read index
    \param fOffset: read at offset from read index
    \return float the value from the read index
    */
    T get (float fOffset = 0.f) const
    {
        // not only read current index
        // can read current index +/- something
        // fractional delay line
        // linear interpolation between two samples
        // for this exercise: interger offset only
        return m_ptBuff[mod(m_iReadIdx + iOffset)];
    }

    /*! return the values starting at the current read index
    \param ptBuff to where the values will be written
    \param iLength: number of values
    \return void
    */
    void get (T* ptBuff, int iLength) const
    {
        // not now
    }
    
    /*! set buffer content and indices to 0
    \return void
    */
    void reset ()
    {
        m_iReadIdx  = 0;
        m_iWriteIdx = 0;
        memset(m_ptBuff, 0, sizeof(T) * m_iBuffLength);
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx () const
    {
        return m_iReadIdx;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx (int iNewWriteIdx)
    {
        m_iWriteIdx = mod(iNewWriteIdx);
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx () const
    {
        return m_iReadIdx;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx (int iNewReadIdx)
    {
         m_iReadIdx = mod(iNewReadIdx);
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    // const: this method can not change the internal state
    // can not write
    int getNumValuesInBuffer () const
    {
        return (m_iWriteIdx - m_iReadIdx + m_iBuffLength) % m_iBuffLength;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength () const
    {
        return m_iBuffLength;
    }
private:
    CRingBuffer ();  // another defualt constructor
    // can't call the defualt constructor without mention how long the buffer will be
    CRingBuffer(const CRingBuffer& that);  // copy constructor (private)
    // copy all the members including pointers, the new class points to the buffer from previous instance, we want to avoid it
    // two ways: lazy way (copy constructor private); complete way (write a copy constructor)

    int m_iBuffLength;              //!< length of the internal buffer
    int m_iReadIdx;
    int m_iWriteIdx;
    T *m_ptBuff;
    
    int mod (int index)
    {
        while(index < 0){
            index += m_iBuffLength;f
        }
        int remainder = index % m_iBuffLength;
        return remainder;
    }
};
#endif // __RingBuffer_hdr__
