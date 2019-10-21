#include "Task11.h"
#include "ConsoleMenu.h"
#include <functional>
#include <iostream>

int main(int argc, char** argv)
{
	std::function<void()> f{[]() -> void { std::cout << "FUNC\n"; }};
	ConsoleMenu<std::string> menu;
	menu.addOption("Disk list", Task11::diskList);
	menu.addOption("Disk info", Task11::diskInfo);
	menu.addOption("Create/remove dirs", Task11::createRemoveDir);
	menu.addOption("Create file", Task11::createFile);
	menu.addOption("Copy/move file", Task11::copyMoveFile);
	menu.addOption("Analyze/change file attribute", Task11::changeFileAttr);

	menu.exec();
	return 0;
}