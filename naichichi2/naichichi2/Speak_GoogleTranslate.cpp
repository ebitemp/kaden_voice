﻿// Speak_GoogleTranslate.cpp: Speak_GoogleTranslate クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_GoogleTranslate.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "XLSoundPlay.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_GoogleTranslate::Speak_GoogleTranslate()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
}

Speak_GoogleTranslate::~Speak_GoogleTranslate()
{
	this->StopFlag = true;
	this->queue_wait.notify_all();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
xreturn::r<bool> Speak_GoogleTranslate::Create(MainWindow* poolMainWindow)
{
	assert(this->Thread == NULL);
	
	this->PoolMainWindow = poolMainWindow;
	this->StopFlag = false;
	this->Thread = new boost::thread([=](){
		try
		{
			this->Run(); 
		}
		catch(xreturn::error &e)
		{
			this->PoolMainWindow->SyncInvokeError( e.getErrorMessage() );
		}
	} );
	return true;
}

void Speak_GoogleTranslate::Run()
{
	_USE_WINDOWS_ENCODING;

	while(!this->StopFlag)
	{
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			if (this->SpeakQueue.size() <= 0)
			{
				this->queue_wait.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return;
		}

		//読み上げる文字列をキューから取得.
		SpeakTask task;
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			this->CancelFlag = false;

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}
		//ダウンロードの準備
		std::string lang = "ja";
		std::string encoding = "utf_8";	

		std::map<std::string , std::string> param;
//		param["q"]		= _A2U(task.text.c_str());
		param["q"]      = XLStringUtil::urlencode(_A2U(task.text.c_str()));
		param["ie"]		= "UTF-8";
		param["tl"]		= "ja";
		param["total"]	= "1";
		param["idx"]	= "0";
//		param["textlen"]	= "5";

		std::map<std::string,std::string> header;
		std::vector<char> retBinary;

		header["host"] = "translate.google.co.jp";
		header["connection"] = "close";
		header["accept-encoding"] = "identity;q=1, *;q=0";
		header["user-agent"] = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/536.11 (KHTML, like Gecko) Chrome/20.0.1132.57 Safari/536.11";
		header["accept"] = "*/*";
//		header["x-chrome-variations"] = "CL21yQEIiLbJAQibtskBCKO2yQEIqLbJAQi5g8oBCMaDygE=";
		header["referer"] = "http://translate.google.co.jp/";
		header["accept-language"] = "ja,en-US;q=0.8,en;q=0.6";
		header["accept-charset"] = "Shift_JIS,utf-8;q=0.7,*;q=0.3";
		header["range"] = "bytes=0-";

		XLHttpSocket::GetBinary("http://translate.google.com/translate_tts?" + XLStringUtil::crossjoin("=","&",param),header,60,&retBinary);
		if (this->StopFlag)
		{
			return;
		}

		//テンポラリに一度出力する.
		const char * tempfilename = tempnam(NULL,"naichichi_");
		if (tempfilename == NULL)
		{
			continue;
		}
		std::string filename = std::string(tempfilename) + ".mp3";
		XLFileUtil::write(filename,retBinary);

		free((void*)tempfilename);
		tempfilename = NULL;

		XLSoundPlay soundplay;
		soundplay.play(filename);

		unlink( filename.c_str() );
		if (this->StopFlag)
		{
			return;
		}

		if (this->CancelFlag)
		{

		}
		else
		{
			//コールバックする.
			this->PoolMainWindow->AsyncInvoke( [=](){
				this->PoolMainWindow->ScriptManager.SpeakEnd(task.callback,task.text);
			} );
		}
	}
}

xreturn::r<bool> Speak_GoogleTranslate::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
	//設定できません!!
	return true;
}

xreturn::r<bool> Speak_GoogleTranslate::Speak(const CallbackDataStruct * callback,const std::string & str)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str));
	this->queue_wait.notify_all();

	return true;
}


xreturn::r<bool> Speak_GoogleTranslate::Cancel()
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
	return true;
}

xreturn::r<bool> Speak_GoogleTranslate::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	return true;
}


