#include "Spider.hpp"

void Welcome()
{
	std::cout << "********************************" << std::endl;
	std::cout << "*  ��ӭʹ�� Gerald's �������  *" << std::endl;
	std::cout << "********************************" << std::endl;
	std::cout << "��������Ҫץȡ�� URL��";

}

int main(void)
{
	Welcome();
	
	std::string url;
	std::cin >> url;

	// �����ļ���
	CreateDirectory("./images", NULL);

	// ��ʼץȡ
	StartCatch(url);


	return 0;
}

