// Include C++ Library
#include<stdio.h>

// Include QT Library
#include <QtGui>

// Include Other Library
#include "headers.h"

// Include Widget
#include "window.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	printf("Iniciando...\n");
	a.setKeyboardInputInterval(1);
	Window w(NULL);
	
	w.show();
	w.setMouseTracking(true); // Detectar MouseMove
	w.setVisible(true);
	return a.exec();
}
