//cloth simulation filter for airborne lidar filtering
#ifndef _CSF_H_
#define _CSF_H_

#include "PointCloud.h"
#include "Cloth.h"

//system
#include <vector>
#include <string>

class ccMainAppInterface;
class QWidget;
class ccMesh;

class CSF
{
public:
	CSF(wl::PointCloud& cloud);
	virtual ~CSF();

	//input PC from files
	bool readPointsFromFile(std::string filename);

	//save the ground points to file
	void saveGroundPoints(const std::vector<int>& grp, std::string path = "");
	void saveOffGroundPoints(const std::vector<int>& grp, std::string path = "");
	
	//ִ���˲����� �õ���������PointCloud �е����
	bool do_filtering(	std::vector<int>& groundIndexes,
						std::vector<int>& offGroundIndexes,
						bool exportClothMesh,
						ccMesh* &clothMesh,
						ccMainAppInterface* app = 0,
						QWidget* parent = 0);

private:
	wl::PointCloud& point_cloud;

public:

	struct Parameters
	{
		//parameters
		//���ٽ������ǵĵ�����һ������Ϊ1
		int k_nearest_points;

		//�Ƿ���б��º���
		bool bSloopSmooth;

		//ʱ�䲽��
		double time_step;

		//������ֵ
		double class_threshold;

		//���ϸ�����С
		double cloth_resolution;

		//����Ӳ�Ȳ���
		int rigidness;

		//����������
		int iterations;
	};
	
	Parameters params;
};

#endif