#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <queue>
#include <cstring>

// ��ʼץȡ
bool StartCatch(const std::string& url);


class CHttp
{
public:
	CHttp();
	~CHttp();

	bool AnalyseUrl(const std::string& url)					// ���� URL
	{
		// �ַ�����Сдת����ȫ��ת��ΪСд
		// �� http/https://www.baidu.com/ ��Щ�ַ�ȫ��ת��Сд

		int loc = 0;
		if (std::string::npos != url.find("http://"))
		{
			loc = 7;
		}
		else if (std::string::npos != url.find("https://"))
		{
			loc = 8;
		}
		else
		{
			return false;
		}

		// double check �������С�� 7  ���ǲ������ҵ� http://
		if (url.length() <= 7)
		{
			return false;
		}

		// ��ȡ��ַ
		int pos = url.find('/', loc);
		if (pos == std::string::npos) //  http://www.baidu.com
		{
			host_ = url.substr(7);
			obj_ = "/";
		}
		else
		{
			host_ = url.substr(loc, pos - loc);
			obj_ = url.substr(pos + 1);
		}

		// �ж��Ƿ��ȡ������
		if (host_.empty())
		{
			return false;
		}

		std::cout << "Host: " << host_.c_str() << std::endl;
		std::cout << "Path: " << obj_.c_str() << std::endl;

		return true;
	}


	bool Connect()						// ������ҳ
	{
		return true;
	}


	bool FetchHtml(std::string& html)	// ��ȡ��ҳ
	{
		return true;
	}


	bool AnalyseHtml()					// ������ҳ
	{

		return true;
	}


	bool Download(std::string& url, std::string& filename)	// ������Դ
	{
		
		return true;
	}

private:
	std::string& ToLower(std::string& url)
	{
		// �ҵ� http/https://www.xxx.com/ ����ַ���
		int pos = url.find('/', 8);
		int sz = 0;

		if (pos == std::string::npos)
		{
			sz = url.size();
		}
		else
		{
			sz = pos;
		}

		for (int i = 0; i < sz; i++)
		{
			url[i] = tolower(url[i]);
		}

		return url;
	}

private:
	std::string host_; // ������
	std::string obj_;  // ��Դ·��
};