#ifndef VIEWERCOREBASE_HPP
#define VIEWERCOREBASE_HPP

#include "DataContainer.hpp"
#include "ImageOps.hpp"
#include <map>
#include "PluginLoader.hpp"

namespace isis
{
namespace viewer
{

class ViewerCoreBase
{
	struct OptionStruct {
		bool propagateZooming;
	};
public:

	typedef std::list<boost::shared_ptr< plugin::PluginInterface > > PluginListType;
	ViewerCoreBase( );

	std::string getVersion() const;

	virtual std::list<boost::shared_ptr<ImageHolder> > addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	void setCurrentImage( const boost::shared_ptr<ImageHolder> image ) { m_CurrentImage = image; }

	boost::shared_ptr<ImageHolder> getCurrentImage() { return m_CurrentImage; }
	size_t getCurrentTimestep() const { return m_CurrentTimestep; }
	bool setCurrentTimestep( size_t timestep ) {
		if(  m_CurrentImage->getImageSize()[3] > timestep ) {
			m_CurrentTimestep = timestep;
			return true;
		} else { return false; }
	}
	
	const DataContainer &getDataContainer() const { return m_DataContainer; }
	DataContainer &getDataContainer() { return m_DataContainer; }

	const OptionStruct *getOption() const { return m_Options; }
	OptionStruct *getOption() { return m_Options; }

	void setCoordsTransformation( const util::fvector4 &transformation ) { m_VoxelTransformation = transformation; }
	util::fvector4 getTransformedCoords( const util::fvector4 &coords ) const;
	
	//plugin stuff
	
	void addPlugin( boost::shared_ptr< plugin::PluginInterface > plugin );
	void addPlugins( plugin::PluginLoader::PluginListType plugins );
	PluginListType getPlugins() const { return m_PluginList; }
	
	bool callPlugin( const std::string &name );

private:
	//this is the container which actually holds all the images
	DataContainer m_DataContainer;
	boost::shared_ptr<ImageHolder>  m_CurrentImage;
	size_t m_CurrentTimestep;
	util::fvector4 m_VoxelTransformation;

protected:
	OptionStruct *m_Options;
	PluginListType m_PluginList;


};




}
} // end namespace



#endif