#include "mainwindow.hpp"
#include <iostream>
#include <QGridLayout>
#include "DataStorage/io_factory.hpp"


namespace isis
{
namespace viewer
{

MainWindow::MainWindow( QViewerCore *core ) :
	m_Core( core )
{
	m_UI.setupUi( this );
	setupBasicElements();

	connect( m_UI.action_Open_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );

}

void MainWindow::openImage()
{
	ImageHolder::ImageType type = ImageHolder::anatomical_image;
	std::string title( "Open Image" );
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString( std::string( "*." ) ) << ")";
	QStringList filenames = QFileDialog::getOpenFileNames( this,
							tr( title.c_str() ),
							m_Core->getCurrentPath().c_str(),
							tr( fileFormats.str().c_str() ) );


	if( !filenames.empty() ) {
		QDir dir;
		m_Core->setCurrentPath( dir.absoluteFilePath( filenames.front() ).toStdString() );
		bool isFirstImage = m_Core->getDataContainer().size() == 0;
		std::list<data::Image> imgList;
		util::slist pathList;
		BOOST_FOREACH( QStringList::const_reference filename, filenames ) {
			std::list<data::Image> tempImgList = isis::data::IOFactory::load( filename.toStdString() , "", "" );
			pathList.push_back( filename.toStdString() );
			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
			}
		}

		m_Core->updateScene( isFirstImage );
	}
}




void MainWindow::setupBasicElements()
{
	//here we setup the basic elements of the viewer


}

void MainWindow::reloadPluginsToGUI()
{
	//adding all processes to the process (plugin) menu and connect the action to the respective call functions
	QMenu *processMenu = new QMenu( QString( "Plugins" ) );

	if( m_Core->getPlugins().size() ) {
		getUI().menubar->addMenu( processMenu );

		QSignalMapper *signalMapper = new QSignalMapper( this );
		BOOST_FOREACH( ViewerCoreBase::PluginListType::const_reference plugin, m_Core->getPlugins() ) {
			std::list<std::string> sepName = isis::util::stringToList<std::string>( plugin->getName(), boost::regex( "/" ) );
			QMenu *tmpMenu = processMenu;
			std::list<std::string>::iterator iter = sepName.begin();

			for ( unsigned short i = 0; i < sepName.size() - 1; iter++, i++ ) {
				tmpMenu = tmpMenu->addMenu( iter->c_str() );

			}

			QAction *processAction = new QAction( QString( ( --sepName.end() )->c_str() ), this );

#warning add plugin icon to toolbar
			//          //optionally add plugin to the toolbar
			//          if( !plugin->getToolbarIcon()->isNull() ) {
			//              processAction->setIcon( *plugin->getToolbarIcon() );
			//              m_Toolbar->addAction( processAction );
			//          }

			processAction->setStatusTip( QString( plugin->getTooltip().c_str() ) );
			signalMapper->setMapping( processAction, QString( plugin->getName().c_str() ) );
			tmpMenu->addAction( processAction );
			connect( processAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

		}
		connect( signalMapper, SIGNAL( mapped( QString ) ), m_Core, SLOT( callPlugin( QString ) ) );
	}
}


}
}