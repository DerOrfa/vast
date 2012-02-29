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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de, Samuel Eckermann, Hans-Christian Heinz
 *
 * VolumeHandler.cpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 ******************************************************************/
#ifndef VAST_VOLUME_HANDLER_HPP
#define VAST_VOLUME_HANDLER_HPP

#include <vtkImageAppendComponents.h>
#include <vtkImageData.h>
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace widget {

class VolumeHandler
{

public:
	VolumeHandler();

	bool addImage( boost::shared_ptr<ImageHolder> image, const size_t &timestep = 0 );

	vtkImageData* getMergedImage();
	
private:
	vtkImageAppendComponents* m_Merger;
	vtkImageData* m_MergedImage;

	std::list<boost::shared_ptr<ImageHolder > > m_ImageList;
};

	

}}} //end namespace


#endif //VAST_VOLUME_HANDLER_HPP