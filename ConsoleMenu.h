#ifndef INC_3_CONSOLEMENU_H
#define INC_3_CONSOLEMENU_H

#include <vector>
#include <functional>

template <typename T>
class ConsoleMenu
{
public:
	ConsoleMenu() = default;

	void addOption(const T name, const std::function<void()>);

	void exec() const;

private:
	template <typename M> void printCommand(int32_t i, const M& name) const;
	void printMenu() const;

	using vector = std::vector<std::pair<T, std::function<void()>>>;
	vector m_options;
};

#endif //INC_3_CONSOLEMENU_H
