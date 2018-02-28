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

#ifndef SOLARDESCRIPTORSEXTRACTORAKAZEOPENCV_H
#define SOLARDESCRIPTORSEXTRACTORAKAZEOPENCV_H

#include "api/features/IDescriptorsExtractor.h"
// Definition of SolARDescriptorExtractorOpencv Class //
// part of SolAR namespace //

#include "ComponentBase.h"
#include "SolAROpencvAPI.h"
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "datastructure/DescriptorBuffer.h"
#include "datastructure/Keypoint.h"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

class SOLAROPENCV_EXPORT_API SolARDescriptorsExtractorAKAZEOpencv : public org::bcom::xpcf::ComponentBase,
        public api::features::IDescriptorsExtractor {
public:
    SolARDescriptorsExtractorAKAZEOpencv();
    ~SolARDescriptorsExtractorAKAZEOpencv();
    void unloadComponent () override final;
    inline std::string getTypeString() override { return std::string("DescriptorsExtractorType::AKAZE") ;};

    void extract (const SRef<Image> image, const std::vector<SRef<Keypoint>> &keypoints, SRef<DescriptorBuffer>& descriptors) override;

    XPCF_DECLARE_UUID("c8cc68db-9abd-4dab-9204-2fe4e9d010cd");

private:
    cv::Ptr<cv::AKAZE> m_extractor;
};

}
}
}  // end of namespace SolAR



#endif // SOLARDESCRIPTORSEXTRACTORAKAZEOPENCV_H