#include "QOrientationHandler.hpp"

namespace isis
{

namespace viewer
{
namespace qt
{


util::fvector4 QOrienationHandler::mapCoordsToOrientation( const util::fvector4 &coords, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation, bool back, bool absolute )
{
	using namespace boost::numeric::ublas;
	matrix<float> transformMatrix = identity_matrix<float>( 4, 4 );
	matrix<float> finMatrix = identity_matrix<float>( 4, 4 );
	vector<float> vec = vector<float>( 4 );
	vector<float> finVec = vector<float>( 4 );

	for( size_t i = 0; i < 4 ; i++ ) {
		vec( i ) = coords[i];
	}

	switch ( orientation ) {
	case axial:
		/*setup axial matrix
		*-1  0  0
		* 0 -1  0
		* 0  0  1
		*/
		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 1;
		break;
	case sagittal:
		/*setup sagittal matrix
		* 0  1  0
		* 0  0  1
		* 1  0  0
		*/
		transformMatrix( 0, 0 ) = 0;
		transformMatrix( 2, 0 ) = 1;
		transformMatrix( 0, 1 ) = 1;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 1, 2 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		break;
	case coronal:
		/*setup coronal matrix
		* -1  0  0
		*  0  0  1
		*  0  1  0
		*/

		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 2, 1 ) = 1;
		transformMatrix( 1, 2 ) = -1;
		break;
	}

	if( back ) {
		finVec = prod( trans( prod(  transformMatrix, image->getNormalizedImageOrientation() ) ), vec );
	} else {
		finVec = prod( prod( transformMatrix, image->getNormalizedImageOrientation() ) , vec );
	}

	if( absolute ) {
		return util::fvector4( fabs( finVec( 0 ) ), fabs( finVec( 1 ) ), fabs( finVec( 2 ) ), fabs( finVec( 3 ) ) );
	} else {
		return util::fvector4( finVec( 0 ), finVec( 1 ), finVec( 2 ), finVec( 3 ) );
	}

}

util::ivector4 QOrienationHandler::getMappedCoords( const boost::shared_ptr< ImageHolder > image, const util::ivector4 &coords )
{
	using namespace boost::numeric::ublas;
	vector<int16_t> vec = vector<int16_t>( 4 );
	vector<int16_t> vec1 = vector<int16_t>( 4 );
	util::ivector4 retVec;

	for( size_t i = 0; i < 4; i++ ) {
		vec( i ) = coords[i];
	}

	vec1 = prod( image->getNormalizedImageOrientation(), vec );

	for ( size_t i = 0; i < 4; i++ ) {
		retVec[i] = abs( vec1( i ) );
	}

	return retVec;
}



ViewPortType QOrienationHandler::getViewPort(  util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation )
{
	ViewPortType viewPort;
	float zoom = properties.getPropertyAs<float>( "currentZoom" );
	util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	util::fvector4 mappedScaling = QOrienationHandler::mapCoordsToOrientation( image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ), image, orientation );
	mappedScaling += QOrienationHandler::mapCoordsToOrientation( image->getISISImage()->getPropertyAs<util::fvector4>( "voxelGap" ), image, orientation );
	util::fvector4 physSize = mappedScaling * mappedSize;

	float scalew = w / float( physSize[0] );
	float scaleh = h / float( physSize[1] );

	float scale = scaleh < scalew ? scaleh : scalew;

	viewPort[0] =  mappedScaling[0] * zoom * scale;
	viewPort[1] =  mappedScaling[1] * zoom * scale;

	float offsetX = ( w - ( mappedSize[0] * viewPort[0] ) ) / 2;
	float offsetY = ( h - ( mappedSize[1] * viewPort[1] ) ) / 2;
	viewPort[2] = offsetX ;
	viewPort[3] = offsetY ;
	viewPort[4] = round( mappedSize[0] * viewPort[0] );
	viewPort[5] = round( mappedSize[1] * viewPort[1] );
	return viewPort;
}


QTransform QOrienationHandler::getTransform( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation )
{
	util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	util::fvector4 flipVec = QOrienationHandler::mapCoordsToOrientation( util::fvector4( 1, 1, 1 ), image, orientation, false, false );
	util::ivector4 mappedVoxelCoords = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, orientation );
	QTransform retTransform;
	retTransform.setMatrix( flipVec[0], 0, 0,
							0, flipVec[1], 0,
							0, 0, 1 );

	//calculate crosshair dependent translation
	retTransform.translate( flipVec[0] * viewPort[2], flipVec[1] * viewPort[3] );
	retTransform.scale( viewPort[0], viewPort[1] );
	retTransform.translate(  flipVec[0] < 0 ? -mappedSize[0] : 0, flipVec[1] < 0 ? -mappedSize[1] : 0 );


	return retTransform;

}

util::ivector4 QOrienationHandler::convertWindow2VoxelCoords( const ViewPortType &viewPort, const util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, const size_t &x, const size_t &y, const size_t &slice, PlaneOrientation orientation )
{
	util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation, false, false );
	size_t voxCoordX = ( x - viewPort[2] ) / viewPort[0];
	size_t voxCoordY = ( y - viewPort[3] ) / viewPort[1];
	util::ivector4 coords =  util::ivector4( voxCoordX, voxCoordY, slice );

	for ( size_t i = 0; i < 2; i++ ) {
		coords[i] = mappedSize[i] < 0 ? abs( mappedSize[i] ) - coords[i] - 1 : coords[i];
	}

	return QOrienationHandler::mapCoordsToOrientation( coords, image, orientation, true );
}

std::pair< size_t, size_t > QOrienationHandler::convertVoxel2WindowCoords( const ViewPortType &viewPort, const isis::util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation )
{
	util::ivector4 mappedVoxelCoords = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, orientation );
	float halfVoxelX = viewPort[0] / 2;
	float halfVoxelY = viewPort[1] / 2;

	float x = mappedVoxelCoords[0] * viewPort[0] + viewPort[2] + halfVoxelX;
	float y = mappedVoxelCoords[1] * viewPort[1] + viewPort[3] + halfVoxelY;
	return std::make_pair<size_t, size_t>( round( x ), round( y ) );
}

}
}
} // end namespace