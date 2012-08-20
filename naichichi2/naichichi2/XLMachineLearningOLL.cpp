#include "XLMachineLearningOLL.h"
//oll�{��
#include "../oll/oll.hpp"
#include <algorithm>


#if _MSC_VER
	#pragma comment(lib, "oll.lib")
#endif

#if !defined(max)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

//�f�o�b�O�p
double getFeatureValue(const XLMachineLearningOLL::feature* feature_nodes , int index) 
{
	const XLMachineLearningOLL::feature* featureP = feature_nodes;
	for( ; 1 ; ++featureP )
	{
		if ( featureP == NULL || featureP->index == -1 ) break;
		if (featureP->index == index)
		{
			return featureP->value;
		}
	}
	return 0;
}

bool IgnoreFeature(int index) 
{
	return false;

	//����̑f���𖳌���
	if (index >= 10 
//		&& 
//		!(
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 12 )
//		(((index - 10) % 25) == 12 )
//		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )

//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) >= 12 &&  ((index - 10) % 25) <= 23 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) == 12 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) == 24 )
//		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )
//||		(((index - 10) % 25) == 12 )
//		0
//		) 
	) 
			return true;

	return false;
}



XLMachineLearningOLL::XLMachineLearningOLL(float C = 1.f, float bias = 0.f)
{
	this->tm_ = new oll_tool::trainMethod( oll_tool::PA1 );
	this->ol_ = new oll_tool::oll;
	
	ol_.setC(C);
	ol_.setBias(bias);
}
XLMachineLearningOLL::~XLMachineLearningOLL()
{
	delete this->tm_;
	delete this->ol_;
}

//�t�@�C������w�K�f�[�^��ǂݍ���
bool XLMachineLearningOLL::LoadTrain(const std::string& filename)
{
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{
		return false;
	}

	std::vector<char> buffer;
	buffer.resize(125535);
	while (! feof(fp) )
	{
		char * p = &buffer[0];
		fgets(p,buffer.size(), fp);
		if (*p == '#' || *p == '\n' || *p == '\0') continue; //�R�����g�Ƃ�

		oll_tool::fv_t fv;
		int y;
		if (ol_.parseLine(p, fv, y) == -1) {
			std::cerr << ol_.getErrorLog() << std::endl;

			fclose(fp);
			return false;
		}

		ol_.trainExample(oll_tool::PA1_s(), fv, y);
	}
	fclose(fp);
	return true;
}

//�w�K�f�[�^�̒ǉ�
void XLMachineLearningOLL::AppendTrain(int classID,const std::vector<XLMachineLearningLibliear::feature>& newnodes)
{
}


//�������񂾂Ƃ��Ɠ����t�@�C���t�H�[�}�b�g���g���āA�F�����𑪒肵�܂��B �f�o�b�O�p
bool XLMachineLearningOLL::DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist);
{
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{
		return false;
	}
	int all = 0;
	int match = 0;
	int Dcount = 0;

	std::string procfilename;

	std::vector<char> buffer;
	buffer.resize(125535);
	while (! feof(fp) )
	{
		char * p = &buffer[0];
		fgets(p,buffer.size(), fp);
		if (*p == '#' || *p == '\n' || *p == '\0')
		{//�R�����g�͂��̂܂܏o�͂���.
			if (*p == '#')
			{
				procfilename = p + 2;
			}
			continue;
		}

		oll_tool::fv_t fv;
		int y;
		if (ol_.parseLine(p, fv, y) == -1) {
			std::cerr << ol_.getErrorLog() << std::endl;

			fclose(fp);
			return false;
		}

		int classid = atoi(p);

		//���ʂ���Ă݂܂��B
		int predictClassID = ol_.classify(fv);

		//���ʂ𑪒肵�܂��B
		if (predictClassID == classid)
		{
			match ++;
		}
		else
		{
			outbadfilelist->push_back( procfilename );

		}
		all ++;
	}
	fclose(fp);

	*outall = all;
	*outmatch = match;
	*outDcount = Dcount;
	return true;
}

//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningOLL::Predict(const XLMachineLearningOLL::feature* params)
{
	oll_tool::fv_t fv;
	for(const XLMachineLearningLibliear::feature* p = params ; p->index != -1 ; p++ )
	{
		fv.push_back(std::pair<int,double>(p->index,p->value) );
	}
	return ol_.classify(fv);
}


//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningOLL::Predict(const std::map<int,double>& params)
{
	if (params.empty())
	{
		return 0;
	}
	
	oll_tool::fv_t fv;
	for(std::map<int,double>::const_iterator it = params.begin() ; it != params.end() ; ++it )
	{
		fv.push_back(std::pair<int,double>(it->first,it->second) );
	}
	return ol_.classify(fv);
}

//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningOLL::Predict(const std::vector<feature>& params)
{
	if (params.empty())
	{
		return 0;
	}
	
	oll_tool::fv_t fv;
	for(std::map<int,double>::const_iterator it = params.begin() ; it != params.end() ; ++it )
	{
		fv.push_back(std::pair<int,double>(it->index,it->value) );
	}
	return ol_.classify(fv);
}

bool XLMachineLearningOLL::SaveModel(const std::string& filename)
{
	if ( ol_.save(filename.c_str()) == -1) {
		std::cerr << ol_.getErrorLog() << std::endl;
		return false;
	}
	return true;
}
bool XLMachineLearningOLL::LoadModel(const std::string& filename)
{
	if ( ol_.load(filename.c_str()) == -1) {
		std::cerr << ol_.getErrorLog() << std::endl;
		return false;
	}
	return true;
}

