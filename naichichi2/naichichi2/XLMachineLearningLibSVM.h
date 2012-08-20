#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>

struct svm_model;

//liblinear ��֗��Ɏg�����߂̃��C�u����
class XLMachineLearningLibSVM
{
public:
	struct feature {
		int index;
		double value;
	};

	XLMachineLearningLibSVM();
	
	virtual ~XLMachineLearningLibSVM();

	//�t�@�C������w�K�f�[�^��ǂݍ���
	bool LoadTrain(const std::string& filename);

	//�������񂾂Ƃ��Ɠ����t�@�C���t�H�[�}�b�g���g���āA�F�����𑪒肵�܂��B �f�o�b�O�p
	bool DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist);

	//�w�K�f�[�^�̒ǉ� (liblinear ���ނ��I�����C���ł��Ȃ��炵������d���Ȃ����ǂ�)
	void AppendTrain(int classID,const std::vector<XLMachineLearningLibSVM::feature>& newnodes);

	//�w�K���܂��B
	bool Train();

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::vector<feature>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::map<int,double>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const XLMachineLearningLibSVM::feature* params);

	bool SaveModel(const std::string& filename);

	bool LoadModel(const std::string& filename);

	void FreeModel();
private:
	std::vector<double> ProblemY;
	std::vector<XLMachineLearningLibSVM::feature*> ProblemX;
	std::vector<XLMachineLearningLibSVM::feature>  RealFeature;
	svm_model* Model;
};
