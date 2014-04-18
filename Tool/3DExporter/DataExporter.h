#ifndef _DADAEXPORTER_H_
#define  _DADAEXPORTER_H_
#include "IGame\IGame.h"
#include "IGame\IGameModifier.h"
#include "impexp.h"
#include "decomp.h"

struct stKeyValue 
{
    INT m_nKeyTime;
    std::string m_strKeyName;
};

struct SWeightData
{
    bool operator < (const SWeightData& ref)
    {
        return fWeight < ref.fWeight;
    }
    ESkeletonBoneType type;
    float fWeight;
};

class CDataExporter
{
public:
	CDataExporter();
	virtual ~CDataExporter();

    void    SetExportFileName(std::string fileName);

	void    ExportScene();

	void    ExportSkeleton();
	BOOL    CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode);
	BOOL    ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer);
    BOOL	ExportAnimationNode(IGameNode* pNode, CSerializer& serializer);

	void    ExportMesh();
    void    GetSkinInfo();
	void	ExportSkinnedMesh();

	void	ExportAnimation();
    void    InitAminiationSegment();

    void    InitNodeBones();
    void    InitFrameInfo();

private:

    void    GetMaterialInfo(IGameNode* pNode);
    bool    FilterVertexData(const Point3& ptPos, const Point3& ptUv);

    std::string   m_strFileName;
    std::string   m_strFilePath;

    IGameScene* m_pIGameScene;

    int     m_nBoneCount;
    int     m_nStartFrame;
    int     m_nEndFrame;
    size_t  m_uFPS;
    Tab<IGameNode*> m_listBones;

    std::vector<stKeyValue> m_vKeys;

    std::vector<Point3> m_vecPos;
    std::vector<Point3> m_vecUV;
    std::vector<SWeightData> m_vecWeightData;

    std::vector<std::string> m_vMaterialName;
    size_t  m_uMeshCount;
    std::vector<int>  m_vMeshVCount;
    std::vector<int>  m_vMeshMeterialCnt;
    std::vector<int>  m_posIndexVector;
    std::vector<int>  m_MeshPosIndexCnt;
};


#endif//_DADAEXPORTER_H_
