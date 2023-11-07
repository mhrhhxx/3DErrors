// SolidMineDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AsdkAcUiSample.h"
#include "SolidMineDlg.h"
#include "MMSYSTEM.H"


// CSolidMineDlg �Ի���

IMPLEMENT_DYNAMIC(CSolidMineDlg, CAcUiDialog)

CSolidMineDlg::CSolidMineDlg(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CSolidMineDlg::IDD, pParent)
{

}

CSolidMineDlg::~CSolidMineDlg()
{
}

void CSolidMineDlg::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAYER0, m_trilayers0);
	DDX_Control(pDX, IDC_LIST_LAYER1, m_trilayers1);
}


BEGIN_MESSAGE_MAP(CSolidMineDlg, CAcUiDialog)
	ON_BN_CLICKED(IDOK, &CSolidMineDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INT, &CSolidMineDlg::OnBnClickedButtonInt)
	ON_BN_CLICKED(IDC_BUTTON_UNION, &CSolidMineDlg::OnBnClickedButtonUnion)
END_MESSAGE_MAP()


// CSolidMineDlg ��Ϣ�������

BOOL CSolidMineDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	m_trilayers0.ResetContent();
	m_trilayers1.ResetContent();
		
	AcDbBlockTable *pBlockTable=NULL;
	AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
        AcDb::kForWrite);
	pBlockTable->close();
	pBlockTable=NULL;

	AcDbBlockTableRecordIterator *pBtrIter;        //����¼�����ָ��
	pBlockTableRecord->newIterator(pBtrIter);

	AcDbEntity *pEnt;
	CString layer;

	int isok,a;

	for(pBtrIter->start();!pBtrIter->done();pBtrIter->step())
	{   //�ÿ���¼�����������״�������¼
		pBtrIter->getEntity(pEnt, AcDb::kForRead);
		//���ʵ�������
		
		if(AcDb3dSolid::cast(pEnt))
		{
			isok=0;
			layer=pEnt->layer();
			for(a=0;a<layArrs0.GetSize();a++)
			{
				if(layer==layArrs0[a])
				{
					isok=1;
					break;
				}
			}
			if(isok==0)
			{
				layArrs0.Add(layer);
				m_trilayers0.AddString(layer);
				m_trilayers1.AddString(layer);
			}
		}
		pEnt->close();
		pEnt=NULL;
	}
	delete pBtrIter;
	pBtrIter=NULL;
	pBlockTableRecord->close();

	layArrs0.RemoveAll();
	//
	UpdateData(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//2017-07-05������
void CSolidMineDlg::OnBnClickedOk()
{
	OnOK();
	UpdateData(true);
	SolidOperate(0);
}

//2017-07-05������
void CSolidMineDlg::OnBnClickedButtonInt()
{
	OnOK();
	UpdateData(true);
	SolidOperate(1);
}

//2017-07-05������
void CSolidMineDlg::OnBnClickedButtonUnion()
{
	OnOK();
	UpdateData(true);
	SolidOperate(2);
}

//2017-7-5mode=0,����mode=1,����mode=2,��
//2023-05-09ͨ����¡����ʵ����뵽���ݿ��У�ʵ����AcDb3dSolid��ʵ���Ĳ���Ͳ�����
void CSolidMineDlg::SolidOperate(int mode)
{
	AcDbBlockTable *pBlockTable=NULL;
	AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
        AcDb::kForWrite);
	pBlockTable->close();
	pBlockTable=NULL;

	AcDbBlockTableRecordIterator *pBtrIter;        //����¼�����ָ��
	pBlockTableRecord->newIterator(pBtrIter);

	AcDb3dSolid *pSolid0,*pSolid1,*pSolid2,*pSolid3;
	double volume;
	double pt[3];
	int colorin;
	AcDbExtents extents0,extents1;
	AcGeVector3d prinAxes[3];
	AcGePoint3d centroid;
	
	AcDbEntity *pEnt;
	AcDbObjectId objId;
	new_solid *sdhead0,*sdhead1,*sd0,*sd1,*sd2,*sd3;        //ʵ������
	sdhead0=NULL;
	sdhead1=NULL;

	CString layer;
	int a,i;

	for(i=0;i<m_trilayers0.GetCount();i++)
	{
		int sel=m_trilayers0.GetSel(i);
		if(sel==1)
		{
			m_trilayers0.GetText(i,layer);
			layArrs0.Add(layer);
			//acutPrintf(_T("\n��Լ��ͼ��"));
		}
	}
	for(i=0;i<m_trilayers1.GetCount();i++)
	{
		int sel=m_trilayers1.GetSel(i);
		if(sel==1)
		{
			m_trilayers1.GetText(i,layer);
			layArrs1.Add(layer);
			//acutPrintf(_T("\n��Լ��ͼ��"));
		}
	}
	//��1����ö������������͵װ���������
	for(pBtrIter->start();!pBtrIter->done();pBtrIter->step())
	{   //�ÿ���¼�����������״�������¼
		pBtrIter->getEntity(pEnt, AcDb::kForRead);
		pBtrIter->getEntityId(objId);
		//���ʵ�������
		
		if(AcDb3dSolid::cast(pEnt))
		{
			layer=pEnt->layer();
			pSolid0=AcDb3dSolid::cast(pEnt);
			extents0=AcDbExtents();
			pSolid0->getMassProp(volume,centroid,pt,pt,pt,prinAxes,pt,extents0);
			//acutPrintf(_T("\nminPoint(%0.2f,%0.2f,%0.2f),maxPoint(%0.2f,%0.2f,%0.2f)"),extents0.minPoint().x,extents0.minPoint().y,extents0.minPoint().z,extents0.maxPoint().x,extents0.maxPoint().y,extents0.maxPoint().z);
			
			for(a=0;a<layArrs0.GetSize();a++)
			{
				if(layer==layArrs0[a])
				{
					sd0=new new_solid;
					sd0->objId=objId;
					sd0->vol=volume;
					sd0->centroid=centroid;
					sd0->extents=extents0;
					sd0->layer=layer;
					sd0->next=NULL;
					if(sdhead0==NULL) sdhead0=sd0;
					else sd1->next=sd0;
					sd1=sd0;
					break;
				}
			}
			for(a=0;a<layArrs1.GetSize();a++)
			{
				if(layer==layArrs1[a])
				{
					sd2=new new_solid;
					sd2->objId=objId;
					sd2->vol=volume;
					sd2->centroid=centroid;
					sd2->extents=extents0;
					sd2->layer=layer;
					sd2->next=NULL;
					if(sdhead1==NULL) sdhead1=sd2;
					else sd3->next=sd2;
					sd3=sd2;
					break;
				}
			}
			pSolid0->close();
			pSolid0=NULL;
		}
		pEnt->close();
		pEnt=NULL;
	}

	delete pBtrIter;
	pBtrIter=NULL;

	//��2������ʵ�������м�����
	Acad::ErrorStatus es;
	bool unit=false;
	DWORD dwStart = timeGetTime();
	if(sdhead0!=NULL&&sdhead1!=NULL)
	{
		sd0=sdhead0;
		while(sd0!=NULL)
		{
			//acutPrintf(_T("\nsd0minPoint(%0.2f,%0.2f,%0.2f),sd0maxPoint(%0.2f,%0.2f,%0.2f)"),sd0->extents.minPoint().x,sd0->extents.minPoint().y,sd0->extents.minPoint().z,sd0->extents.maxPoint().x,sd0->extents.maxPoint().y,sd0->extents.maxPoint().z);
			acdbOpenObject(pSolid1, sd0->objId, AcDb::kForWrite);      //������ʵ��
			colorin=pSolid1->colorIndex();
			sd2=sdhead1;
			
			if(mode==0||mode==1)    //���뽻����
			{
				while(sd2!=NULL)
				{
					//acutPrintf(_T("\nsd2minPoint(%0.2f,%0.2f,%0.2f),sd2maxPoint(%0.2f,%0.2f,%0.2f)"),sd2->extents.minPoint().x,sd2->extents.minPoint().y,sd2->extents.minPoint().z,sd2->extents.maxPoint().x,sd2->extents.maxPoint().y,sd2->extents.maxPoint().z);
					acdbOpenObject(pSolid0, sd2->objId, AcDb::kForWrite);
					if(boxinginter(sd0->extents.minPoint(),sd0->extents.maxPoint(),sd2->extents.minPoint(),sd2->extents.maxPoint()))
					{
						if(pSolid0!=NULL)
						{
							pSolid2 = AcDb3dSolid::cast(pSolid0->clone());    //����ʵ�壬�����������в�������Ķ���
							if(pSolid2!=NULL)
							{
								if(mode==0){
									es=pSolid1->booleanOper(AcDb::kBoolSubtract ,pSolid0); //������
									if(es==Acad::eOk)
									{
										pBlockTableRecord->appendAcDbEntity(sd2->objId,pSolid2);
										Setentlayer(pDb,pSolid2,sd2->layer,0);
									}
									pSolid1->setColorIndex(colorin);
								}
								else if(mode==1)
								{
									pSolid3 = AcDb3dSolid::cast(pSolid1->clone());    //��ԭʵ����п�¡
									es=pSolid3->booleanOper(AcDb::kBoolIntersect ,pSolid2);  //������
								
									if(es==Acad::eOk)
									{
										pSolid3->setColorIndex(colorin);
										pBlockTableRecord->appendAcDbEntity(pSolid3);
										Setentlayer(pDb,pSolid3,sd0->layer+_T("+��"),0);
									}
									pSolid3->close();
									pSolid3=NULL;
								}
								
								pSolid2->close();
								pSolid2=NULL;
							}
						}
					}
					
					pSolid0->close();
					pSolid0=NULL;
					sd2=sd2->next;
				}
			}
			else//������
			{
				if(!unit)     //��һ��ʵ��������ʵ����в�����
				{
					while(sd2!=NULL)
					{
						//acutPrintf(_T("\nsd2minPoint(%0.2f,%0.2f,%0.2f),sd2maxPoint(%0.2f,%0.2f,%0.2f)"),sd2->extents.minPoint().x,sd2->extents.minPoint().y,sd2->extents.minPoint().z,sd2->extents.maxPoint().x,sd2->extents.maxPoint().y,sd2->extents.maxPoint().z);
						acdbOpenObject(pSolid0, sd2->objId, AcDb::kForWrite);
						es=pSolid1->booleanOper(AcDb::kBoolUnite ,pSolid0); //������
						
						pSolid0->close();
						pSolid0=NULL;
						sd2=sd2->next;
					}
				
					unit=true;
					pSolid0 = AcDb3dSolid::cast(pSolid1->clone());
				}
				else         //�ӵڶ�����ʼ���ڲ�ʵ�����������
				{
					es=pSolid1->booleanOper(AcDb::kBoolUnite ,pSolid0); //������
					pSolid0 = AcDb3dSolid::cast(pSolid1->clone());
				}
			}
			pSolid1->close();
			pSolid1=NULL;
			sd0=sd0->next;
		}
	}
	pBlockTableRecord->close();
	
	layArrs0.RemoveAll();
	layArrs1.RemoveAll();
	DeleteMemory(sdhead1);
	DeleteMemory(sdhead0);
	
	dwStart=timeGetTime()-dwStart;
	acutPrintf(_T("\n�������е�ʱ��=%d"),dwStart);
}
