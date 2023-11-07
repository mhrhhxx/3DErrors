//
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
//
// AsdkAcUiSample.cpp : Defines the initialization routines for the DLL.
//
#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "AsdkAcUiSample.h" 

#include "AsdkAcUiDialogSample.h"
#include "AcExtensionModule.h"
#include "dbSubD.h"
#include "SolidMineDlg.h"
#include "MMSYSTEM.H"
#include "Asdkbody.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EPSILON 0.000001

/* some macros */
#define CROSS(dest,v1,v2)                      \
              dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
              dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
              dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; dest[1]=v1[1]-v2[1]; dest[2]=v1[2]-v2[2]; 

#define ADD(dest,v1,v2) dest[0]=v1[0]+v2[0]; dest[1]=v1[1]+v2[1]; dest[2]=v1[2]+v2[2]; 

#define MULT(dest,v,factor) dest[0]=factor*v[0]; dest[1]=factor*v[1]; dest[2]=factor*v[2];

/////////////////////////////////////////////////////////////////////////////
// Define the sole extension module object.

AC_IMPLEMENT_EXTENSION_MODULE(theArxDLL);

void dialogCreate()
{
 
    // Modal
    AsdkAcUiDialogSample dlg(CWnd::FromHandle(adsw_acadMainWnd()));
    INT_PTR nReturnValue = dlg.DoModal();
    
}

static void initApp()
{

  CAcModuleResourceOverride resOverride;
  
   	//2018-06-23�Բ�ȷ��ģ������AcDb3dSolidʵ��ģ�ͣ������ļ��ĸ�ʽ��x y diz dinz errors
	//�Զ���ȡ�������������
	acedRegCmds->addCommand(_T("ASDK_3DERRORS_SAMPLE"),
						_T("so_showerr"),
						_T("so_showerr"),
						ACRX_CMD_TRANSPARENT,
						so_showerrors);
	//2023-05-09�����ɳ�������ʵ��,������tritoex����
	acedRegCmds->addCommand(_T("ASDK_3DERRORS_SAMPLE"),
							_T("so_pit"), 
							_T("so_pit"), 
							ACRX_CMD_TRANSPARENT,
							so_openpit);
	//2017-06-24ʵ��AcDb3dSolid�����˲������㣨���ģ�ͶԵ���ģ�͵Ĳ��У�
	acedRegCmds->addCommand(_T("ASDK_3DERRORS_SAMPLE"),
							_T("so_bool"), 
							_T("so_bool"), 
							ACRX_CMD_TRANSPARENT,
							so_bool);

	//2017-11-27�õ�AsdkBody��AcDb3dSolidʵ������.
	acedRegCmds->addCommand(_T("ASDK_XLU_SAMPLE"),
							_T("asdkvol"),
							_T("asdkvol"),
							ACRX_CMD_TRANSPARENT,
							getasdkvol);
}

static void unloadApp()
{

  // Do other cleanup tasks here  
  acedRegCmds->removeGroup(_T("ASDK_3DERRORS_SAMPLE"));
}



//////////////////////////////////////////////////////////////
//
// Entry points
//
//////////////////////////////////////////////////////////////

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // Remove this if you use lpReserved
    UNREFERENCED_PARAMETER(lpReserved);
    
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        theArxDLL.AttachInstance(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        theArxDLL.DetachInstance();  
    }
    return 1;   // ok
}


extern "C" AcRx::AppRetCode acrxEntryPoint( AcRx::AppMsgCode msg, void* appId)
{
    switch( msg ) 
    {
    case AcRx::kInitAppMsg: 
        acrxDynamicLinker->unlockApplication(appId);
        acrxDynamicLinker->registerAppMDIAware(appId);
        initApp(); 
        break;
    case AcRx::kUnloadAppMsg: 
        unloadApp(); 
        break;
    case AcRx::kInitDialogMsg:
        
        break;
    default:
        break;
    }
    return AcRx::kRetOK;
}

//2018-06-23�Բ�ȷ��ģ������AcDb3dSolidʵ��ģ�ͣ������ļ��ĸ�ʽ��x y diz dinz errors
//�Զ���ȡ�������������
//2023-04-26����������Ϊ�������չ����
void so_showerrors()
{
	int num;
	int a,a1;
	int nten;
	int np;
	int row,vol;
	double fx,fy,difz,dinfz,ferros;
	FILE *file=NULL;

	AcDbPoint *pPoint;
	//AcDbFace *pFace;
	AcGePoint3d point3;
	AcGePoint3dArray pt_dins,pt_dis,point3s,pt_erros;
	AcGePoint3dArray pt_ups;
	AcGePoint3dArray pt_downs;
	
	double len=4;
	double heigth=4;
	double maxe,mine,errors;
	double dinzmax,dinzmin,dizmax,dizmin;
	double x0;

	AcCmColor color;
	color.setRGB(255,0,0);
	maxe=mine=0;
	row=vol=0;
	x0=0;
	CFileDialog filedialog(TRUE, _T("txt"), NULL, OFN_ALLOWMULTISELECT|OFN_ENABLESIZING |OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  _T("�����ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||"),NULL);
	if(filedialog.DoModal()==IDOK)
	{
		//filename=filedialog.GetPathName();

		//AcDbBlockTable *pBlockTable;
		//AcDbBlockTableRecord *pBlockTableRecord;
		//AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();        //�õ��������
		//	pDb->getSymbolTable(pBlockTable, AcDb::kForRead);
		//pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
		//	AcDb::kForWrite);
		//pBlockTable->close();

		POSITION pos = filedialog.GetStartPosition();
		while(pos)
		{
			CString filename=filedialog.GetNextPathName(pos);
			USES_CONVERSION;

			char *cfile;
			cfile=T2A(filename.GetBuffer(0));
			file=fopen(cfile,"r");

			num=0;
			a=0;
			np=0;

			while(!feof(file))
			{
				fscanf(file,"%lf%lf%lf%lf%lf\n",&fx,&fy,&difz,&dinfz,&ferros);
				
				point3.x=fx;point3.y=fy;point3.z=dinfz;
				if(difz>dinfz||fabs(dinfz-difz)<0.1)
				{
					acutPrintf(_T("\nnum=%i,difz=%f,dinfz=%f"),num,difz,dinfz);
					point3.z=difz+1.0;
				}
				pt_dins.append(point3);
				////acutPrintf(_T("\npoint3(%f,%f,%f)"),point3.x,point3.y,point3.z);

				//pPoint=new AcDbPoint(point3);
				//pBlockTableRecord->appendAcDbEntity(objId,pPoint);
				//pPoint->close();
				////objIds.append(objId);*/

				point3.z=difz;
				pt_dis.append(point3);
				///*pPoint=new AcDbPoint(point3);
				//pBlockTableRecord->appendAcDbEntity(objId,pPoint);
				//pPoint->close();
				//objIds.append(objId);*/
				
				point3.z=ferros;
				pt_erros.append(point3);
				
				if(num==0)
				{
					maxe=mine=ferros;
					dinzmax=dinzmin=dinfz;
					dizmax=dizmin=difz;
				}
				else
				{
					if(maxe<ferros) maxe=ferros;
					if(mine>ferros) mine=ferros;

					if(dinzmax<dinfz) dinzmax=dinfz;
					if(dinzmin>dinfz) dinzmin=dinfz;

					if(dizmax<difz) dizmax=difz;
					if(dizmin>difz) dizmin=difz;
				}
				
				num=num+1;
				
				if(x0!=fx) 
				{
					vol=vol+1;
				}
				if(vol==1) row=row+1;
				x0=fx;
				
			}
			acutPrintf(_T("\n�ɹ�չ��%i����"),num);
			fclose(file);
			
		}
		//pBlockTableRecord->close();

		acutPrintf(_T("\nrow=%i,vol=%i,dinzmaxmin(%f,%f),dizmaxmin(%f,%f)"),row,vol,dinzmax,dinzmin,dizmax,dizmin);
		
		if(pt_dins.length()==pt_dis.length())
		{
			int n;
			CString modlay,link;
			
			TCHAR *mName=new TCHAR[_MAX_FNAME+_MAX_EXT];
			if(acedGetString(0,_T("\n�����½�ģ�͵�ͼ����:"),mName,256)==RTNORM)
			{
				modlay=mName;
			}
			n=0;
			int oktet=0;

			AcDbBlockTable *pBlockTable;
			AcDbBlockTableRecord *pBlockTableRecord;
			AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();        //�õ��������
				pDb->getSymbolTable(pBlockTable, AcDb::kForRead); 	
			pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
				AcDb::kForWrite);
			pBlockTable->close();
		/*	for(num=0;num<pt_dins.length();num++)
			{
				pPoint=new AcDbPoint(pt_dis[num]);
				pBlockTableRecord->appendAcDbEntity(pPoint);
				pPoint->close();

				link.Format(_T("%d"),num);
				AcDbText *pText=new AcDbText();
				pText->setTextString(link);
				pText->setPosition(pt_dis[num]); 
				pText->setHeight(8);
				pText->setHorizontalMode(AcDb::kTextCenter);
				pText->setVerticalMode(AcDb::kTextVertMid);
				pText->setAlignmentPoint(pt_dis[num]);

				pBlockTableRecord->appendAcDbEntity(pText);
				pText->close();
				
			}*/

			AcDb3dSolid *pSolid0,*pSolid1,*pSolid3;
			for(num=0;num+row+1<pt_dins.length();num++)
			{
				if(n<row-1)      //�ɵ����ϣ���ʱ��
				{
				
					////pt_dis[num].z=pt_dis[num+vol].z=pt_dis[num+1+vol].z=pt_dis[num+1].z=-100.00;
					////pt_dins[num].z=pt_dins[num+vol].z=pt_dins[num+1+vol].z=pt_dins[num+1].z=1000.00;
					errors=(pt_erros[num].z+pt_erros[num+row].z+pt_erros[num+1+row].z+pt_erros[num+1].z)/4.0;

					for(a=0;a<2;a++)
					{
						if(a==0)
						{
							pt_downs.append(pt_dis[num]);
							pt_downs.append(pt_dis[num+row]);
							pt_downs.append(pt_dis[num+1+row]);
							pt_downs.append(pt_dis[num]);

							pt_ups.append(pt_dins[num]);
							pt_ups.append(pt_dins[num+row]);
							pt_ups.append(pt_dins[num+1+row]);
							pt_ups.append(pt_dins[num]);
						}
						else
						{
							pt_downs.append(pt_dis[num+1+row]);
							pt_downs.append(pt_dis[num+1]);
							pt_downs.append(pt_dis[num]);
							pt_downs.append(pt_dis[num+1+row]);

							pt_ups.append(pt_dins[num+1+row]);
							pt_ups.append(pt_dins[num+1]);
							pt_ups.append(pt_dins[num]);
							pt_ups.append(pt_dins[num+1+row]);
						}
						//����������
						pSolid0=cextrudeSolid(pt_downs,dinzmax+50);
						//acutPrintf(_T("\nnum=%i,pt_downs=%i"),num,pt_downs.length());

						if(pSolid0!=NULL)
						{
							//����������
							pSolid1=cextrudeSolid(pt_ups,dizmin-50);
							if(pSolid1!=NULL)
							{
								pSolid1->booleanOper(AcDb::kBoolIntersect,pSolid0);
								//pSolid1->getMassProp(volume,pt_inter,pt,pt,pt,prinAxes,pt,extents);
								//acutPrintf(_T("\n���=%f"),volume);
								if(pSolid1!=NULL)
								{
									color=getcolor(maxe,mine,errors);
									pBlockTableRecord->appendAcDbEntity(pSolid1);
									setFloatXdata(pDb,pSolid1,_T("errors"),errors);

									Setentlayer(pDb,pSolid1,modlay,7);
									pSolid1->setColor(color);
								}
								pSolid1->close();
							}
							pSolid0->close();
						}
						pt_downs.removeAll();
						pt_ups.removeAll();

						
						////�������100
						//point3=pt_dis[num];
						//point3.z=dizmin-100;
						//pt_fours.append(point3);
						//point3=pt_dis[num+row];
						//point3.z=dizmin-100;
						//pt_fours.append(point3);
						//point3=pt_dis[num+1+row];
						//point3.z=dizmin-100;
						//pt_fours.append(point3);
						//point3=pt_dis[num+1];
						//point3.z=dizmin-100;
						//pt_fours.append(point3);
						//pt_fours.append(pt_dis[num]);
						//pt_fours.append(pt_dis[num+row]);
						//pt_fours.append(pt_dis[num+1+row]);
						//pt_fours.append(pt_dis[num+1]);
						
						//������100
						/*pt_fours.append(pt_dins[num]);
						pt_fours.append(pt_dins[num+row]);
						pt_fours.append(pt_dins[num+1+row]);
						pt_fours.append(pt_dins[num+1]);
						point3=pt_dins[num];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dins[num+row];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dins[num+1+row];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dins[num+1];
						point3.z=dinzmax+100;
						pt_fours.append(point3);*/

						//�������¸�����100
						/*point3=pt_dis[num];
						point3.z=dizmin-100;
						pt_fours.append(point3);
						point3=pt_dis[num+row];
						point3.z=dizmin-100;
						pt_fours.append(point3);
						point3=pt_dis[num+1+row];
						point3.z=dizmin-100;
						pt_fours.append(point3);
						point3=pt_dis[num+1];
						point3.z=dizmin-100;
						pt_fours.append(point3);
						point3=pt_dis[num];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dis[num+row];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dis[num+1+row];
						point3.z=dinzmax+100;
						pt_fours.append(point3);
						point3=pt_dis[num+1];
						point3.z=dinzmax+100;
						pt_fours.append(point3);*/
					}
					
				}
				
				if(n==row-1) n=0;
				else n=n+1;

				oktet=oktet+1;
				//if(oktet==1) break;
			}
			pBlockTableRecord->close();
		}
		pt_dins.removeAll();
		pt_dis.removeAll();
	}
}

//2013-2-21������ɫ,��max-min��Ϊ15�ݣ�
AcCmColor getcolor(double max,double min,double val)
{
	AcCmColor color;
	double denf;

	denf=(max-min)/15.0;

	if(val>=min&&val<min+denf) color.setRGB(0,207,255);   //��һ��
	else if(val>=min+denf&&val<min+2*denf) color.setRGB(0,144,255);   //��2��
	else if(val>=min+2*denf&&val<min+3*denf) color.setRGB(0,0,255);   //��3��
	else if(val>=min+3*denf&&val<min+4*denf) color.setRGB(0,255,0);   //��4��
	else if(val>=min+4*denf&&val<min+5*denf) color.setRGB(0,207,0);   //��5��
	else if(val>=min+5*denf&&val<min+6*denf) color.setRGB(0,144,0);   //��6��
	else if(val>=min+6*denf&&val<min+7*denf) color.setRGB(0,96,0);   //��7��
	else if(val>=min+7*denf&&val<min+8*denf) color.setRGB(255,255,0);   //��8��
	else if(val>=min+8*denf&&val<min+9*denf) color.setRGB(255,144,0);   //��9��
	else if(val>=min+9*denf&&val<min+10*denf) color.setRGB(255,0,0);   //��10��
	else if(val>=min+10*denf&&val<min+11*denf) color.setRGB(207,0,0);   //��11��
	else if(val>=min+11*denf&&val<min+12*denf) color.setRGB(144,0,0);   //��12��
	else if(val>=min+12*denf&&val<min+13*denf) color.setRGB(255,0,255);   //��13��
	else if(val>=min+13*denf&&val<min+14*denf) color.setRGB(144,96,147);   //��14��
	else color.setRGB(255,255,255);   //��15��

	return color;

}
//2017-6-25����AcDb3dSolidʵ��ͨ��extrude����
//2017-6-30��������������չ�����������
//2018-9-10�޸ģ��������治���γ�����������޸�
AcDb3dSolid *cextrudeSolid(double pt0[3],double pt1[3],double pt2[3],double hz)
{
	AcDb3dSolid *pSolid;
	AcDb3dPolyline *pNewPline,*pPathPline;
	AcDb3dPolylineVertex *pVertex;
	AcDbRegion *pRegion;

	AcDbVoidPtrArray curveSegments,regions;
	AcGePoint3d point3;
	AcGePoint3d pt_mid;

	pSolid=NULL;
	//����extrudeʵ��
	
	//(1)�õ�·��
	pt_mid.x=point3.x=(pt0[0]+pt1[0]+pt2[0])/3.0;
	pt_mid.y=point3.y=(pt0[1]+pt1[1]+pt2[1])/3.0;
	pt_mid.z=(pt0[2]+pt1[2]+pt2[2])/3.0;
	point3.z=hz;
	pPathPline=new AcDb3dPolyline();
	pVertex=new AcDb3dPolylineVertex(pt_mid);
	pPathPline->appendVertex(pVertex);
	pVertex->close();
	pVertex=new AcDb3dPolylineVertex(point3);
	pPathPline->appendVertex(pVertex);
	pVertex->close();

	for(int a=0;a<3;a++)
	{
		//��2������������
		pNewPline=new AcDb3dPolyline();
		if(a==0){point3.x=pt0[0];point3.y=pt0[1];point3.z=pt0[2];}
		else if(a==1) {point3.x=pt1[0];point3.y=pt1[1];point3.z=pt1[2];}
		else {point3.x=pt2[0];point3.y=pt2[1];point3.z=pt2[2];}
		pVertex=new AcDb3dPolylineVertex(point3);
		pNewPline->appendVertex(pVertex);
		pVertex->close();
		if(a==0){point3.x=pt1[0];point3.y=pt1[1];point3.z=pt1[2];}
		else if(a==1) {point3.x=pt2[0];point3.y=pt2[1];point3.z=pt2[2];}
		else {point3.x=pt0[0];point3.y=pt0[1];point3.z=pt0[2];}
		pVertex=new AcDb3dPolylineVertex(point3);
		pNewPline->appendVertex(pVertex);
		pVertex->close();
		if(a==0){point3.x=pt2[0];point3.y=pt2[1];point3.z=pt2[2];}
		else if(a==1) {point3.x=pt0[0];point3.y=pt0[1];point3.z=pt0[2];}
		else {point3.x=pt1[0];point3.y=pt1[1];point3.z=pt1[2];}
		pVertex=new AcDb3dPolylineVertex(point3);
		pNewPline->appendVertex(pVertex);
		pVertex->close();
		if(a==0){point3.x=pt0[0];point3.y=pt0[1];point3.z=pt0[2];}
		else if(a==1) {point3.x=pt1[0];point3.y=pt1[1];point3.z=pt1[2];}
		else {point3.x=pt2[0];point3.y=pt2[1];point3.z=pt2[2];}
		pVertex=new AcDb3dPolylineVertex(point3);
		pNewPline->appendVertex(pVertex);
		pVertex->close();
		pNewPline->makeClosed();

		//pBlockTableRecord->appendAcDbEntity(pNewPline);
		curveSegments.append(pNewPline);
		if(AcDbRegion::createFromCurves(curveSegments,regions)==Acad::eOk)
		{
			pRegion=(AcDbRegion *)regions[0];
			pSolid=new AcDb3dSolid();
			pSolid->extrudeAlongPath(pRegion,pPathPline,0.0);
			
			//pBlockTableRecord->appendAcDbEntity(pSolid);
			//Setentlayer(pDb,pSolid,_T("Extrudeʵ��"),3);
			//pSolid->close();
			//pBlockTableRecord->appendAcDbEntity(pRegion);
			pRegion->close();
			break;
		}
		/*else 
		{
			ads_printf( ACRX_T("\nû�γ�����") );
			acutPrintf(_T("\n(%0.2f,%0.2f,%0.2f),(%0.2f,%0.2f,%0.2f),(%0.2f,%0.2f,%0.2f)"),
				pt0[0],pt0[1],pt0[2],pt1[0],pt1[1],pt1[2],pt2[0],pt2[1],pt2[2]);
		}*/
		pNewPline->close();
		curveSegments.removeAll();
	}
	//pBlockTableRecord->appendAcDbEntity(pPathPline);
	pPathPline->close();

	curveSegments.removeAll();
	regions.removeAll();

	return pSolid;
}
//2017-6-25����AcDb3dSolidʵ��ͨ��extrude����
//2017-6-30��������������չ����������죬����α�������ͬһƽ��
AcDb3dSolid *cextrudeSolid(AcGePoint3dArray pt_pls,double hz)
{
	//AcDbBlockTable *pBlockTable=NULL;
	//AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	//pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	//AcDbBlockTableRecord *pBlockTableRecord=NULL;
	//pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
	//	AcDb::kForWrite);
	//pBlockTable->close();
	//pBlockTable=NULL;

	AcDb3dSolid *pSolid;
	AcDb3dPolyline *pNewPline,*pPathPline;
	AcDb3dPolylineVertex *pVertex;
	AcDbRegion *pRegion;

	AcDbVoidPtrArray curveSegments,regions;
	AcGePoint3d point3;
	AcGePoint3d pt_mid;

	pSolid=NULL;
	//����extrudeʵ��
	//��1�������������
	pt_mid.x=pt_mid.y=pt_mid.z=0.0;
	pNewPline=new AcDb3dPolyline();
	for(int np=0;np<pt_pls.length();np++)
	{
		pVertex=new AcDb3dPolylineVertex(pt_pls[np]);
		pNewPline->appendVertex(pVertex);
		pVertex->close();
		pt_mid.x=pt_mid.x+pt_pls[np].x;
		pt_mid.y=pt_mid.y+pt_pls[np].y;
		pt_mid.z=pt_mid.z+pt_pls[np].z;
	}
	pNewPline->makeClosed();
	//(2)�õ�·��
	pt_mid.x=point3.x=pt_mid.x/(double)pt_pls.length();
	pt_mid.y=point3.y=pt_mid.y/(double)pt_pls.length();
	pt_mid.z=pt_mid.z/(double)pt_pls.length();
	point3.z=hz;
	pPathPline=new AcDb3dPolyline();
	pVertex=new AcDb3dPolylineVertex(pt_mid);
	pPathPline->appendVertex(pVertex);
	pVertex->close();
	pVertex=new AcDb3dPolylineVertex(point3);
	pPathPline->appendVertex(pVertex);
	pVertex->close();

	//pBlockTableRecord->appendAcDbEntity(pNewPline);
	curveSegments.append(pNewPline);
	if(AcDbRegion::createFromCurves(curveSegments,regions)==Acad::eOk)
	{
		pRegion=(AcDbRegion *)regions[0];
		pSolid=new AcDb3dSolid();
		pSolid->extrudeAlongPath(pRegion,pPathPline,0.0);
		
		/*pBlockTableRecord->appendAcDbEntity(pSolid);
		Setentlayer(pDb,pSolid,_T("Extrudeʵ��"),3);
		pSolid->close();*/
		//pBlockTableRecord->appendAcDbEntity(pRegion);
		pRegion->close();
	}
	pNewPline->close();
	//pBlockTableRecord->appendAcDbEntity(pPathPline);
	pPathPline->close();

	//pBlockTableRecord->close();

	curveSegments.removeAll();
	regions.removeAll();

	return pSolid;
}
//04-12-18����һ��ͳһ�ĺ��������ַ���Ϊ�������չ����
//2023-04-27����������Ϊ��չ����
void setFloatXdata(AcDbDatabase* pDatabase,AcDbEntity *pEnt,TCHAR *appName,double errs)
{	
	struct  resbuf  *pRb, *pTemp;

	pRb = pEnt->xData(appName);
	if (pRb != NULL)          //�������չ����
	{
		for (pTemp = pRb; pTemp->rbnext != NULL;
		pTemp = pTemp->rbnext)
		{ ;}
		pTemp->resval.rreal=errs;
		//pTemp->resval.rlong=(long)vol;
	}
	else //���������������չ����
	{
		//registerApplication(pDb,appName);
		acdbRegApp(appName);
		pRb = acutNewRb(AcDb::kDxfRegAppName);
		pTemp = pRb;
		pTemp->resval.rstring= (TCHAR*) malloc((_tcslen(appName) + 1) * sizeof(TCHAR));
		_tcscpy(pTemp->resval.rstring, appName);
		//�����չ����ֵ
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdReal);//����С��
		//pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger32);//��������
		pTemp = pTemp->rbnext;
		pTemp->resval.rreal=errs;
		//pTemp->resval.rlong=(long)vol;
				
	}
	
	pEnt->upgradeOpen();
	pEnt->setXData(pRb);

	acutRelRb(pRb);
}
///04-12-19����ͼ�㣬��ͼ�㽫ʵ����뵽ͼ���У�û���½�ͼ�㲢�������������
void Setentlayer(AcDbDatabase *pDb,AcDbEntity *pEnt,CString layerName,int i)
{
	AcDbLayerTable *pLayerTable;
	pDb->getSymbolTable(pLayerTable,AcDb::kForWrite);
	if(pLayerTable->has(layerName)==true)                //�и�ͼ��
	{
		pLayerTable->close();
		pEnt->setLayer(layerName);   //����ͼ��
		
	}
	else         //û��������ͼ�㣬�½�һ��
	{
		AcDbLayerTableRecord *pLayerTableRecord=new AcDbLayerTableRecord;
		pLayerTableRecord->setName(layerName);
		AcCmColor color;
		color.setColorIndex(i); // set color to red
		pLayerTableRecord->setColor(color);
    
		AcDbObjectId pLayerId;
		pLayerTable->add(pLayerId,pLayerTableRecord);

		pLayerTable->close();
		pLayerTableRecord->close();

		pEnt->setLayer(layerName);   //����ͼ��
	}
}

//2023-05-09�����ɳ�������ʵ��
void so_openpit()
{
	createopenpit();
}

//2023-05-09�����ɳ�������ʵ��
void createopenpit()
{
	AcDbEntity *pEnt;
	AcDbObjectId objId;
	
	AcDbFace *pFace;
	AcGePoint3d point3;
	
	int np,nvert0,nso;
	int ntri0;
	int a,i;
	
	double pt[3];
	double hval;            //���쵽�ĸ߳�

	DWORD dwStart = timeGetTime();
	
	//ʹ�����ݼ����в���
	CNewPoint newpoint;
	CNewTri newtri;
	CArray<CNewPoint,CNewPoint&>newpoints0;       //���㼯
	CArray<CNewTri,CNewTri&>newtris0;               //�����μ�
	bool isok;
	
	AcDb3dSolid *pSolid0;

	//��1����ö������������͵װ���������
	nvert0=0;
	ads_name ssname,en;
	acutPrintf(_T("\nѡ�����:"));
	if(acedSSGet(NULL,NULL,NULL,NULL,ssname)==RTNORM)
	{
		long ilast;
		acedSSLength(ssname,&ilast);   //���ѡ�񼯵ĳ���
		
		for( i=0;i<ilast;i++)
		{			
			//���ʵ�������
			acedSSName(ssname,i,en);
			acdbEntGet(en);

			Acad::ErrorStatus retState;
			retState=acdbGetObjectId(objId,en);
			acdbOpenObject(pEnt, objId, AcDb::kForRead);
			
			if(AcDbFace::cast(pEnt))
			{
				
				pFace=AcDbFace::cast(pEnt);
					
				//�������㼯
				for(a=0;a<3; a++)
				{
					pFace->getVertexAt(a,point3);
					pt[0]=point3.x;pt[1]=point3.y;pt[2]=point3.z;
					
					isok=false;
						
					for(np=0;np<newpoints0.GetSize();np++)
					{
						if(newpoints0[np].p[0]==pt[0]&&newpoints0[np].p[1]==pt[1]&&newpoints0[np].p[2]==pt[2])   //����ͬ�ĵ�
						{
							newtri.v[a]=np;
							isok=true;
							break;
						}
					}
					if(!isok)           //û����ͬ��
					{
						newpoint.p[0]=pt[0];newpoint.p[1]=pt[1];newpoint.p[2]=pt[2];
						newpoint.pno=nvert0;
						newpoints0.Add(newpoint);
						newtri.v[a]=nvert0;
							
						nvert0=nvert0+1;
					}
				}
				
				//���������漯
				//for(np=0;np<20;np++) newtri.inslnnos[np]=-1;
				newtri.direct=-1;                    //����ڲ�����ķ���
				newtri.objId=objId;
				newtri.inslnhead=NULL;
				newtris0.Add(newtri);         //��������

				pFace->close();
			}
			pEnt->close();
			pEnt=NULL;
		}
		acedSSFree(ssname);
	}
	dwStart=timeGetTime()-dwStart;
	acutPrintf(_T("\n��ȡ���ݵ�ʱ��=%d"),dwStart);
	dwStart=timeGetTime();

	//��2��������������������
	AcDbBlockTable *pBlockTable=NULL;
	AcDbDatabase *pDb=acdbHostApplicationServices()->workingDatabase();
	pDb->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, //�õ���������¼
        AcDb::kForWrite);
	pBlockTable->close();
	pBlockTable=NULL;

	
	AcGePoint3d minpt0,maxpt0;
	if(newtris0.GetSize()>0)
	{
		if(acedGetReal(_T("\n����ɳ�ʵ��ĸ߶�:"),&hval)==RTNORM)
		{
			acutPrintf(_T("\nhval=%f"),hval);
		
			for(ntri0=0;ntri0<newtris0.GetSize();ntri0++)
			{
				//��������������
				pSolid0=cextrudeSolid(newpoints0[newtris0[ntri0].v[0]].p,newpoints0[newtris0[ntri0].v[1]].p,newpoints0[newtris0[ntri0].v[2]].p,hval);
					
				if(pSolid0!=NULL)
				{
					pBlockTableRecord->appendAcDbEntity(pSolid0);
					Setentlayer(pDb,pSolid0,_T("�ɳ�ʵ��"),3);
					pSolid0->close();
				}
			}
		}
			
	}
	pBlockTableRecord->close();
		
	newpoints0.RemoveAll();
	newtris0.RemoveAll();

	dwStart=timeGetTime()-dwStart;
	acutPrintf(_T("\n����ʵ��ʱ��=%d"),dwStart);
}

//2017-06-24ʵ�岼������
void so_bool()
{
	CSolidMineDlg solidminedlg;
	solidminedlg.DoModal();
}

//2012-8-9���ò��Ժн��пռ��ཻ���ж�
bool boxinginter(AcGePoint3d minpt1,AcGePoint3d maxpt1,AcGePoint3d minpt2,AcGePoint3d maxpt2)
{
	//if(minpt1.x<=maxpt2.x&&minpt1.y<=maxpt2.y&&minpt1.z<=maxpt2.z&&
	//	maxpt1.x>=minpt2.x&&maxpt1.y>=minpt2.y&&maxpt1.z>=minpt2.z)
	//{
	//	//acutPrintf(_T("\n��Χ���ཻ"));
	//	return true;
	//}
	//else
	//{
	//	//acutPrintf(_T("\n��Χ�в��ཻ"));
	//	return false;
	//}

	//��a1Ϊ׼���ж�a2����ǰ�����º�����
	if(minpt1.y>maxpt2.y) return false;          //a1��a2ǰ
	else if(maxpt1.y<minpt2.y) return false;     //a1��a2��

	else if(maxpt1.x<minpt2.x) return false;     //a1��a2��
	else if(minpt1.x>maxpt2.x) return false;     //a1��a2��

	else if(minpt1.z>maxpt2.z) return false;     //a1��a2��
	else if(maxpt1.z<minpt2.z) return false;     //a1��a2��
	

	return true;
}

void DeleteMemory(new_solid *head)
{
	new_solid *p=head, *pr=NULL;
	while (p!=NULL) 
	{
		pr=p; 
		p=p->next; 
		free(pr); 
	}
}
//2017-11-27�õ�AsdkBody��AcDb3dSolidʵ������.
void getasdkvol()
{
	getAsdkBodyVol();
}
//2017-11-27�õ�AsdkBody��AcDb3dSolidʵ������.
//2023-04-27���ļ������AcDb3dSolidʵ�������;���
void getAsdkBodyVol()
{
	AcDbEntity *pEnt;
	AcDbObjectId objId;
	
	AsdkBody *pBody0;
	AcDb3dSolid *pSolid0;
	
	double volume;
	double pt[3];
	int i,j;
	AcDbExtents extents0;
	AcGeVector3d prinAxes[3];
	AcGePoint3d centroid;
	AcGePoint2d point2d;
	CArray<CSolid_vol,CSolid_vol&> csvol;
	CSolid_vol svol;
	CString layer;

	TCHAR *appName;
	struct  resbuf  *pRb;

	double vol,errs;
	//��1����ò���ʵ��ͱ�����ʵ��
	ads_name ssname,en;
	acutPrintf(_T("\nѡ�����:"));
	if(acedSSGet(NULL,NULL,NULL,NULL,ssname)==RTNORM)
	{
		long ilast;
		acedSSLength(ssname,&ilast);   //���ѡ�񼯵ĳ���
		
		vol=0;
		for(i=0;i<ilast;i++)
		{			
			//���ʵ�������
			acedSSName(ssname,i,en);
			acdbEntGet(en);

			Acad::ErrorStatus retState;
			retState=acdbGetObjectId(objId,en);
			acdbOpenObject(pEnt, objId, AcDb::kForWrite);
			layer=pEnt->layer();
			//acutPrintf(_T("\nlayer=%s"),layer);
			if(AsdkBody::cast(pEnt))
			{
				pBody0=AsdkBody::cast(pEnt);
				vol=vol+pBody0->body().volume();
				pBody0->close();
				pBody0=NULL;
			}
			if(AcDb3dSolid::cast(pEnt))
			{
				//�õ�ʵ��ľ��Ⱥ����
				errs=-1;
				appName=_T("errors");
				pRb = pEnt->xData(appName);
				if (pRb != NULL)          //�������չ����
				{
					for( j = 0;pRb != NULL;j++, pRb = pRb->rbnext)
					{ 
						if (pRb->restype == AcDb::kDxfXdReal)
						{
							errs=pRb->resval.rreal;
						}
					}
				}
				extents0=AcDbExtents();
				pSolid0=AcDb3dSolid::cast(pEnt);
				pSolid0->getMassProp(volume,centroid,pt,pt,pt,prinAxes,pt,extents0);
				if(errs!=-1)
				{
					svol.errs=errs;
					svol.vol=volume;
					svol.layer=layer;
					csvol.Add(svol);
				}
						
				vol=vol+volume;
				pSolid0->close();
				pSolid0=NULL;
			}
			
			pEnt->close();
			pEnt=NULL;
		}
		acedSSFree(ssname);

		if(csvol.GetSize()>0)
		{
			CFileDialog filedialog(TRUE, _T("txt"), NULL, OFN_ALLOWMULTISELECT|OFN_ENABLESIZING |OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  _T("�����ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||"),NULL);
			if(filedialog.DoModal()==IDOK)
			{
				FILE *file=NULL;
				POSITION pos = filedialog.GetStartPosition();
				while(pos)
				{
					CString filename=filedialog.GetNextPathName(pos);
					USES_CONVERSION;

					char *cfile;
					cfile=T2A(filename.GetBuffer(0));
					file=fopen(cfile,"w");

					CString str1;
					if(file==0)
					{
						str1=_T("���ļ�")+(CString)filename+_T("ʧ��!");
						AfxMessageBox(str1);		
					}
					else
					{
						for(i=0;i<csvol.GetSize();i++)
						{
							fwprintf(file,_T("%i,%s,%f,%f\n"),i,csvol[i].layer,csvol[i].vol,csvol[i].errs);
						}
					}
					fclose(file);
			
				}
				csvol.RemoveAll();
			}
			
		}

		acutPrintf(_T("\n��ѡʵ��������%f"),vol);
	}
}
