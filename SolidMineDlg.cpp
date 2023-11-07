// SolidMineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AsdkAcUiSample.h"
#include "SolidMineDlg.h"
#include "MMSYSTEM.H"


// CSolidMineDlg 对话框

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


// CSolidMineDlg 消息处理程序

BOOL CSolidMineDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	m_trilayers0.ResetContent();
	m_trilayers1.ResetContent();
		
	AcDbBlockTable *pBlockTable=NULL;
	AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //得到工作块表记录
        AcDb::kForWrite);
	pBlockTable->close();
	pBlockTable=NULL;

	AcDbBlockTableRecordIterator *pBtrIter;        //块表记录浏览器指针
	pBlockTableRecord->newIterator(pBtrIter);

	AcDbEntity *pEnt;
	CString layer;

	int isok,a;

	for(pBtrIter->start();!pBtrIter->done();pBtrIter->step())
	{   //用块表记录浏览器遍历线状对象块表记录
		pBtrIter->getEntity(pEnt, AcDb::kForRead);
		//输出实体的数据
		
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
	// 异常: OCX 属性页应返回 FALSE
}

//2017-07-05差运算
void CSolidMineDlg::OnBnClickedOk()
{
	OnOK();
	UpdateData(true);
	SolidOperate(0);
}

//2017-07-05交运算
void CSolidMineDlg::OnBnClickedButtonInt()
{
	OnOK();
	UpdateData(true);
	SolidOperate(1);
}

//2017-07-05并运算
void CSolidMineDlg::OnBnClickedButtonUnion()
{
	OnOK();
	UpdateData(true);
	SolidOperate(2);
}

//2017-7-5mode=0,减；mode=1,交；mode=2,并
//2023-05-09通过克隆布尔实体加入到数据库中，实现了AcDb3dSolid多实体间的差、交和并运算
void CSolidMineDlg::SolidOperate(int mode)
{
	AcDbBlockTable *pBlockTable=NULL;
	AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //得到工作块表记录
        AcDb::kForWrite);
	pBlockTable->close();
	pBlockTable=NULL;

	AcDbBlockTableRecordIterator *pBtrIter;        //块表记录浏览器指针
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
	new_solid *sdhead0,*sdhead1,*sd0,*sd1,*sd2,*sd3;        //实体链表
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
			//acutPrintf(_T("\n是约束图层"));
		}
	}
	for(i=0;i<m_trilayers1.GetCount();i++)
	{
		int sel=m_trilayers1.GetSel(i);
		if(sel==1)
		{
			m_trilayers1.GetText(i,layer);
			layArrs1.Add(layer);
			//acutPrintf(_T("\n是约束图层"));
		}
	}
	//（1）获得顶板三角面网和底板三角面网
	for(pBtrIter->start();!pBtrIter->done();pBtrIter->step())
	{   //用块表记录浏览器遍历线状对象块表记录
		pBtrIter->getEntity(pEnt, AcDb::kForRead);
		pBtrIter->getEntityId(objId);
		//输出实体的数据
		
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

	//（2）对两实体网进行减操作
	Acad::ErrorStatus es;
	bool unit=false;
	DWORD dwStart = timeGetTime();
	if(sdhead0!=NULL&&sdhead1!=NULL)
	{
		sd0=sdhead0;
		while(sd0!=NULL)
		{
			//acutPrintf(_T("\nsd0minPoint(%0.2f,%0.2f,%0.2f),sd0maxPoint(%0.2f,%0.2f,%0.2f)"),sd0->extents.minPoint().x,sd0->extents.minPoint().y,sd0->extents.minPoint().z,sd0->extents.maxPoint().x,sd0->extents.maxPoint().y,sd0->extents.maxPoint().z);
			acdbOpenObject(pSolid1, sd0->objId, AcDb::kForWrite);      //被运算实体
			colorin=pSolid1->colorIndex();
			sd2=sdhead1;
			
			if(mode==0||mode==1)    //差与交运算
			{
				while(sd2!=NULL)
				{
					//acutPrintf(_T("\nsd2minPoint(%0.2f,%0.2f,%0.2f),sd2maxPoint(%0.2f,%0.2f,%0.2f)"),sd2->extents.minPoint().x,sd2->extents.minPoint().y,sd2->extents.minPoint().z,sd2->extents.maxPoint().x,sd2->extents.maxPoint().y,sd2->extents.maxPoint().z);
					acdbOpenObject(pSolid0, sd2->objId, AcDb::kForWrite);
					if(boxinginter(sd0->extents.minPoint(),sd0->extents.maxPoint(),sd2->extents.minPoint(),sd2->extents.maxPoint()))
					{
						if(pSolid0!=NULL)
						{
							pSolid2 = AcDb3dSolid::cast(pSolid0->clone());    //运算实体，保留用来进行布尔运算的对象
							if(pSolid2!=NULL)
							{
								if(mode==0){
									es=pSolid1->booleanOper(AcDb::kBoolSubtract ,pSolid0); //差运算
									if(es==Acad::eOk)
									{
										pBlockTableRecord->appendAcDbEntity(sd2->objId,pSolid2);
										Setentlayer(pDb,pSolid2,sd2->layer,0);
									}
									pSolid1->setColorIndex(colorin);
								}
								else if(mode==1)
								{
									pSolid3 = AcDb3dSolid::cast(pSolid1->clone());    //对原实体进行克隆
									es=pSolid3->booleanOper(AcDb::kBoolIntersect ,pSolid2);  //布尔交
								
									if(es==Acad::eOk)
									{
										pSolid3->setColorIndex(colorin);
										pBlockTableRecord->appendAcDbEntity(pSolid3);
										Setentlayer(pDb,pSolid3,sd0->layer+_T("+交"),0);
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
			else//布尔并
			{
				if(!unit)     //第一个实体与所有实体进行并运算
				{
					while(sd2!=NULL)
					{
						//acutPrintf(_T("\nsd2minPoint(%0.2f,%0.2f,%0.2f),sd2maxPoint(%0.2f,%0.2f,%0.2f)"),sd2->extents.minPoint().x,sd2->extents.minPoint().y,sd2->extents.minPoint().z,sd2->extents.maxPoint().x,sd2->extents.maxPoint().y,sd2->extents.maxPoint().z);
						acdbOpenObject(pSolid0, sd2->objId, AcDb::kForWrite);
						es=pSolid1->booleanOper(AcDb::kBoolUnite ,pSolid0); //布尔并
						
						pSolid0->close();
						pSolid0=NULL;
						sd2=sd2->next;
					}
				
					unit=true;
					pSolid0 = AcDb3dSolid::cast(pSolid1->clone());
				}
				else         //从第二个开始，内部实体间做并运算
				{
					es=pSolid1->booleanOper(AcDb::kBoolUnite ,pSolid0); //布尔并
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
	acutPrintf(_T("\n处理剖切的时间=%d"),dwStart);
}
