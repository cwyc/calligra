#include "kimageshop_factory.h"
#include "kimageshop_doc.h"

#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>

#include <kimgio.h>

extern "C"
{
    void* init_libkimageshop()
    {
	return new KImageShopFactory;
    }
};

KInstance* KImageShopFactory::s_global = 0;

KImageShopFactory::KImageShopFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
  KImageIO::registerFormats();

  s_global = new KInstance( "kimageshop" );
  
  s_global->dirs()->addResourceType("kis",
				    KStandardDirs::kde_default("data") + "kimageshop/");
  s_global->dirs()->addResourceType("kis_images",
				    KStandardDirs::kde_default("data") + "kimageshop/images/");
  s_global->dirs()->addResourceType("kis_brushes",
				    KStandardDirs::kde_default("data") + "kimageshop/brushes/");
  s_global->dirs()->addResourceType("toolbar",
				    KStandardDirs::kde_default("data") + "koffice/toolbar/");
  s_global->dirs()->addResourceType("kis_pics",
				    KStandardDirs::kde_default("data") + "kimageshop/pics/");
}

KImageShopFactory::~KImageShopFactory()
{
}

QObject* KImageShopFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KImageShopFactory: parent does not inherit KoDocument");
	return 0;
    }

    return new KImageShopDoc( 510, 510, (KoDocument*)parent, name );
}

KInstance* KImageShopFactory::global()
{
    return s_global;
}

#include "kimageshop_factory.moc"
