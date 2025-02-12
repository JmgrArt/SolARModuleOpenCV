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

#include "SolARImagesAsCameraOpencv.h"
#include "SolAROpenCVHelper.h"

namespace xpcf = org::bcom::xpcf;

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARImagesAsCameraOpencv)

namespace SolAR {
namespace MODULES {
namespace OPENCV {

    SolARImagesAsCameraOpencv::SolARImagesAsCameraOpencv():ConfigurableBase(xpcf::toUUID<SolARImagesAsCameraOpencv>())
    {
        addInterface<api::input::devices::ICamera>(this);
        SRef<xpcf::IPropertyMap> params = getPropertyRootNode();
        params->wrapString("calibrationFile", m_calibrationFile);
        params->wrapString("imagesDirectoryPath", m_ImagesDirectoryPath);
        m_is_resolution_set = false;
    }

    xpcf::XPCFErrorCode SolARImagesAsCameraOpencv::onConfigured()
    {
        LOG_DEBUG(" SolARImagesAsCameraOpencv onConfigured");
        if (m_calibrationFile.empty())
        {
            LOG_ERROR("Camera Calibration file path is empty");
            return xpcf::_FAIL;
        }
        cv::FileStorage fs(m_calibrationFile, cv::FileStorage::READ);
        cv::Mat intrinsic_parameters;
        cv::Mat distortion_parameters;

        if (fs.isOpened())
        {
            int width, height;
            fs["image_width"] >> width;
            fs["image_height"] >> height;
            fs["camera_matrix"] >> intrinsic_parameters;
            fs["distortion_coefficients"] >> distortion_parameters;

            m_resolution.width = width;
            m_resolution.height = height;
            m_is_resolution_set = true;

            if (intrinsic_parameters.empty())
            {
                LOG_ERROR ("SolARImagesAsCameraOpencv::loadCameraParameters: Use the landmark camera_matrix to define the intrinsic matrix in the .yml camera calibration file")
                return xpcf::_FAIL;
            }

            if (intrinsic_parameters.rows == m_intrinsic_parameters.rows() && intrinsic_parameters.cols == m_intrinsic_parameters.cols())
                for (int i = 0; i < intrinsic_parameters.rows; i++)
                    for (int j = 0; j < intrinsic_parameters.cols; j++)
                        m_intrinsic_parameters(i,j) = (float)intrinsic_parameters.at<double>(i,j);
            else
            {
                LOG_ERROR("SolARImagesAsCameraOpencv::loadCameraParameters: Camera Calibration should be a 3x3 Matrix")
                return xpcf::_FAIL;
            }

            if (distortion_parameters.empty())
            {
                LOG_ERROR("SolARImagesAsCameraOpencv::loadCameraParameters: Use the landmark distortion_coefficients to define the distortion vector in the .yml camera calibration file")
                return xpcf::_FAIL;
            }

            if (distortion_parameters.rows == m_distorsion_parameters.rows() && distortion_parameters.cols == m_distorsion_parameters.cols())
                for (int i = 0; i < distortion_parameters.rows; i++)
                    for (int j = 0; j < distortion_parameters.cols; j++)
                        m_distorsion_parameters(i,j) = distortion_parameters.at<double>(i,j);
            else
            {
                LOG_ERROR("SolARImagesAsCameraOpencv::loadCameraParameters: Camera distortion matrix should be a 5x1 Matrix")
                return xpcf::_FAIL;
            }
            return xpcf::_SUCCESS;
        }
        else
        {
            LOG_ERROR("SolARImagesAsCameraOpencv::loadCameraParameters: Cannot open camera calibration file ")
            return xpcf::_FAIL;
        }
    }

    void SolARImagesAsCameraOpencv::setResolution(Sizei resolution)
    {
        m_resolution = resolution;
        m_is_resolution_set = true;
    }

    FrameworkReturnCode SolARImagesAsCameraOpencv::getNextImage(SRef<Image> & img)
    {

        cv::Mat cvFrame;
        m_capture >> cvFrame;
        if(!cvFrame.data)
            return FrameworkReturnCode::_ERROR_LOAD_IMAGE;

        return SolAROpenCVHelper::convertToSolar(cvFrame,img);
    }

    FrameworkReturnCode SolARImagesAsCameraOpencv::start(){

        LOG_INFO(" SolARImagesAsCameraOpencv::setParameters");
        if(m_capture.isOpened())
        {
            m_capture.release();
        }
        m_capture = cv::VideoCapture( m_ImagesDirectoryPath);
        if (m_capture.isOpened())
        {
            if (m_is_resolution_set)
            {
                m_capture.set(CV_CAP_PROP_FRAME_WIDTH, m_resolution.width );
                m_capture.set( CV_CAP_PROP_FRAME_HEIGHT, m_resolution.height );
            }
            return FrameworkReturnCode::_SUCCESS;
        }
        else
        {
            LOG_ERROR("Cannot open images directory {}", m_ImagesDirectoryPath);
            return FrameworkReturnCode::_ERROR_;
        }
    }

    void SolARImagesAsCameraOpencv::setIntrinsicParameters(const CamCalibration & intrinsic_parameters){
//        m_intrinsic_parameters = intrinsic_parameters;
    }

     void SolARImagesAsCameraOpencv::setDistorsionParameters(const CamDistortion & distorsion_parameters){
//           m_distorsion_parameters = distorsion_parameters;
     }

     Sizei SolARImagesAsCameraOpencv::getResolution()
     {
         return m_resolution;
     }

    CamCalibration SolARImagesAsCameraOpencv::getIntrinsicsParameters(){
        return m_intrinsic_parameters;
    }

    CamDistortion SolARImagesAsCameraOpencv::getDistorsionParameters(){
        return m_distorsion_parameters;
    }

}
}
}

