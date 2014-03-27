#ifndef _DADAEXPORTER_H_
#define  _DADAEXPORTER_H_
#include "IGame\IGame.h"
#include "IGame\IGameModifier.h"
#include "impexp.h"

class CDataExporter
{
public:
	CDataExporter();
	virtual ~CDataExporter();

	void    ExportScene();

	void    ExportSkeleton();
	BOOL    CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode);
	BOOL    ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer);
    BOOL	ExportAnimationNode(IGameNode* pNode, CSerializer& serializer);

	void    ExportMesh();
	BOOL    ExportMeshNode(IGameNode* pNode);
	void	ExportSkinnedMesh(IGameNode* pNode);

	void	ExportAnimation();

    void    InitNodeBones();
    void    InitFrameInfo();

private:
    IGameScene* m_pIGameScene;

    int     m_nBoneCount;
    int     m_nStartFrame;
    int     m_nEndFrame;
    size_t  m_uFPS;
    Tab<IGameNode*> m_listBones;
};


#endif//_DADAEXPORTER_H_
