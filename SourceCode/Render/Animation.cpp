#include "stdafx.h"
#include "Animation.h"
#include "Utility/BeatsUtility/Serializer.h"

CAnimation::CAnimation()
    : m_uFrameCount(0)
    , m_uFPS(0)
    , m_fDuration(0.0F)
    , m_uCurFrame(0)
{

}

CAnimation::~CAnimation()
{

}

float CAnimation::Duration()
{
    return m_fDuration;
}


const std::vector<ESkeletonBoneType>& CAnimation::GetBones() const
{
    return m_bones;
}

const std::vector<kmMat4*>& CAnimation::GetBoneMatrixByFrame( size_t uFrame ) const
{
    BEATS_ASSERT(uFrame < m_uFrameCount);
    return m_posOfFrame[uFrame];
}

const std::vector<kmMat4*>& CAnimation::GetBoneMatrixByTime( float playtime ) const
{
    size_t currFrame = static_cast<size_t>(playtime * m_uFPS);
    currFrame %= m_uFrameCount;
    return GetBoneMatrixByFrame(currFrame);
}

size_t CAnimation::GetFPS() const
{
    return m_uFPS;
}

const kmMat4* CAnimation::GetOneBoneMatrixByTime(float fTime, ESkeletonBoneType boneType) const
{
    kmMat4* pRet = NULL;
    const std::vector<kmMat4*>& bones = GetBoneMatrixByTime(fTime);
    BEATS_ASSERT(m_bones.size() == bones.size());
    for (size_t i = 0; i < bones.size(); ++i)
    {
        if (m_bones[i] == boneType)
        {
            pRet = bones[i];
            break;
        }
    }
    return pRet;
}

bool CAnimation::Load()
{
    bool bRet = true;
    // Load From File

    TCHAR* filePath = (TCHAR*)m_strPath.c_str();
    CSerializer serializer(filePath);
    size_t uBoneCount = 0;
    serializer >> uBoneCount;
    serializer >> m_uFPS;
    int nFrameCount = 0;
    serializer >> nFrameCount;
    m_uFrameCount = (unsigned short)nFrameCount;//Type cast!
    m_fDuration = (float)m_uFrameCount / m_uFPS;
    for ( size_t j = 0 ; j < uBoneCount ; ++ j )
    {
        ESkeletonBoneType boneType = eSBT_Count;
        serializer >> boneType;
        BEATS_ASSERT(boneType < eSBT_Count, _T("Invalid bone type!"));
        m_bones.push_back(boneType);
    }

    for (size_t i = 0; i < m_uFrameCount; ++i)
    {        
        std::vector<kmMat4*> _vMatix;
        for ( size_t j = 0 ; j < uBoneCount; ++j )
        {
            kmMat4* _TPosMatrix = new kmMat4();
            serializer >> _TPosMatrix->mat[0] >> _TPosMatrix->mat[1] >> _TPosMatrix->mat[2] >>_TPosMatrix->mat[3]
            >>_TPosMatrix->mat[4] >> _TPosMatrix->mat[5] >> _TPosMatrix->mat[6] >> _TPosMatrix->mat[7] >> _TPosMatrix->mat[8] 
            >> _TPosMatrix->mat[9] >> _TPosMatrix->mat[10] >> _TPosMatrix->mat[11] >> _TPosMatrix->mat[12] >> _TPosMatrix->mat[13]
            >> _TPosMatrix->mat[14] >> _TPosMatrix->mat[15] ;
            _vMatix.push_back( _TPosMatrix );
        }
        m_posOfFrame.push_back( _vMatix );
    }
    return bRet;
}

bool CAnimation::Unload()
{
    bool bRet = true;

    size_t uCount = m_posOfFrame.size();
    for ( size_t i = 0 ; i < uCount ; ++i )
    {
        size_t uMatixCount = m_posOfFrame[i].size();
        for ( size_t j = 0 ; j < uMatixCount; ++j )
        {
            BEATS_SAFE_DELETE( m_posOfFrame[i][j] );
        }
    }
    return bRet;
}

unsigned short CAnimation::GetFrameCount()
{
    return m_uFrameCount;
}

void CAnimation::SetCurFrame(size_t curFrame)
{
   m_uCurFrame = curFrame;
}

size_t CAnimation::GetCurFrame() const
{
    return m_uCurFrame;
}
