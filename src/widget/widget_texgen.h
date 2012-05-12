#pragma once

#include "stdafx.h"

class widget_texgen : public QWidget
{
	Q_OBJECT
public:
	widget_texgen(QWidget *parent = 0);
	~widget_texgen();
	void setTG( std::string fileName );
private:
	private slots:
		void paintEvent(QPaintEvent* paintEvent);
private:
	std::auto_ptr<QImage> texImg_;
};