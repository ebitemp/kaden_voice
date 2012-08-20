#include "XLMachineLearningLibliear.h"
//liblinear�{��
#include "../liblinear/linear.h"
#include <algorithm>


#if _MSC_VER
	#pragma comment(lib, "liblinear.lib")
#endif

#if !defined(max)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

//�f�o�b�O�p
static double getFeatureValue(const XLMachineLearningLibliear::feature* feature_nodes , int index) 
{
	const XLMachineLearningLibliear::feature* featureP = feature_nodes;
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

static bool IgnoreFeature(int index) 
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



XLMachineLearningLibliear::XLMachineLearningLibliear()
{
	this->Model = NULL;
}

XLMachineLearningLibliear::~XLMachineLearningLibliear()
{
	for(std::vector<XLMachineLearningLibliear::feature*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
	{
		delete [] *it;
	}
	FreeModel();
}

//�t�@�C������w�K�f�[�^��ǂݍ���
bool XLMachineLearningLibliear::LoadTrain(const std::string& filename)
{
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{
		return false;
	}

	std::vector<char> buffer;
	buffer.resize(65535);
	while (! feof(fp) )
	{
		char * p = &buffer[0];
		fgets(p,buffer.size(), fp);
		if (*p == '#' || *p == '\n' || *p == '\0') continue; //�R�����g�Ƃ�

		char * startN = p;

		//���� : �����邩�𐔂��܂��B
		int featurecount = 0;
		for( ; *startN  ; ++startN ) 
		{
			if (*startN == ':') featurecount ++;
		}

		//�N���X�ԍ��Ƃ̋�؂��
		startN = p;
		for( ; *p != ' ' && *p != '\t' && *p != '\n' && *p != '\0'; ++p );
		if (*p == '\0' || *p == '\n' ) continue;
		*p = '\0';

		//�f���̐������m��(����Ɋm�ۂ��Ȃ��ƃ_��)
		feature* feature_nodes = new feature[featurecount + 1]; //�I�[������̂� +1����

		//�N���X�ԍ�
		const int classid = atoi(startN);
		ProblemY.push_back(classid);
		ProblemX.push_back(feature_nodes);

		feature* featureP = feature_nodes;
		for( ; 1 ; ++featureP )
		{
			//�f���ԍ�
			p ++;
			startN = p;
			for( ; *p != ':' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' ) break;
			*p = '\0';
			featureP->index = atoi(startN);

			//�f���̒l
			p ++;
			startN = p;
			for( ; *p != ' ' && *p != '\t' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' )
			{
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
				break;
			}
			else
			{
				*p = '\0';
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
			}
		}
		//�I�[�}�[�N
		featureP->index = -1;
		featureP->value = 0;
	}
	fclose(fp);
	return true;
}
//�������񂾂Ƃ��Ɠ����t�@�C���t�H�[�}�b�g���g���āA�F�����𑪒肵�܂��B �f�o�b�O�p
bool XLMachineLearningLibliear::DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist)
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
	buffer.resize(262144);
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

		char * startN = p;

		//���� : �����邩�𐔂��܂��B
		int featurecount = 0;
		for( ; *startN  ; ++startN ) 
		{
			if (*startN == ':') featurecount ++;
		}

		//�N���X�ԍ��Ƃ̋�؂��
		startN = p;
		for( ; *p != ' ' && *p != '\t' && *p != '\n' && *p != '\0'; ++p );
		if (*p == '\0' || *p == '\n' ) continue;
		*p = '\0';

		//�ǂ�Ȓl����������ۑ����Ă����܂��B
		std::string feature_string = p + 1;

		//�f���̐������m��(����Ɋm�ۂ��Ȃ��ƃ_��)
		feature* feature_nodes = new feature[featurecount + 1]; //�I�[������̂� +1����

		//�N���X�ԍ�
		const int classid = atoi(startN);

		feature* featureP = feature_nodes;
		for( ; 1 ; ++featureP )
		{
			//�f���ԍ�
			p ++;
			startN = p;
			for( ; *p != ':' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' ) break;
			*p = '\0';
			featureP->index = atoi(startN);

			//�f���̒l
			p ++;
			startN = p;
			for( ; *p != ' ' && *p != '\t' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' )
			{
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
				break;
			}
			else
			{
				*p = '\0';
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
			}
		}
		//�I�[�}�[�N
		featureP->index = -1;
		featureP->value = 0;

		//���ʂ���Ă݂܂��B
		int predictClassID = this->Predict(feature_nodes);

		//���ʂ𑪒肵�܂��B
		if (predictClassID == classid)
		{
			match ++;
		}
		else
		{
			outbadfilelist->push_back( procfilename );

			if ( getFeatureValue(feature_nodes,8) <= 201 )
			{
				Dcount ++;
			}
		}
		all ++;

		//�����������Y�ꂸ�ɁB
		delete [] feature_nodes;
		//���ݏ������Ă���t�@�C�������J��
		procfilename.clear();
	}
	fclose(fp);

	*outall = all;
	*outmatch = match;
	*outDcount = Dcount;
	return true;
}



//�w�K�f�[�^�̒ǉ� (liblinear ���ނ��I�����C���ł��Ȃ��炵������d���Ȃ����ǂ�)
void XLMachineLearningLibliear::AppendTrain(int classID,const std::vector<XLMachineLearningLibliear::feature>& newnodes)
{
	feature* nodes = new feature[newnodes.size()+1]; //�I�[������̂�+1���Ƃ��܂��B

	ProblemY.push_back(classID);
	ProblemX.push_back(nodes);

	for(std::vector<feature>::const_iterator it = newnodes.begin() ; it != newnodes.end() ; ++it )
	{
		*nodes++ = *it;
	}

	//�I�[�}�[�N -1 ������B
	nodes->index = -1;
	nodes->index = 0;
}

//�w�K���܂��B
bool XLMachineLearningLibliear::Train()
{
	int maxindex = 0;
	for(std::vector<feature*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
	{
		for(feature* nodes = *it; nodes->index != -1 ; ++nodes )
		{
			maxindex = max(nodes->index,maxindex);
		}
	}

	struct problem prob;
	prob.bias = -1;				//�o�C�A�X�H����������Ƃ��̈����ŗ^����炵�����E�E�E
	prob.l = ProblemY.size();	//�s��
	prob.n = maxindex;	//��Ԃł����f���ԍ�
	prob.y = &ProblemY[0];
	prob.x = (::feature_node**) ((feature*) &ProblemX[0]);

	struct parameter param;
	param.solver_type = L2R_L2LOSS_SVC_DUAL;
	param.C = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	{
		if(param.solver_type == L2R_LR || param.solver_type == L2R_L2LOSS_SVC)
			param.eps = 0.01;
		else if(param.solver_type == L2R_L2LOSS_SVC_DUAL || param.solver_type == L2R_L1LOSS_SVC_DUAL || param.solver_type == MCSVM_CS || param.solver_type == L2R_LR_DUAL)
			param.eps = 0.1;
		else if(param.solver_type == L1R_L2LOSS_SVC || param.solver_type == L1R_LR)
			param.eps = 0.01;
	}

	FreeModel();
	this->Model = train(&prob,&param);
	return this->Model != NULL;
}

//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningLibliear::Predict(const std::vector<feature>& params)
{
	if (params.size() <= 0)
	{
		return 0;
	}
	if ( (params.rbegin())->index == -1 )
	{
		return predict(this->Model , (::feature_node*) ((feature*) &params[0] ) );
	}
	else
	{
		std::vector<feature> params2 = params;
		feature fin;
		fin.index = -1;
		fin.value = 0;
		params2.push_back(fin);

		return predict(this->Model ,(::feature_node*) ((feature*) &params2[0] ) );
	}
}
//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningLibliear::Predict(const std::map<int,double>& params)
{
	if (params.empty())
	{
		return 0;
	}

	std::vector<feature> nodes;
	nodes.resize(params.size() + 1 );

	feature * p = &nodes[0];
	for(std::map<int,double>::const_iterator it = params.begin() ; it != params.end() ; ++it , ++p)
	{
		p->index = it->first;
		p->value = it->second;
	}
	p->index = -1;
	p->value = 0;

	return predict(this->Model , (::feature_node*) ((feature*) &nodes[0] ) );
}

//�w�K�������f������N���X�ԍ����擾���܂��B
int XLMachineLearningLibliear::Predict(const XLMachineLearningLibliear::feature* params)
{
	return predict(this->Model , (::feature_node*) params  );
}

bool XLMachineLearningLibliear::SaveModel(const std::string& filename)
{
	save_model(filename.c_str(),this->Model);
//		save_model(filename.c_str(),this->Model);
	return true;
}

bool XLMachineLearningLibliear::LoadModel(const std::string& filename)
{
	FreeModel();
	this->Model = load_model(filename.c_str());
	return true;
}
void XLMachineLearningLibliear::FreeModel()
{
	if (this->Model)
	{
		free_model_content(this->Model);
//			free_model_content(this->Model);
		this->Model = NULL;
	}
}

