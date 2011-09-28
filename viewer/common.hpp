/*
 * common.hpp
 *
 *  Created on: Nov 2, 2010
 *      Author: tuerke
 */

#ifndef VIEWER_COMMON_HPP_
#define VIEWER_COMMON_HPP_

#include <vector>
#include <DataStorage/chunk.hpp>
#include <DataStorage/image.hpp>
#include <boost/shared_ptr.hpp>
#include <DataStorage/common.hpp>
#include <CoreUtils/common.hpp>

namespace isis
{

struct ViewerLog {static const char *name() {return "Viewer";}; enum {use = _ENABLE_LOG};};
struct ViewerDebug {static const char *name() {return "ViewerDebug";}; enum {use = _ENABLE_DEBUG};};

namespace viewer
{

typedef uint8_t InternalImageType;

// just some helper typedefs which we will need regularly
enum PlaneOrientation { axial, sagittal, coronal };
enum WidgetType { type_gl, type_qt };
enum InterpolationType { nn = 0, lin };
enum ScalingType { no_scaling, automatic_scaling, manual_scaling };

template<typename TYPE>
TYPE roundNumber( TYPE number, unsigned  short placesOfDec )
{
	return floor( number * pow( 10., placesOfDec ) + .5 ) / pow( 10., placesOfDec );
}

void setOrientationToIdentity( data::Image &image );
std::string getFileFormatsAsString( const std::string preSeparator, const std::string postSeparator = std::string( " " ) );

util::ivector4 get32BitAlignedSize( const util::ivector4 &origSize );

typedef ViewerLog Runtime;
typedef ViewerDebug Debug;
template<typename HANDLE> void enable_log( LogLevel level )
{
	ENABLE_LOG( Runtime, HANDLE, level );
	ENABLE_LOG( Debug, HANDLE, level );
}

}
}

#endif /* VIEWER_COMMON_HPP_ */
