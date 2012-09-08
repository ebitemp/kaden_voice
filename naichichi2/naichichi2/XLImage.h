﻿// XLImage.h: XLImage クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLImage_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
#define AFX_XLImage_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <gdiplus.h>

class XLImage  
{
public:
	XLImage();
	XLImage(Gdiplus::Image* newImage);
	virtual ~XLImage();

	//画像読み込み
	bool Load(const std::string & fileName);
	//画像読み込み
	bool Load(const std::vector<char> & data);
	//画像読み込み
	bool Load(HBITMAP hbitmap);
	//画像読み込み
	bool Load(HICON hicon);

	//画像保存
	bool Save(const std::string & fileName,int option = INT_MAX) const;
	//画像保存
	bool Save(const std::string & ext,std::vector<char> * data,int option) const;

	//描画
	void Draw(XLImage* image,int x,int y) const;
	//描画
	void Draw(XLImage* image,int x,int y,int width,int height) const;
	//描画
	void Draw(HDC dc,int x,int y) const;
	//描画
	void Draw(HDC dc,int x,int y,int width,int height) const;
	//サムネイルを取得する
	Gdiplus::Image* GetThumbnailImage(int width,int height) ;
private:
	void Clear();
	bool findEncoder(const std::string & ext,CLSID* clsid) const;
	bool IsEnable() const
	{
		return this->image != NULL;
	}

	Gdiplus::Image* image;
};

#endif // !defined(AFX_XLImage_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
