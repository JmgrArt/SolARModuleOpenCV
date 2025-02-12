/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <boost/filesystem.hpp>
#include "SolARMarker2DNaturalImageOpencv.h"
#include "SolAROpenCVHelper.h"

namespace xpcf  = org::bcom::xpcf;

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARMarker2DNaturalImageOpencv)

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

    SolARMarker2DNaturalImageOpencv::SolARMarker2DNaturalImageOpencv():ConfigurableBase(xpcf::toUUID<SolARMarker2DNaturalImageOpencv>())
    {        
        addInterface<api::input::files::IMarker2DNaturalImage>(this);
        SRef<xpcf::IPropertyMap> params = getPropertyRootNode();
        params->wrapString("filePath", m_filePath);

        m_size.width = 0;
        m_size.height = 0;

        LOG_DEBUG("SolARMarker2DNaturalImageOpencv constructor")
    }

    FrameworkReturnCode SolARMarker2DNaturalImageOpencv::loadMarker()
    {
        std::string imagePath;

        cv::FileStorage fs(m_filePath, cv::FileStorage::READ);
        fs["MarkerWidth"] >> m_size.width;
        fs["MarkerHeight"] >> m_size.height;
        fs["ImagePath"] >> imagePath;
        fs.release();

        if (imagePath.empty())
        {
            LOG_ERROR("Marker file doesn not define an image path under markup ImagePath")
            return FrameworkReturnCode::_ERROR_;
        }

        // If the path is relative, append it to the directory of the marker file
        boost::filesystem::path imageFullPath(imagePath);
        if (imageFullPath.is_relative())
        {
            imageFullPath = m_filePath;
            boost::filesystem::path parentPath = imageFullPath.parent_path();
            imageFullPath.remove_filename();
            imageFullPath/=imagePath;
        }

        LOG_DEBUG("{}",imageFullPath)

        m_ocvImage = cv::imread(imageFullPath.string(), CV_LOAD_IMAGE_COLOR);
        if(! m_ocvImage.data )  // Check for invalid input
        {
            LOG_ERROR("Error: Could not open or find the 2D natural image marker {}", m_filePath)
            return FrameworkReturnCode::_ERROR_LOAD_IMAGE;
        }

        return FrameworkReturnCode::_SUCCESS;
    }

    FrameworkReturnCode SolARMarker2DNaturalImageOpencv::getImage(SRef<Image> & img)
    {
        if(!m_ocvImage.data)
            return FrameworkReturnCode::_ERROR_LOAD_IMAGE;

        return SolAROpenCVHelper::convertToSolar(m_ocvImage,img);
    }

}
}
}  // end of namespace Solar
