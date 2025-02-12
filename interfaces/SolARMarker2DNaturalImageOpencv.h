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

#ifndef SOLARMARKER2DNATURALIMAGEOPENCV_H
#define SOLARMARKER2DNATURALIMAGEOPENCV_H

#include "api/input/files/IMarker2DNaturalImage.h"

#include "xpcf/component/ConfigurableBase.h"
#include "SolAROpencvAPI.h"
#include "opencv2/opencv.hpp"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

class SOLAROPENCV_EXPORT_API SolARMarker2DNaturalImageOpencv : public org::bcom::xpcf::ConfigurableBase,
        public api::input::files::IMarker2DNaturalImage {
public:
    SolARMarker2DNaturalImageOpencv();

    ~SolARMarker2DNaturalImageOpencv() = default;
    void unloadComponent () override final;

    FrameworkReturnCode loadMarker() override;
    FrameworkReturnCode getImage(SRef<Image> & img) override;

 private:
     cv::Mat m_ocvImage;

     /// @brief the path to the file describing the 2D Squared binary marker
     std::string m_filePath ="";
};

}
}
}

#endif
