#include "main/stdafx.h"
#include "log.h"
#include "ui_nlModel.h"

extern Ui_MainWindow* g_ui;

/**/
void log( QString str )
{
    std::string stdStr = str.toStdString();
    log(stdStr.c_str());
}
/**/
void log( std::string str )
{
    log(str.c_str());
}
/**/
void log( const char* str )
{
    puts(str);
}
