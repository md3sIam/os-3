#include "ConsoleMenu.h"
#include <iostream>

template <typename T>
void ConsoleMenu<T>::addOption(const T name, const std::function<void()> f)
{
	m_options.emplace_back(name, f);
}

template <typename T>
void ConsoleMenu<T>::exec() const
{
	int32_t option = -1;
	do
	{
		system("cls");
		printMenu();
		std::cout << "Enter an option: ";
		std::cin >> option;

		if(option > 0 && option <= m_options.size())
		{
			system("cls");
			m_options[option - 1].second();
			system("pause");
		}
	}while(option);
}

template <typename T>
template <typename M>
void ConsoleMenu<T>::printCommand(int32_t i, const M& name) const
{
	if(std::is_same<M, std::string>::value)
		std::cout << "\t" << i << ") " << name.c_str() << std::endl;
	else if(std::is_same<M, std::wstring>::value)
		std::wcout << L"\t" << i << L") " << name.c_str() << std::endl;
}

template <typename T>
void ConsoleMenu<T>::printMenu() const
{
	std::cout << "Commands: " << std::endl;

	for(uint64_t i = 0; i < m_options.size(); ++i)
	{
		printCommand<T>(static_cast<int32_t>(i) + 1, m_options[i].first);
	}

	printCommand<std::string>(0, "Exit");
	std::cout << std::endl;
}

template class ConsoleMenu<std::string>;
template class ConsoleMenu<std::wstring>;
