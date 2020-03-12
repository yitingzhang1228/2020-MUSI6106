
#include "Vector.h"
#include "Util.h"

#include "Dtw.h"

CDtw::CDtw( void )
{

}

CDtw::~CDtw( void )
{
    if(m_bIsInitialized)
        reset();
}

Error_t CDtw::init( int iNumRows, int iNumCols )
{
    if (iNumRows <= 0 || iNumCols <= 0)
        return kFunctionInvalidArgsError;
    
    m_iNumRows = iNumRows;
    m_iNumCols = iNumCols;
    m_fpathCost   = 0.f;
    m_iPathLength = 0;
        
    m_ppfCostMatrix = new float *[m_iNumRows];
    for (int i = 0; i < m_iNumRows; i++)
    {
        m_ppfCostMatrix[i] = new float [m_iNumCols];
        for (int j = 0; j < m_iNumCols; j++)
            m_ppfCostMatrix[i][j] = 0;
    }
    
    m_ppiPathResult = new int *[2];
    for (int i = 0; i < 2; i++)
    {
        m_ppiPathResult[i] = new int [m_iNumRows + m_iNumCols - 2];
        for (int j = 0; j < m_iNumRows + m_iNumCols - 2; j++){
            m_ppiPathResult[i][j] = 0;
        }
    }
    
    m_ppfDirectionsMatrix = new Directions_t *[m_iNumRows];
    for (int i = 0; i < m_iNumRows; i++)
    {
        m_ppfDirectionsMatrix[i] = new Directions_t [m_iNumCols];
        for (int j = 0; j < m_iNumCols; j++)
            m_ppfDirectionsMatrix[i][j] = Directions_t::kDiag;
    }
    
    m_bIsInitialized = true;
    
    return kNoError;
}

Error_t CDtw::reset()
{
    m_fpathCost = 0.f;
    m_iPathLength = 0;
    
    for (int i= 0; i < m_iNumRows; i++)
        delete m_ppfCostMatrix[i];
    delete [] m_ppfCostMatrix;
    m_ppfCostMatrix = 0;
    
    for (int i= 0; i < 2; i++)
        delete m_ppiPathResult[i];
    delete [] m_ppiPathResult;
    m_ppiPathResult = 0;

    for (int i= 0; i < m_iNumRows; i++)
        delete m_ppfDirectionsMatrix[i];
    delete [] m_ppfDirectionsMatrix;
    m_ppfDirectionsMatrix = 0;
    
    m_bIsInitialized = false;
    
    return kNoError;
}

Error_t CDtw::process(float **ppfDistanceMatrix)
{
    if(!m_bIsInitialized)
        return kNotInitializedError;
    
    if(ppfDistanceMatrix == NULL)
        return kFunctionInvalidArgsError;
    
    // first column
    for (int i = 1; i < m_iNumRows; i++){
        m_ppfCostMatrix[i][0] = ppfDistanceMatrix[i][0] + m_ppfCostMatrix[i-1][0];
        m_ppfDirectionsMatrix[i][0] = CDtw::kVert;
    }
    // first row
    for (int j = 1; j < m_iNumCols; j++){
        m_ppfCostMatrix[0][j] = ppfDistanceMatrix[0][j] + m_ppfCostMatrix[0][j-1];
        m_ppfDirectionsMatrix[0][j] = CDtw::kHoriz;
    }

    // rest of the cost matrix
    for (int i = 1; i < m_iNumRows; i++)
    {
        for (int j = 1; j < m_iNumCols; j++)
        {
            m_ppfCostMatrix[i][j] = ppfDistanceMatrix[i][j] + getMinCost(m_ppfCostMatrix[i][j-1],                                                             m_ppfCostMatrix[i-1][j], m_ppfCostMatrix[i-1][j-1],                                                           m_ppfDirectionsMatrix[i][j]);
        }
    }
    
    // backtracking
    int i = m_iNumRows - 1;
    int j = m_iNumCols - 1;
    m_fpathCost = ppfDistanceMatrix[i][j];
    m_ppiPathResult[0][0] = i;
    m_ppiPathResult[1][0] = j;
    m_iPathLength = 1;
    
    while (i+j > 0)
    {
        if (i == 0)
        {
            m_fpathCost += ppfDistanceMatrix[i][j-1];
            j = j - 1;
        }
        else if (j == 0)
        {
            m_fpathCost += ppfDistanceMatrix[i-1][j];
            i = i - 1;
        }
        else
        {
            if(m_ppfDirectionsMatrix[i][j] == CDtw::kDiag){
                m_fpathCost += ppfDistanceMatrix[i-1][j-1];
                i = i - 1;
                j = j -1 ;
            }
            else if(m_ppfDirectionsMatrix[i][j] == CDtw::kHoriz){
                m_fpathCost += ppfDistanceMatrix[i-1][j];
                i = i - 1;
            }
            else if(m_ppfDirectionsMatrix[i][j] == CDtw::kVert){
                m_fpathCost += ppfDistanceMatrix[i][j-1];
                j = j - 1;
            }
        }
        m_iPathLength++;
        m_ppiPathResult[0][m_iPathLength-1] = i;
        m_ppiPathResult[1][m_iPathLength-1] = j;
    }
    
    return kNoError;
}

int CDtw::getPathLength()
{
    return m_iPathLength;
}

float CDtw::getPathCost() const
{
    return m_fpathCost;
}

Error_t CDtw::getPath( int **ppiPathResult ) const
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < m_iPathLength; j++){
            ppiPathResult[i][m_iPathLength-j-1] = m_ppiPathResult[i][j];
        }
    }
    return kNoError;
}

float CDtw::getMinCost(float horiz, float vert, float diag, Directions_t& direction)
{
    float min = diag;
    direction = CDtw::kDiag;

    if (vert < diag){
        min = vert;
        direction = CDtw::kVert;
    }
    if (horiz < diag && horiz < vert){
        min = horiz;
        direction = CDtw::kHoriz;
    }

    return min;
}
