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

#ifndef SOLARPOSEESTIMATIONPLANARPOINTSOPENCV_H
#define SOLARPOSEESTIMATIONPLANARPOINTSOPENCV_H
#include <vector>
#include "opencv2/core.hpp"
#include "api/solver/pose/I3DTransformSACFinderFrom2D3D.h"
#include "datastructure/Image.h"
#include "SolAROpencvAPI.h"
#include "xpcf/component/ConfigurableBase.h"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {
    /**
     * @class SolARPoseEstimationPlanarPointsOpencv
     * @brief Finds the camera pose of 2D-3D planar points correspondances based on opencv homography.
     */
    class SOLAROPENCV_EXPORT_API SolARPoseEstimationPlanarPointsOpencv : public org::bcom::xpcf::ConfigurableBase,
        public api::solver::pose::I3DTransformSACFinderFrom2D3D
    {
    public:
        ///@brief SolARPoseEstimationPlanarPointsOpencv constructor;
        SolARPoseEstimationPlanarPointsOpencv();
        ///@brief SolARPoseEstimationPlanarPointsOpencv destructor;
        ~SolARPoseEstimationPlanarPointsOpencv();

        /// @brief Estimates camera pose from a set of 2D image points of their corresponding 3D  world points.
        /// @param[in] imagePoints, set of 2d_points seen in view_1.
        /// @param[in]  worldPoints, set of 3d_points corresponding to view_1.
        /// @param[out] imagePoints_inlier, image 2d points that are inliers
        /// @param[out] worldPoints_inlier, world 3d points that are inliers.
        /// @param[out] pose, camera pose (pose the camera defined in world corrdinate system) expressed as a Transform3D.
        /// @param[in] initialPose (Optional), a tranfsform3D to initialize the pose (reducing the convergence time and improving its success). If your world points are planar, do not use this argument.
        FrameworkReturnCode estimate(const std::vector<SRef<Point2Df>> & imagePoints,
                                     const std::vector<SRef<Point3Df>> & worldPoints,
                                     std::vector<SRef<Point2Df>>&imagePoints_inlier,
                                     std::vector<SRef<Point3Df>>&worldPoints_inlier,
                                     Transform3Df & pose,
                                     const Transform3Df initialPose = Transform3Df::Identity()) override;

        /// @brief this method is used to set intrinsic parameters and distorsion of the camera
        /// @param[in] Camera calibration matrix parameters.
        /// @param[in] Camera distorsion parameters.
        void setCameraParameters(const CamCalibration & intrinsicParams,
                                 const CamDistortion & distorsionParams)  override;

        void unloadComponent () override final;


    private:

        // minimal number of inliers to validate the pose
        int m_minNbInliers = 10;

        /// @brief Inlier threshold value used by the RANSAC procedure. The parameter value is the maximum allowed distance between the observed and computed point projections to consider it an inlier.
        float m_reprojErrorThreshold = 0.1;


        cv::Mat m_camMatrix;
        cv::Mat m_camDistorsion;
    };
}
}
}

#endif // SOLARPOSEESTIMATIONPLANARPOINTSOPENCV_H