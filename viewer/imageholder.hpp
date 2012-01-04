/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik T�rke, tuerke@cbs.mpg.de
 *
 * imageholder.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include "common.hpp"
#include "color.hpp"
#include "widgetinterface.hpp"

namespace isis
{
namespace viewer
{

namespace color
{
class Color;
}
class WidgetInterface;
/**
 * Class that holds one image in a vector of data::ValuePtr's
 * It ensures the data is hold in continuous memory and only consists of one type.
 * Furthermore this class handles the meta information of the image
 */

class ImageHolder
{


public:
	typedef std::list<boost::shared_ptr< ImageHolder > > ImageListType;
	typedef data::_internal::ValuePtrBase::Reference ImagePointerType;

	enum ImageType { structural_image, z_map };

	ImageHolder();

	bool setImage( const data::Image &image, const ImageType &imageType, const std::string &filename = "" );

	size_t getID() const { return m_ID; }
	void setID( size_t id ) { m_ID = id; }

	std::vector< ImagePointerType > getImageVector() const { return m_ImageVector; }
	const std::vector< data::Chunk > &getChunkVector() const { return m_ChunkVector; }
	std::vector< data::Chunk > &getChunkVector() { return m_ChunkVector; }
	util::PropertyMap &getPropMap() { return m_PropMap; }
	const util::PropertyMap &getPropMap() const { return m_PropMap; }
	const util::FixedVector<size_t, 4> &getImageSize() const { return m_ImageSize; }
	boost::shared_ptr< data::Image >getISISImage() const { return m_Image; }
	boost::numeric::ublas::matrix<double> getNormalizedImageOrientation( bool transposed = false ) const;
	boost::numeric::ublas::matrix<double> getImageOrientation( bool transposed = false ) const;
	void addChangedAttribute( const std::string &attribute );
	bool removeChangedAttribute( const std::string &attribute );

	boost::weak_ptr<void>
	getImageWeakPointer( size_t timestep = 0 ) const {
		return getImageVector()[timestep]->getRawAddress();
	}

	util::slist getFileNames() const { return m_Filenames; }

	bool operator<( const ImageHolder &ref ) const { return m_ID < ref.getID(); }

	void checkVoxelCoords( util::ivector4 &voxelCoords );

	void updateColorMap();

	void addWidget( WidgetInterface *widget ) { m_WidgetList.push_back( widget ); }
	void removeWidget( WidgetInterface *widget );
	std::list< WidgetInterface * > getWidgetList() { return m_WidgetList; }

	void updateOrientation();

	void syncImage();

	/**offset, scaling**/
	std::pair<double, double> getOptimalScaling();

	util::ivector4 voxelCoords;
	util::fvector4 physicalCoords;
	util::fvector4 voxelSize;
	bool isVisible;
	bool isRGB;
	float opacity;
	util::ivector4 alignedSize32;
	double offset;
	double scaling;
	double extent;
	double lowerThreshold;
	double upperThreshold;
	QVector<QRgb> colorMap;
	std::string lut;
	ImageType imageType;
	InterpolationType interpolationType;
	std::pair<util::ValueReference, util::ValueReference> minMax;
	std::pair<util::ValueReference, util::ValueReference> internMinMax;
	std::pair<double, double> optimalScalingOffset;
	boost::numeric::ublas::matrix<double> orientation;
	boost::numeric::ublas::matrix<double> latchedOrientation;
	unsigned short majorTypeID;
	std::vector< double *> histogramVector;
	std::pair<util::ValueReference, util::ValueReference> scalingToInternalType;

private:

	util::FixedVector<size_t, 4> m_ImageSize;
	util::PropertyMap m_PropMap;

	boost::shared_ptr<data::Image> m_Image;
	util::slist m_Filenames;
	size_t m_ID;
	std::pair<double, double> m_OptimalScalingPair;

	std::vector< ImagePointerType > m_ImageVector;
	std::vector< data::Chunk > m_ChunkVector;

	std::list<WidgetInterface *> m_WidgetList;

	boost::shared_ptr<color::Color> m_ColorHandler;

	template<typename TYPE>
	void copyImageToVector( const data::Image &image ) {
		data::ValuePtr<TYPE> imagePtr( ( TYPE * ) calloc( image.getVolume(), sizeof( TYPE ) ), image.getVolume() );
		LOG( Debug, verbose_info ) << "Needed memory: " << image.getVolume() * sizeof( TYPE ) / ( 1024.0 * 1024.0 ) << " mb.";
		data::TypedImage<TYPE> typedefImage ( image );
		static_cast<data::Image &>( typedefImage ).copyToMem<TYPE>( &imagePtr[0], image.getVolume() );
		LOG( Debug, verbose_info ) << "Copied image to continuous memory space.";
		internMinMax = imagePtr.getMinMax();

		//splice the image in its volumes -> we get a vector of t volumes
		if( m_ImageSize[3] > 1 ) { //splicing is only necessary if we got more than 1 timestep
			m_ImageVector = imagePtr.splice( m_ImageSize[0] * m_ImageSize[1] * m_ImageSize[2] );
		} else {
			m_ImageVector.push_back( imagePtr );
		}
	}

	template<typename TYPE>
	void _syncImage() {
		#pragma omp parallel for

		for( size_t t = 0; t < getImageSize()[3]; t++ ) {
			for( size_t z = 0; z < getImageSize()[2]; z++ ) {
				for( size_t y = 0; y < getImageSize()[1]; y++ ) {
					for( size_t x = 0; x < getImageSize()[0]; x++ ) {
						getISISImage()->voxel<TYPE>( x, y, z, t ) = util::Value<InternalImageType> ( getChunkVector()[t].voxel<InternalImageType>( x, y, z ) ).as<TYPE>();
					}
				}
			}
		}
	}

};

}
} //end namespace

#endif