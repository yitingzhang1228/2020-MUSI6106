
#if !defined(__Dtw_HEADER_INCLUDED__)
#define __Dtw_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"

/*! \brief basic dynamic time warping (http://www.audiocontentanalysis.org/code/helper-functions/dtw/)
*/
class CDtw
{
public:
    enum MatrixDimension_t
    {
        kRow,
        kCol,

        kNumMatrixDimensions
    };

    enum Directions_t
    {
        kHoriz,
        kVert,
        kDiag,

        kNumDirections
    };
    
    CDtw(void);
    virtual ~CDtw(void);

    /*! initializes the class with the size of the distance matrix
    \param iNumRows
    \param iNumCols
    \return Error_t
    */
    Error_t init (int iNumRows, int iNumCols);
    
    /*! resets all internal class members
    \return Error_t
    */
    Error_t reset ();

    /*! computes cost and path w/o back-tracking
    \param ppfDistanceMatrix (dimensions [rows][columns])
    \return Error_t
    */
    Error_t process (float **ppfDistanceMatrix);
 
    /*! returns the length of the path
    \return int
    */
    int getPathLength ();
    
    /*! returns the overall cost
    \return float
    */
    float getPathCost () const;
    
    /*! returns the path \sa getPathLength
    \param ppiPathResult pointer to memory the result is written to (dimensions [2][length_of_path])
    \return Error_t
    */
    Error_t getPath (int **ppiPathResult) const;
    
    /*! returns the minimum of directions in cost matrix
    \return float
    */
    float getMinCost (float horiz, float vert, float diag, CDtw::Directions_t& kDirection);
    
private:
    
    enum DtwParam_t
    {
        xLength,
        yLength,

        kDtwParams
    };
    
    float **ppfCostMatrix;
    int   **m_ppiPathResult;
    
    Directions_t **ppfDirectionsMatrix;
    bool  m_bIsInitialized;
    
    int   m_iNumRows;
    int   m_iNumCols;
    
    float m_fpathCost;
    int   m_iPathLength;
};


#endif