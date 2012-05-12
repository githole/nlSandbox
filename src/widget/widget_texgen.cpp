#include "main/stdafx.h"
#include "widget_texgen.h"
#include "64k_base.h"

using namespace std;

widget_texgen::widget_texgen( QWidget *parent )
	:QWidget( parent )
{
	texImg_ = std::auto_ptr<QImage>( new QImage( 256,256, QImage::Format_ARGB32 ) );	

	setTG("test");
}
/**/
widget_texgen::~widget_texgen()
{
}
/**/
void widget_texgen::setTG( std::string fileName )
{
	/* HACK XML�t�@�C���ǂݍ��݂̃e�X�g */
	nlTgRawData tg;
	nlLoadTG( MEDIA_PATH"test.xml", tg );
	nlTGData64 shortTgData;
	nlShortnateTGFormat( shortTgData, tg );
	/* �e�N�X�`���� */
	/* pixmap���쐬���A�������� */
	assert( texImg_->height() == texImg_->width() );
	const int size  = texImg_->width();
	const int SURF_NUM = tg.size();
	nlSurface surfs[SURF_NUM] = {};
	for( int i=0;i<SURF_NUM;++i)
	{
		surfs[i].buffer_ = (nlColor*)nlMem_alloc( sizeof(nlColor)*size*size );
	}
	nlTexgenerateFromtTG( surfs, SURF_NUM, shortTgData );

	/* 0�Ԃ̃T�[�t�F�C�X����������B�Ō�Ƀs�N�Z����]������ */
	for(int y=0;y<size;++y)
	{
		for(int x=0;x<size;++x)
		{
			nlColor color;
			color = (surfs[3].buffer_[x+y*size]);
			color.a_ = 0xff;
			texImg_->setPixel ( x, y, color.color_ );
		}
	}
}


void widget_texgen::paintEvent( QPaintEvent* paintEvent )
{
	QPainter painter;
	painter.begin(this);
	if (!texImg_->isNull()) 
	{ 
		QImage  scaled_image = texImg_->scaled( 256, 256, Qt::IgnoreAspectRatio, Qt::FastTransformation);
		QPixmap pixmap = QPixmap::fromImage(scaled_image, Qt::AutoColor);
		painter.drawPixmap(0, 0,  pixmap ); 
	}
	painter.end();
}