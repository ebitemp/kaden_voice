#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>

struct model;

//liblinear ��֗��Ɏg�����߂̃��C�u����
class XLMachineLearningLibliear
{
public:
	struct feature {
		int index;
		double value;
	};

	XLMachineLearningLibliear();
	
	virtual ~XLMachineLearningLibliear();

	//�t�@�C������w�K�f�[�^��ǂݍ���
	bool LoadTrain(const std::string& filename);

	//�������񂾂Ƃ��Ɠ����t�@�C���t�H�[�}�b�g���g���āA�F�����𑪒肵�܂��B �f�o�b�O�p
	bool DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist);

	//�w�K�f�[�^�̒ǉ� (liblinear ���ނ��I�����C���ł��Ȃ��炵������d���Ȃ����ǂ�)
	void AppendTrain(int classID,const std::vector<XLMachineLearningLibliear::feature>& newnodes);

	//�w�K���܂��B
	bool Train();

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::vector<feature>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::map<int,double>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const XLMachineLearningLibliear::feature* params);

	bool SaveModel(const std::string& filename);

	bool LoadModel(const std::string& filename);

	void FreeModel();
private:
	std::vector<int> ProblemY;
	std::vector<XLMachineLearningLibliear::feature*> ProblemX;
	std::vector<XLMachineLearningLibliear::feature>  RealFeature;
	model* Model;
};
