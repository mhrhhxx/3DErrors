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


//2018-06-23对不确定模型生成AcDb3dSolid实体模型，数据文件的格式：x y diz dinz errors
//自动提取点的行数和列数
void so_showerrors();
//2013-2-21返回颜色,将max-min分为15份，
AcCmColor getcolor(double max,double min,double val);
//2017-6-25建立AcDb3dSolid实体通过extrude方法
//2017-6-30从三角面拉伸扩展到多边形拉伸
AcDb3dSolid *cextrudeSolid(AcGePoint3dArray pt_pls,double hz);
//2017-6-25建立AcDb3dSolid实体通过extrude方法
AcDb3dSolid *cextrudeSolid(double pt0[3],double pt1[3],double pt2[3],double hz);

//04-12-18建立一个统一的函数，将字符设为对象的扩展数据
//2023-04-27将浮点设置为扩展数据
void setFloatXdata(AcDbDatabase* pDatabase,AcDbEntity *pEnt,TCHAR *appName,double errs);
///04-12-19建立图层，有图层将实体加入到图层中，没有新建图层并将对象加入其中
void Setentlayer(AcDbDatabase *pDb,AcDbEntity *pEnt,CString layerName,int i);
//2023-05-09建立采场的拉伸实体
void createopenpit();
//2023-05-09建立采场的拉伸实体
void so_openpit();
//2017-06-24建立矿山的实体地质模型
void so_bool();
//2017-11-27得到AsdkBody和AcDb3dSolid实体的体积.
void getasdkvol();
//2017-11-27得到AsdkBody和AcDb3dSolid实体的体积.
void getAsdkBodyVol();

//2012-8-9利用测试盒进行相交性判断
bool boxinginter(AcGePoint3d minpt1,AcGePoint3d maxpt1,AcGePoint3d minpt2,AcGePoint3d maxpt2);
void DeleteMemory(new_solid *head);





