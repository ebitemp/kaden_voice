﻿#include "common.h"
//#include <io.h>
#include "ScriptManager.h"
#include "MainWindow.h"

//作成
bool ScriptManager::Create(MainWindow * poolMainWindow)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow = poolMainWindow;
	this->PoolMainWindow->SyncInvokeLog("ScriptManager初期化開始",LOG_LEVEL_DEBUG);

	this->loadLua(this->PoolMainWindow->Config.GetBaseDirectory() );
	this->RunAllLua();

	this->WebScript.Create(this->PoolMainWindow);
	this->WebScript.Load();

	this->PoolMainWindow->SyncInvokeLog("音声認識エンジンコミット開始",LOG_LEVEL_DEBUG);
	this->PoolMainWindow->Recognition.CommitRule();
	this->PoolMainWindow->SyncInvokeLog("音声認識エンジンコミット終了",LOG_LEVEL_DEBUG);

	this->PoolMainWindow->SyncInvokeLog("ScriptManager初期化完了",LOG_LEVEL_DEBUG);
	return true;
}

ScriptManager::~ScriptManager()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->destoryLua();
}

#include "XLFileUtil.h"
#include "XLStringUtil.h"

//luaファイル郡の読み込み
bool ScriptManager::loadLua(const std::string & baseDirectory)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->Scripts.clear();
	bool ret = XLFileUtil::findfile(baseDirectory , [&](const std::string& filename,const std::string& fullfilename) -> bool {
		//scenario_*.lua を探索
		if ( strstr(filename.c_str() , "scenario_") == NULL || XLStringUtil::baseext_nodot(filename) != "lua" )
		{
			return true;
		}

		ScriptRunner* runner = new ScriptRunner(this->PoolMainWindow , true );

		try
		{
			runner->LoadScript(fullfilename);
		}
		catch(XLException &e)
		{
			this->PoolMainWindow->SyncInvokeError( e.getFullErrorMessage() );
			return true;
		}

		this->Scripts.push_back(runner);
		return true;
	});
	if (!ret)
	{
		throw XLException("findfirstに失敗 ディレクトリ:" + baseDirectory + " を検索できません。",-1);
	}

	return true;
}


//luaの実行
void ScriptManager::RunAllLua()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto it = this->Scripts.begin();
	for( ; it != this->Scripts.end() ; ++it )
	{
		try
		{
			(*it)->callFunction("call");
		}
		catch(XLException &e)
		{
			this->PoolMainWindow->SyncInvokeError( e.getFullErrorMessage() );
			continue;
		}
	}
}

//luaの終了
void ScriptManager::destoryLua()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto it = this->Scripts.begin();
	for( ; it != this->Scripts.end() ; ++it )
	{
		delete *it;
	}
	this->Scripts.clear();
}


//音声認識に失敗した時の結果
void ScriptManager::BadVoiceRecogntion(int errorCode,const std::string& matString,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictationCheck)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "音声認識失敗しました。コード:" + num2str(errorCode) + " 認識:" + " yobikake:" + num2str(yobikakeRuleConfidenceFilter) + " basic:" + num2str(basicRuleConfidenceFilter) + " diccheck:" + num2str((int)dictationCheck) + " dic:" + diction + " failmatch:" + matString ,LOG_LEVEL_DEBUG);
}

//音声認識した結果
void ScriptManager::VoiceRecogntion(const CallbackDataStruct* callback,const std::map< std::string , std::string >& capture,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "音声認識完了しました。認識:" + capture.begin()->second + " diction:" + diction + " yobikake:" + num2str(yobikakeRuleConfidenceFilter) + " basic:" + num2str(basicRuleConfidenceFilter),LOG_LEVEL_DEBUG);
	this->PoolMainWindow->Recognition.ClearTemporary();
	this->fireCallback(callback,capture);
	this->PoolMainWindow->Recognition.CommitRule();
}

//喋り終わった時
void ScriptManager::SpeakEnd(const CallbackDataStruct* callback,const std::string& text)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog(std::string() + "spack完了しました。" + text + "コールバックを打ち返します。",LOG_LEVEL_DEBUG);

	std::map<std::string,std::string > dummy;
	this->fireCallback(callback,dummy);
}

//汎用的なコールバック打ち返し
std::string ScriptManager::fireCallback(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args) const
{
	std::string ret;
	if (callback == NULL)
	{
		return ret;
	}

	int func = callback->getFunc();
	if (func == NO_CALLBACK)
	{
		return ret;
	}
	return callback->getRunner()->callbackFunction(callback,args);
}

//家電制御が終わった時
void ScriptManager::ActionEnd(const CallbackDataStruct* callback,const std::map< std::string , std::string >& data)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog("action完了しました。コールバックを打ち返します。",LOG_LEVEL_DEBUG);

	this->fireCallback(callback,data);
	this->UnrefCallback(callback);
}

//トリガーが呼ばれたとき
void ScriptManager::TriggerCall(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args,std::string * respons)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	*respons = this->fireCallback(callback,args);

	//トリガーは何度も打てるので、 UnrefCallback はしません。
}

//ウェブメニューからの実行
void ScriptManager::WebMenuCall(const CallbackDataStruct* callback)
{
	std::map< std::string , std::string > args;
	this->fireCallback(callback,args);

	//ウェブメニューは何度も打てるので、 UnrefCallback はしません。
}

//コールバックを消す通知をします。
void ScriptManager::UnrefCallback(const CallbackDataStruct* callback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	callback->getRunner()->unrefCallback(callback);
}

//webからアクセスがあった時
bool ScriptManager::WebAccess(const std::string& path,const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString)
{
	return this->WebScript.WebAccess(path,httpHeaders,result,responsString);
}
