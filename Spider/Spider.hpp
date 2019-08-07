#define _CRT_SECURE_NO_WARNINGS
#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <Windows.h>
#include <queue>
#include <cstring>
#include <regex>
#include <cstdio>

class CHttp
{
public:
	CHttp()
	{}

	~CHttp()
	{}

	bool Init()
	{
		WSADATA wd;
		if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		{
			return false;
		}

		if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
		{
			return false;
		}

		socket_ = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_ == INVALID_SOCKET)
		{
			return false;
		}

		return true;
	}

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
			obj_ = url.substr(pos);
		}

		// �ж��Ƿ��ȡ������
		if (host_.empty())
		{
			return false;
		}

		// TODO
		std::cout << "Host: " << host_.c_str() << std::endl;
		std::cout << "Path: " << obj_.c_str() << std::endl;

		return true;
	}


	bool Connect()						// ���ӷ�����
	{
		Init();

		// ������ip��ַ��ת��
		hostent* p = gethostbyname(host_.c_str());
		if (NULL == p)
		{
			return false;
		}


		// ���ӷ�����
		sockaddr_in sa;
		memcpy(&sa.sin_addr, p->h_addr, 4);
		sa.sin_family = AF_INET;
		sa.sin_port = htons(80);

		if (SOCKET_ERROR == connect(socket_, (sockaddr*)& sa, sizeof(sa)))
		{
			return false;
		}

		return true;
	}


	bool FetchHtml(std::string& html)	// ��ȡ��ҳ
	{
		// ������������ GET ����
		std::string info;
		info += "GET " + obj_ + " HTTP/1.1\r\n";
		info += "Host: " + host_ + "\r\n";
		info += "Connection: Close\r\n\r\n";

		// ���͸�������
		if (SOCKET_ERROR == send(socket_, info.c_str(), info.size(), 0))
		{
			return false;
		}

		char ch = 0;
		while (recv(socket_, &ch, sizeof(ch), 0) > 0)
		{
			html += ch;
		}

		return true;
	}


	bool AnalyseHtml(const std::string& html, std::vector<std::string>& vecImage, std::queue<std::string>& q)					// ������ҳ
	{
		std::smatch mat;
		// ������ʽ
		std::regex rex("http://[^\\s'\"<>()]+");

		std::string::const_iterator start = html.begin();
		std::string::const_iterator end = html.end();

		while (regex_search(start, end, mat, rex))
		{
			std::string per(mat[0].first, mat[0].second);

			if (per.find(".jpg") != std::string::npos ||
				per.find(".png") != std::string::npos ||
				per.find(".jpeg") != std::string::npos)
			{
				vecImage.push_back(per);
			}
			else
			{
				// ȥ�� w3c
				if (per.find("http://www.w3.org/") == std::string::npos)
				{
					q.push(per);
				}
			}

			start = mat[0].second;
		}

		return true;
	}


	bool Download(std::string& url, std::string& filename)	// ������Դ
	{
		if (!AnalyseUrl(url))
		{
			return false;
		}
		if (!Connect())
		{
			return false;
		}

		// ������������ GET ����
		std::string info;
		info += "GET " + obj_ + " HTTP/1.1\r\n";
		info += "Host: " + host_ + "\r\n";
		info += "Connection: Close\r\n\r\n";

		// ���͸�������
		if (SOCKET_ERROR == send(socket_, info.c_str(), info.size(), 0))
		{
			return false;
		}

		// ���ļ�
		FILE* fp = fopen(filename.c_str(), "wb");
		if (fp == nullptr)
		{
			return false;
		}

		// ����ͷ��Ϣ
		char ch = 0;
		while (recv(socket_, &ch, 1, 0))
		{
			// �����������б�ʾ�������� \r\n\r\n
			if (ch == '\r')
			{
				recv(socket_, &ch, 1, 0);
				if (ch == '\n')
				{
					recv(socket_, &ch, 1, 0);
					if (ch == '\r')
					{
						recv(socket_, &ch, 1, 0);
						if (ch == '\n')
						{
							break;
						}
					}
				}
			}
		}
		// ��������
		while (recv(socket_, &ch, sizeof(ch), 0) > 0)
		{
			fwrite(&ch, 1, 1, fp);
		}

		// �ر��ļ�
		fclose(fp);

		return true;
	}

	bool Close()
	{
		closesocket(socket_);
		host_.clear();
		obj_.clear();
		socket_ = 0;

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
	SOCKET socket_;	   // �ͻ����׽���
};


// ��ʼץȡ
bool StartCatch(const std::string& url)
{
	// URL ����
	std::queue<std::string> q;
	q.push(url);
	CHttp http;

	while (!q.empty())
	{
		// ץȡ
		// 1. ȡ����ǰ���׵� URL
		std::string c_url = q.front();
		q.pop();

		// 2. ���� URL
		http.AnalyseUrl(c_url);

		// 3. ���ӷ�����
		if (http.Connect())
		{
			std::cout << "���ӳɹ���" << std::endl;
		}
		else
		{
			std::cout << "����ʧ�ܣ�" << std::endl;
			return false;
		}

		// 4. ��ȡ��ҳ
		std::string html;
		http.FetchHtml(html);

		// ���ͼƬ����
		std::vector<std::string> vecImage;

		// 5. ������ҳ����
		http.AnalyseHtml(html, vecImage, q);
		http.Close();

		// 6. ����ͼƬ
		CHttp download;
		for (int i = 0; i < vecImage.size(); i++)
		{
			std::string filename = "D:/ѧϰ/��Ŀ/Spider/Spider/images/" + vecImage[i].substr(vecImage[i].find_last_of('/') + 1);

			if (!download.Download(vecImage[i], filename))
			{
				std::cout << "����ʧ�ܣ�" << std::endl;
			}
			else
			{
				std::cout << "���سɹ���" << std::endl;
			}
		}
	}


	return true;
}