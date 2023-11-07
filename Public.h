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
#include <dbhatch.h >       //����

#include "aced.h"
#include "dbapserv.h"
#include "geassign.h"
#include "adscodes.h"
#include "achapi.h"         //��������
#include "geplane.h"         //��������

#include "dbsol3d.h"         //��άʵ��
#include "dbbody.h"         //��άʵ��
#include "dbgroup.h"        //��
#include "migrtion.h"

#include "acdb.h"               // acdb definitions
#include "acdocman.h"           // MDI document manager
#include "acgiutil.h"           // graphical definitions

//�㡢�ߺŽṹ
typedef struct new_plno
{
	long plno;                   //���߱��
	new_plno *next;
}new_plno;

class CNewPoint
{
public:
	
	long pno;                         //���������ݵ�ı��
	double p[3];                       //��������
	int volume;            //�к�
	int row;               //�к�
	int lno;               //ͼ��
	double vertdis;                    //�����������Ĵ�ֱ����
};
class CNewTri      //������
{
public:
	long trino;                         //�����������α��
	long v[3];                          //�����ε�����������
	int layer;                          //ͼ���
	int color;                          
	double direct;                         //����ڲ�����ķ���
	bool xl;                            //�Ƿ�������������
	AcDbObjectId objId;               //�������id��
	//int inslnnos[20];             //���ߺ���
	AcGePoint3d pt_center;           //����������ĵ�
	AcGePoint3d minpt1,maxpt1;      //���εĺнǵ�

	new_plno *inslnhead;          //�ཻ�ߺ��׵�ַ
};
//2017-6-29AcDb3dSolidʵ�����ݽṹ
typedef struct new_solid
{
	double vol;                     //�����
	AcGePoint3d centroid;
	AcDbExtents extents;
	AcDbObjectId objId;             //��ӦAcDb3dSolid��ID��
	CString layer;
	new_solid *next;
}new_solid;
//2023-04-27ʵ�徫�ȼ����
class CSolid_vol
{	
public:
	double vol;                      //ʵ�����
	double errs;                     //ʵ�徫��
	CString layer;                   //ͼ����
};