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


#include "resource.h"       // main symbols

#include "Public.h"

#define PI  3.14159265359
#define E  2.718281828459
/////////////////////////////////////////////////////////////////////////////


//2018-06-23�Բ�ȷ��ģ������AcDb3dSolidʵ��ģ�ͣ������ļ��ĸ�ʽ��x y diz dinz errors
//�Զ���ȡ�������������
void so_showerrors();
//2013-2-21������ɫ,��max-min��Ϊ15�ݣ�
AcCmColor getcolor(double max,double min,double val);
//2017-6-25����AcDb3dSolidʵ��ͨ��extrude����
//2017-6-30��������������չ�����������
AcDb3dSolid *cextrudeSolid(AcGePoint3dArray pt_pls,double hz);
//2017-6-25����AcDb3dSolidʵ��ͨ��extrude����
AcDb3dSolid *cextrudeSolid(double pt0[3],double pt1[3],double pt2[3],double hz);

//04-12-18����һ��ͳһ�ĺ��������ַ���Ϊ�������չ����
//2023-04-27����������Ϊ��չ����
void setFloatXdata(AcDbDatabase* pDatabase,AcDbEntity *pEnt,TCHAR *appName,double errs);
///04-12-19����ͼ�㣬��ͼ�㽫ʵ����뵽ͼ���У�û���½�ͼ�㲢�������������
void Setentlayer(AcDbDatabase *pDb,AcDbEntity *pEnt,CString layerName,int i);
//2023-05-09�����ɳ�������ʵ��
void createopenpit();
//2023-05-09�����ɳ�������ʵ��
void so_openpit();
//2017-06-24������ɽ��ʵ�����ģ��
void so_bool();
//2017-11-27�õ�AsdkBody��AcDb3dSolidʵ������.
void getasdkvol();
//2017-11-27�õ�AsdkBody��AcDb3dSolidʵ������.
void getAsdkBodyVol();

//2012-8-9���ò��Ժн����ཻ���ж�
bool boxinginter(AcGePoint3d minpt1,AcGePoint3d maxpt1,AcGePoint3d minpt2,AcGePoint3d maxpt2);
void DeleteMemory(new_solid *head);





