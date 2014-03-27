#include "stdafx.h"
#include "DataExporter.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "3DSceneExport.h"

#define  MAX_BONE_BLEND 4

inline static ESkeletonBoneType GetBoneType( const CHAR* pszNewName)
{
    ESkeletonBoneType ret = eSBT_Null;
    if (strcmp ( pszNewName, "Bip01 头部")==0)  
        ret = eSBT_Head;
    else if (strcmp ( pszNewName, "Bip01 Neck")==0)  
        ret = eSBT_Neck;
    else if (strcmp ( pszNewName, "Bip01 R Clavicle")==0)  
        ret = eSBT_RightClavicle;
    else if (strcmp ( pszNewName, "Bip01 L Clavicle")==0)  
        ret = eSBT_LeftClavicle;
    else if (strcmp ( pszNewName, "Bip01 R UpperArm")==0)  
        ret = eSBT_RightUpperArm;
    else if (strcmp ( pszNewName, "Bip01 L UpperArm")==0)  
        ret = eSBT_LeftUpperArm;
    else if (strcmp ( pszNewName, "Bip01 R Forearm")==0)  
        ret = eSBT_RightForearm;
    else if (strcmp ( pszNewName, "Bip01 L Forearm")==0)  
        ret = eSBT_LeftForearm;
    else if (strcmp ( pszNewName, "Bip01 R Hand")==0)  
        ret = eSBT_RightHand;
    else if (strcmp ( pszNewName, "Bip01 L Hand")==0)  
        ret = eSBT_LeftHand;
    else if (strcmp ( pszNewName, "Bip01 R Finger0")==0)  
        ret = eSBT_RightFinger1;
    else if (strcmp ( pszNewName, "Bip01 R Finger1")==0)  
        ret = eSBT_RightFinger2;	
    else if (strcmp ( pszNewName, "Bip01 R Finger2")==0)  
        ret = eSBT_RightFinger3;
    else if (strcmp ( pszNewName, "Bip01 R Finger3")==0)  
        ret = eSBT_RightFinger4;
    else if (strcmp ( pszNewName, "Bip01 L Finger0")==0)  
        ret = eSBT_LeftFinger1;
    else if (strcmp ( pszNewName, "Bip01 L Finger1")==0)  
        ret = eSBT_LeftFinger2;
    else if (strcmp ( pszNewName, "Bip01 L Finger2")==0)  
        ret = eSBT_LeftFinger3;
    else if (strcmp ( pszNewName, "Bip01 L Finger3")==0)  
        ret = eSBT_LeftFinger4;	

    else if (strcmp ( pszNewName, "Bip01 R Finger01")==0)  
        ret = eSBT_RightFinger11;
    else if (strcmp ( pszNewName, "Bip01 R Finger11")==0)  
        ret = eSBT_RightFinger21;	
    else if (strcmp ( pszNewName, "Bip01 R Finger21")==0)  
        ret = eSBT_RightFinger31;
    else if (strcmp ( pszNewName, "Bip01 R Finger31")==0)  
        ret = eSBT_RightFinger41;
    else if (strcmp ( pszNewName, "Bip01 L Finger01")==0)  
        ret = eSBT_LeftFinger11;
    else if (strcmp ( pszNewName, "Bip01 L Finger11")==0)  
        ret = eSBT_LeftFinger21;
    else if (strcmp ( pszNewName, "Bip01 L Finger21")==0)  
        ret = eSBT_LeftFinger31;
    else if (strcmp ( pszNewName, "Bip01 L Finger31")==0)  
        ret = eSBT_LeftFinger41;	

    else if (strcmp ( pszNewName, "Bip01 R Finger02")==0)  
        ret = eSBT_RightFinger12;
    else if (strcmp ( pszNewName, "Bip01 R Finger12")==0)  
        ret = eSBT_RightFinger22;	
    else if (strcmp ( pszNewName, "Bip01 R Finger22")==0)  
        ret = eSBT_RightFinger32;
    else if (strcmp ( pszNewName, "Bip01 R Finger32")==0)  
        ret = eSBT_RightFinger42;
    else if (strcmp ( pszNewName, "Bip01 L Finger02")==0)  
        ret = eSBT_LeftFinger12;
    else if (strcmp ( pszNewName, "Bip01 L Finger12")==0)  
        ret = eSBT_LeftFinger22;
    else if (strcmp ( pszNewName, "Bip01 L Finger22")==0)  
        ret = eSBT_LeftFinger32;
    else if (strcmp ( pszNewName, "Bip01 L Finger32")==0)  
        ret = eSBT_LeftFinger42;	

    else if (strcmp ( pszNewName, "Bip01 R Finger0Nub")==0)  
        ret = eSBT_RightFinger13;
    else if (strcmp ( pszNewName, "Bip01 R Finger1Nub")==0)  
        ret = eSBT_RightFinger23;	
    else if (strcmp ( pszNewName, "Bip01 R Finger2Nub")==0)  
        ret = eSBT_RightFinger33;
    else if (strcmp ( pszNewName, "Bip01 R Finger3Nub")==0)  
        ret = eSBT_RightFinger43;
    else if (strcmp ( pszNewName, "Bip01 L Finger0Nub")==0)  
        ret = eSBT_LeftFinger13;
    else if (strcmp ( pszNewName, "Bip01 L Finger1Nub")==0)  
        ret = eSBT_LeftFinger23;
    else if (strcmp ( pszNewName, "Bip01 L Finger2Nub")==0)  
        ret = eSBT_LeftFinger33;
    else if (strcmp ( pszNewName, "Bip01 L Finger3Nub")==0)  
        ret = eSBT_LeftFinger43;	

    else if (strcmp ( pszNewName, "Bip01 Spine2")==0)  
        ret = eSBT_Spine2;
    else if (strcmp ( pszNewName, "Bip01 Spine1")==0)  
        ret = eSBT_Spine1;
    else if (strcmp ( pszNewName, "Bip01 Spine")==0)  
        ret = eSBT_Spine;
    else if (strcmp ( pszNewName, "Bip01 骨盆")==0)  
        ret = eSBT_Pelvis;
    else if (strcmp ( pszNewName, "Bip01")==0)  
        ret = eSBT_Root;
    else if (strcmp ( pszNewName, "Bip01 R Thigh")==0)  
        ret = eSBT_RightThigh;
    else if (strcmp ( pszNewName, "Bip01 L Thigh")==0)  
        ret = eSBT_LeftThigh;
    else if (strcmp ( pszNewName, "Bip01 R Calf")==0)  
        ret = eSBT_RightCalf;
    else if (strcmp ( pszNewName, "Bip01 L Calf")==0)  	
        ret = eSBT_LeftCalf;
    else if (strcmp ( pszNewName, "Bip01 R Foot")==0)  
        ret = eSBT_RightFoot;
    else if (strcmp ( pszNewName, "Bip01 L Foot")==0)  
        ret = eSBT_LeftFoot;
    else if (strcmp ( pszNewName, "Bip01 R Toe0")==0)  
        ret = eSBT_RightToe1;
    else if (strcmp ( pszNewName, "Bip01 L Toe0")==0)  
        ret = eSBT_LeftToe1;
    else if (strcmp ( pszNewName, "Bip01 R Toe01")==0)  
        ret = eSBT_RightToe2;
    else if (strcmp ( pszNewName, "Bip01 L Toe01")==0)  
        ret = eSBT_LeftToe2;
    else if (strcmp ( pszNewName, "Bip01 R Toe02")==0)  
        ret = eSBT_RightToe3;
    else if (strcmp ( pszNewName, "Bip01 L Toe02")==0)  
        ret = eSBT_LeftToe3;

    else if (strcmp ( pszNewName, "Bip01 R ForeTwist")==0)  
        ret = eSBT_RightForeTwist;
    else if (strcmp ( pszNewName, "Bip01 RUpArmTwist")==0)  
        ret = eSBT_RightUpArmTwist;
    else if (strcmp ( pszNewName, "Bip01 L ForeTwist")==0)  
        ret = eSBT_LeftForeTwist;
    else if (strcmp ( pszNewName, "Bip01 LUpArmTwist")==0)  
        ret = eSBT_LeftUpArmTwist;
    else if (strcmp ( pszNewName, "Bone04")==0)  
        ret = eSBT_Tooth;
    else if (strcmp ( pszNewName, "Bip01 L Toe0Nub")==0)  
        ret = eSBT_LeftToe5;
    else if (strcmp ( pszNewName, "Bip01 R Toe0Nub")==0)  
        ret = eSBT_RightToe5;
    else if (strcmp ( pszNewName, "Bip01 头部Nub")==0)  
        ret = eSBT_Count;

    BEATS_ASSERT(ret != eSBT_Null, _T("GetBoneType"));
    return ret;
}

CDataExporter::CDataExporter()
    : m_pIGameScene (GetIGameInterface())
    , m_nBoneCount(0)
    , m_nStartFrame(0)
    , m_nEndFrame (0)
    , m_uFPS(0)
{
    
}

CDataExporter::~CDataExporter()
{
    m_pIGameScene->ReleaseIGame();
}

void    CDataExporter::InitNodeBones()
{
    UINT uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    for(UINT x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        this->CollectBoneNode(m_listBones, pNode);
    }
}

void    CDataExporter::InitFrameInfo()
{
    TimeValue start_time = m_pIGameScene->GetSceneStartTime();
    TimeValue end_time = m_pIGameScene->GetSceneEndTime();
    INT nTickPerFrame = m_pIGameScene->GetSceneTicks();
    m_nStartFrame = start_time / nTickPerFrame;
    m_nEndFrame = end_time / nTickPerFrame;
    m_uFPS = TIME_TICKSPERSEC / nTickPerFrame;
}

void    CDataExporter::ExportScene()
{
    InitNodeBones();
    InitFrameInfo();

    ExportSkeleton();
    ExportMesh();
    ExportAnimation();
}

void CDataExporter::ExportSkeleton()
{
    m_pIGameScene->InitialiseIGame();
    m_pIGameScene->SetStaticFrame(0);

    UINT nBoneCount = (UINT)m_listBones.Count();

    CSerializer serializer;
    serializer << nBoneCount;

    for(UINT x = 0; x < nBoneCount; x++)
    {
        IGameNode* pNode = m_listBones[x];
        assert(pNode != NULL);
        this->ExportSkeletonNode(pNode, serializer);
    }
    serializer.Deserialize(_T("C:/org.ske"));
    m_pIGameScene->ReleaseIGame();
}

void    CDataExporter::ExportMesh()
{
    m_pIGameScene->InitialiseIGame();

    UINT uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    for(UINT x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        ExportMeshNode(pNode);
    }

    m_pIGameScene->ReleaseIGame();
}

BOOL	CDataExporter::CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode)
{
    IGameObject* pObject = pNode->GetIGameObject();

    IGameObject::ObjectTypes gameType = pObject->GetIGameType();
    IGameObject::MaxType maxType = pObject->GetMaxType();

    BOOL bBone = gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER;

    pNode->ReleaseIGameObject();

    if (bBone)
    {
        res.Append(1, &pNode);
    }

    INT nChildCnt = pNode->GetChildCount();
    for(INT x = 0; x < nChildCnt; x++)
    {
        IGameNode* pChild = pNode->GetNodeChild(x);
        BEATS_ASSERT(pChild != NULL);
        this->CollectBoneNode(res, pChild);
    }

    return TRUE;
}

BOOL	CDataExporter::ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer)
{
#ifdef _DEBUG
	IGameObject* pObject = pNode->GetIGameObject();
    IGameObject::ObjectTypes gameType = pObject->GetIGameType();
    IGameObject::MaxType maxType = pObject->GetMaxType();

    BEATS_ASSERT(gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER, _T("Error at exporting a bone!"));

#endif

    //BoneType
    ESkeletonBoneType type = GetBoneType(pNode->GetName());

    ESkeletonBoneType parentType = eSBT_Null;
    IGameNode* pParentNode = pNode->GetNodeParent();
    if ( pParentNode )
    {
        parentType = GetBoneType(pParentNode->GetName());
        BEATS_ASSERT(parentType != eSBT_Null, _T("Unknown parent bone type with name %s"), pParentNode->GetName());
    }

    serializer << type << parentType;

    GMatrix mtxWorld = pNode->GetWorldTM();

    for(size_t i = 0; i < 4; i++)
    {
        Point4 pt = mtxWorld.GetRow(i);
        serializer << pt.x << pt.y << pt.z << pt.w;
    }

    return TRUE;
}

BOOL	CDataExporter::ExportAnimationNode(IGameNode* pNode, CSerializer& serializer)
{
#ifdef _DEBUG
	IGameObject* pObject = pNode->GetIGameObject();
	IGameObject::ObjectTypes gameType = pObject->GetIGameType();
	IGameObject::MaxType maxType = pObject->GetMaxType();

	BOOL bBone = gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER;
	BEATS_ASSERT(bBone, _T("Error at exporting a bone! ExportAnimationNode"));
#endif // _DEBUG

    GMatrix mtxWorld = pNode->GetWorldTM();
    for(size_t i = 0; i < 4; i++)
    {
        Point4 pt = mtxWorld.GetRow(i);
        serializer << pt.x << pt.y << pt.z << pt.w;
    }

    return TRUE;
}

BOOL CDataExporter::ExportMeshNode(IGameNode* pNode)
{
    IGameObject* pObject = pNode->GetIGameObject();
    {
        IGameObject::ObjectTypes gameType = pObject->GetIGameType();

        if(gameType == IGameObject::IGAME_MESH)
        {
            this->ExportSkinnedMesh(pNode);
        }
    }
    pNode->ReleaseIGameObject();

    return TRUE;
}

void	CDataExporter::ExportSkinnedMesh(IGameNode* pNode)
{
    IGameMesh* pMesh = (IGameMesh*)pNode->GetIGameObject();

    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();

    CSerializer serializer;

    std::vector<Point3> vecPos;
    std::vector<Point3> vecUV;
    std::vector<int>    vecPosIndex;
    
    Tab<int> mapNums = pMesh->GetActiveMapChannelNum();
    BEATS_ASSERT( mapNums.Count() == 1);
    int uFaceCount = pMesh->GetNumberOfFaces();
    for(int i = 0; i < uFaceCount; ++i)
    {
        FaceEx* pFace = pMesh->GetFace(i);
        for(int  j = 0; j < 3; ++j)
        {
            DWORD mapIndex[3];
            Point3 ptUV;
            int indexUV = pFace->texCoord[j];
            if(pMesh->GetMapFaceIndex(mapNums[0], pFace->meshFaceIndex, mapIndex))
                ptUV = pMesh->GetMapVertex(mapNums[0], mapIndex[j]);
            else
                ptUV = pMesh->GetMapVertex(mapNums[0], indexUV);
            vecUV.push_back(ptUV);

            int indexPos = pFace->vert[j];
            Point3 pos = pMesh->GetVertex(indexPos);
            vecPos.push_back(pos);

            vecPosIndex.push_back(indexPos);
        }
    }
    int uVertexCount  = vecPos.size();
    serializer << uVertexCount;
    IGameSkin* pSkin = pMesh->GetIGameSkin();
    for(int i = 0; i < uVertexCount; ++i)
    {
        serializer << vecPos[i].x << vecPos[i].y << vecPos[i].z;
        serializer << vecUV[i].x << vecUV[i].y ;
        int uEffectedBones = pSkin->GetNumberOfBones(vecPosIndex[i]);
        float weightSum = 0;
        struct SWeightData
        {
            bool operator < (const SWeightData& ref)
            {
                return fWeight < ref.fWeight;
            }
            ESkeletonBoneType type;
            float fWeight;
        };
        std::map<ESkeletonBoneType, SWeightData> sortCache;
        for (int j = 0; j < uEffectedBones; ++j)
        {
            INode* pBone = pSkin->GetBone(vecPosIndex[i], j);
            const char* pszBoneName = pBone->GetName();
            ESkeletonBoneType boneType = GetBoneType(pszBoneName);
            BEATS_ASSERT(boneType != eSBT_Null, _T("Unknown parent bone type with name %s"), pBone->GetName());
            float fWeight = pSkin->GetWeight(vecPosIndex[i], j);

            if (!BEATS_FLOAT_EQUAL(fWeight, 0))
            {
                SWeightData data;
                data.type = boneType;
                data.fWeight = fWeight;
                sortCache[boneType] = data;
                weightSum += fWeight;
            }
        }
        int counter = 0;
        for (std::map<ESkeletonBoneType, SWeightData>::iterator iter = sortCache.begin(); iter != sortCache.end(); ++iter)
        {
            serializer << iter->first << iter->second.fWeight;
            if (++counter == MAX_BONE_BLEND)
            {
                break;
            }
        }
        for (int k = counter; k < MAX_BONE_BLEND; ++k)
        {
            serializer << -1 << 0;
        }

        BEATS_ASSERT(weightSum > 0.98f && weightSum < 1.01f);
    }
   
    serializer.Deserialize(_T("C:/org.skin"));

    pNode->ReleaseIGameObject();
}

void	CDataExporter::ExportAnimation()
{
    m_pIGameScene->InitialiseIGame();

    size_t nBoneCount = (size_t)m_listBones.Count();
    if(nBoneCount > 0)
    {
        int nFrameCount = m_nEndFrame - m_nStartFrame;

        CSerializer serializer;
        serializer << nBoneCount << m_uFPS << nFrameCount;
        for(size_t x = 0; x < nBoneCount; x++)
        {
            IGameNode* pNode = m_listBones[x];
            ESkeletonBoneType type = GetBoneType(pNode->GetName());
            serializer << type;
        }
        for(int frame = m_nStartFrame; frame <= m_nEndFrame; frame++)
        {
            m_pIGameScene->SetStaticFrame(frame);
            for(size_t x = 0; x < nBoneCount; x++)
            {
                IGameNode* pNode = m_listBones[x];
                BEATS_ASSERT(pNode != NULL);
                this->ExportAnimationNode(pNode, serializer);
            }
        }
        serializer.Deserialize(_T("C:/org.ani"));
    }

    m_pIGameScene->ReleaseIGame();
}

