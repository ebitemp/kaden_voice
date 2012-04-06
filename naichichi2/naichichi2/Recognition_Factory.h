﻿#pragma once


struct ISpeechRecognitionInterface
{
	ISpeechRecognitionInterface(){}
	virtual ~ISpeechRecognitionInterface(){}

	virtual xreturn::r<bool> Create(MainWindow* poolMainWindow) = 0;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual xreturn::r<bool> SetYobikake(const std::list<std::string> & yobikakeList) = 0;
	virtual xreturn::r<bool> SetCancel(const std::list<std::string> & cancelList) = 0;
	//認識結果で不感染なものを捨てる基準値を設定します。
	virtual xreturn::r<bool> SetRecognitionFilter(double temporaryRuleConfidenceFilter) = 0;
	//コマンドに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddCommandRegexp(const CallbackDataStruct * callback,const std::string & str) = 0;
	//テンポラリルールに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddTemporaryRegexp(const CallbackDataStruct * callback,const std::string & str) = 0;
	//テンポラリルールをすべてクリアします
	virtual xreturn::r<bool> ClearTemporary() = 0;
	//構築したルールを音声認識エンジンにコミットします。
	virtual xreturn::r<bool> CommitRule() = 0;
	//このコールバックに関連付けられているものをすべて消す
	virtual xreturn::r<bool> RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) = 0;
	//メディア情報をアップデートします。
	virtual xreturn::r<bool> UpdateMedia(const std::string& name ,const std::list<std::string>& list ) = 0;

};

class Recognition_Factory
{
public:
	Recognition_Factory();
	virtual ~Recognition_Factory();

	xreturn::r<bool> Recognition_Factory::Create(const std::string & name , MainWindow* poolMainWindow,std::list<std::string> yobikakeListArray,std::list<std::string> cancelListArray,double temporaryRuleConfidenceFilter);
	xreturn::r<bool> Recognition_Factory::AddCommandRegexp(const CallbackDataStruct * callback ,const std::string p1);
	xreturn::r<bool> Recognition_Factory::AddTemporaryRegexp(const CallbackDataStruct * callback ,const std::string p1);
	xreturn::r<bool> Recognition_Factory::CommitRule();
	xreturn::r<bool> Recognition_Factory::ClearTemporary();
	xreturn::r<bool> Recognition_Factory::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback);
	xreturn::r<bool> Recognition_Factory::UpdateMedia(const std::string& name ,const std::list<std::string>& list );

private:
	ISpeechRecognitionInterface* Engine;
};
