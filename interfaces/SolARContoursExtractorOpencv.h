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

#ifndef SOLARCONTOURSEXTRACTOROPENCV_H
#define SOLARCONTOURSEXTRACTOROPENCV_H

#include "api/features/IContoursExtractor.h"
#include "ComponentBase.h"
#include "SolAROpencvAPI.h"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

class SOLAROPENCV_EXPORT_API SolARContoursExtractorOpencv : public org::bcom::xpcf::ComponentBase,
        public api::features::IContoursExtractor {
public:
    SolARContoursExtractorOpencv();
    ~SolARContoursExtractorOpencv() = default;

    void setParameters (float minContourSize)  override;

    FrameworkReturnCode extract(const SRef<Image> inputImg, std::vector<SRef<Contour2Df>> & contours) override;

    void unloadComponent () override final;
    XPCF_DECLARE_UUID("6acf8de2-cc63-11e7-abc4-cec278b6b50a");

private:
    float m_minContourSize;
};

}
}
}  // end of namespace Solar

#endif // SOLARCONTOURSEXTRACTOROPENCV_H