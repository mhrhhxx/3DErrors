#include "afxtempl.h"
#include "math.h"

#include "dbents.h"
#include "adslib.h"
#include "dbpl.h"
#include "dbsymtb.h"
#include "rxregsvc.h"
#include "dbxrecrd.h"
#include "rxmfcapi.h"
#include "acedads.h"
#include "dbregion.h"
#include "tchar.h"
#include "acestext.h"
#include <dbhatch.h >       //充填

#include "aced.h"
#include "dbapserv.h"
#include "geassign.h"
#include "adscodes.h"
#include "achapi.h"         //超级链接
#include "geplane.h"         //超级链接

#include "dbsol3d.h"         //三维实体
#include "dbbody.h"         //三维实体
#include "dbgroup.h"        //组
#include "migrtion.h"

#include "acdb.h"               // acdb definitions
#include "acdocman.h"           // MDI document manager
#include "acgiutil.h"           // graphical definitions

//点、线号结构
typedef struct new_plno
{
	long plno;                   //交线编号
	new_plno *next;
}new_plno;

class CNewPoint
{
public:
	
	long pno;                         //用来存数据点的编号
	double p[3];                       //顶点坐标
	int volume;            //列号
	int row;               //行号
	int lno;               //图层
	double vertdis;                    //到三角面网的垂直距离
};
class CNewTri      //三角形
{
public:
	long trino;                         //用来存三角形编号
	long v[3];                          //三角形的三个顶点编号
	int layer;                          //图层号
	int color;                          
	double direct;                         //相对于裁切面的方向
	bool xl;                            //是否有相邻三角形
	AcDbObjectId objId;               //三角面的id号
	//int inslnnos[20];             //交线号组
	AcGePoint3d pt_center;           //四面体的中心点
	AcGePoint3d minpt1,maxpt1;      //矩形的盒角点

	new_plno *inslnhead;          //相交线号首地址
};
//2017-6-29AcDb3dSolid实体数据结构
typedef struct new_solid
{
	double vol;                     //体体积
	AcGePoint3d centroid;
	AcDbExtents extents;
	AcDbObjectId objId;             //对应AcDb3dSolid的ID号
	CString layer;
	new_solid *next;
}new_solid;
//2023-04-27实体精度及体积
class CSolid_vol
{	
public:
	double vol;                      //实体体积
	double errs;                     //实体精度
	CString layer;                   //图层名
};