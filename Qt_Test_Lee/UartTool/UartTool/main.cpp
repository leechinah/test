#include "UartTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	UartTool w;
	w.show();
	return a.exec();
}
