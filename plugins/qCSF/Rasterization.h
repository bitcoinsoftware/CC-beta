#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_

#include "Cloth.h"
#include "PointCloud.h"

class Rasterization
{
public:

	//�Ե��ƽ������ٽ�������Ѱ����Χ�����N����  ����������
	static bool RasterTerrain(const Cloth& cloth, const wl::PointCloud& pc, std::vector<double>& heightVal, unsigned KNN);
};

#endif //_RASTERIZATION_H_
