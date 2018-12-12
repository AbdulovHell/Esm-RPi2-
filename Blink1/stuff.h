#ifndef _STUFF_H_
#define _STUFF_H_

namespace Stuff {
	
	using namespace std;

	//проверка контрольной суммы
	bool Verify(char* buf);
	//подсчет контрольной суммы
	void CalcSum(char* buf, size_t size);
	//генерация рандомных чисел
	int RangeRand(int min, int max);

	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}
#endif	// _STUFF_H_
