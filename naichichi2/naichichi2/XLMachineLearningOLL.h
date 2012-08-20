#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>

class oll_tool::oll ol_;
class oll_tool::trainMethod tm_;


//liblinear ��֗��Ɏg�����߂̃��C�u����
class XLMachineLearningLibliear
{
public:
	XLMachineLearningLibliear();
	
	virtual ~XLMachineLearningLibliear();

	//�t�@�C������w�K�f�[�^��ǂݍ���
	bool LoadTrain(const std::string& filename);

	//�������񂾂Ƃ��Ɠ����t�@�C���t�H�[�}�b�g���g���āA�F�����𑪒肵�܂��B �f�o�b�O�p
	bool DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist);

	//�w�K�f�[�^�̒ǉ�
	void AppendTrain(int classID,const std::vector<XLMachineLearningLibliear::feature>& newnodes);

	//�w�K���܂��B �I�����C���w�K�@�Ȃ̂Ŋ֌W�Ȃ� �݊����̂��߂Ɏc��
	bool Train() { return true; };

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::vector<feature>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const std::map<int,double>& params);

	//�w�K�������f������N���X�ԍ����擾���܂��B
	int Predict(const XLMachineLearningLibliear::feature* params);

	bool SaveModel(const std::string& filename);

	bool LoadModel(const std::string& filename);

//	void FreeModel();
private:
	class oll_tool::oll* ol_;
	class oll_tool::trainMethod* tm_;
};
