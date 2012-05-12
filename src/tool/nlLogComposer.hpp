#pragma once

#include "main/stdafx.h"

namespace Ui
{
    class nlSandboxClass;
};

void logInit( Ui::nlSandboxClass* ui );
void logInfo( const QByteArray& ba );
void logError( const QByteArray& ba );


class nlLogComposer
    :public QWidget
{
    Q_OBJECT
public:
    nlLogComposer(QWidget*);
    ~nlLogComposer();
public slots:
    void filterRegExpChanged();
};