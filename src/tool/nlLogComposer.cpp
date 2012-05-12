#include "main/stdafx.h"
#include "ui_nlsandbox.h"
#include "nlLogComposer.hpp"

static Ui::nlSandboxClass* g_ui = NULL;
nlLogComposer* composer;

//class ListItem : public QListViewItem
//{
//public:
//    ListItem(QListView *lv, QString arg0, QString arg1) : QListViewItem(lv, arg0, arg1)
//    {
//    }
//    int compare(QListViewItem *i, int col, bool ascending) const
//    {
//        return key(col, ascending).compare(i->key(col, ascending));
//    }
//};

void logInit( Ui::nlSandboxClass* ui )
{
    g_ui = ui;
    composer = new nlLogComposer(NULL);
    /**/
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setDynamicSortFilter(true);
    g_ui->logList->setModel(proxyModel);
    /**/
    QObject::connect(g_ui->logFilter, SIGNAL(textChanged(const QString &)), composer, SLOT(filterRegExpChanged()));
}

void logInfo( const QByteArray& ba )
{
    //ListItem* newItem = new ListItem(g_ui->logList,ba,"");
    //g_ui->logList->insertItem ( newItem );
}

void logError( const QByteArray& ba )
{
    /*ListItem* newItem = new ListItem(g_ui->logList,ba,"");
    g_ui->logList->insertItem ( newItem );*/
}
/**/
nlLogComposer::nlLogComposer(QWidget*parent)
:QWidget(parent)
{
}
nlLogComposer::~nlLogComposer()
{
}
void nlLogComposer::filterRegExpChanged()
{
};