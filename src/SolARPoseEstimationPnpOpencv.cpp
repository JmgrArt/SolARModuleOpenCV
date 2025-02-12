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
#include "SolARPoseEstimationPnpOpencv.h"
#include "SolAROpenCVHelper.h"
#include "opencv2/calib3d/calib3d.hpp"

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARPoseEstimationPnpOpencv);

namespace xpcf  = org::bcom::xpcf;

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

SolARPoseEstimationPnpOpencv::SolARPoseEstimationPnpOpencv():ConfigurableBase(xpcf::toUUID<SolARPoseEstimationPnpOpencv>())
{
    addInterface<api::solver::pose::I3DTransformFinderFrom2D3D>(this);
    SRef<xpcf::IPropertyMap> params = getPropertyRootNode();
    params->wrapInteger("iterationsCount", m_iterationsCount);
    params->wrapFloat("reprojError", m_reprojError);
    params->wrapFloat("confidence", m_confidence);
	params->wrapInteger("minNbInliers", m_NbInliersToValidPose);

    m_camMatrix.create(3, 3, CV_32FC1);
    m_camDistorsion.create(5, 1, CV_32FC1);

    LOG_DEBUG(" SolARPoseEstimationOpencv constructor");
}

SolARPoseEstimationPnpOpencv::~SolARPoseEstimationPnpOpencv(){

}

FrameworkReturnCode SolARPoseEstimationPnpOpencv::estimate( const std::vector<SRef<Point2Df>> & imagePoints,
                                                            const std::vector<SRef<Point3Df>> & worldPoints,
                                                            Transform3Df & pose,
                                                            const Transform3Df initialPose) {

    std::vector<cv::Point2f> imageCVPoints;
    std::vector<cv::Point3f> worldCVPoints;

    Transform3Df initialPoseInverse = initialPose.inverse();

    if (worldPoints.size()!=imagePoints.size() || worldPoints.size()< 4 ){
        LOG_WARNING("world/image points must be valid ( equal and > to 4)");
        return FrameworkReturnCode::_ERROR_  ; // vector of 2D and 3D points must have same size
    }

    for (int i=0;i<imagePoints.size();++i) {
        Point2Df point2D = *(imagePoints.at(i));
        Point3Df point3D = *(worldPoints.at(i));
        imageCVPoints.push_back(cv::Point2f(point2D.getX(), point2D.getY()));
        worldCVPoints.push_back(cv::Point3f(point3D.getX(), point3D.getY(),point3D.getZ()));
    }

    cv::Mat Rvec;
    cv::Mat_<float> Tvec;
    cv::Mat raux, taux;

    // If initialPose is not Identity, set the useExtrinsicGuess to true. Warning, does not work on coplanar points
    if (!initialPoseInverse.isApprox(Transform3Df::Identity()))
    {
        RotationMatrixf initialRot = initialPoseInverse.rotation();
        cv::Mat cvInitialPose = SolAROpenCVHelper::mapToOpenCV(initialRot);
        cv::Rodrigues(cvInitialPose, raux);
        Vector3f initialTranslation = initialPoseInverse.translation();
        taux = SolAROpenCVHelper::mapToOpenCV(initialTranslation);

        cv::solvePnP(worldCVPoints, imageCVPoints, m_camMatrix, m_camDistorsion, raux, taux, 1, cv::SOLVEPNP_ITERATIVE);
    }
    else
        cv::solvePnP(worldCVPoints, imageCVPoints, m_camMatrix, m_camDistorsion, raux, taux, 0, cv::SOLVEPNP_ITERATIVE);

    raux.convertTo(Rvec, CV_32F);
    taux.convertTo(Tvec, CV_32F);

    cv::Mat_<float> rotMat(3, 3);
    cv::Rodrigues(Rvec, rotMat);

    Eigen::Matrix3f Rpose;
    Eigen::Vector3f Tpose;

    for (int col = 0; col<3; col++){
          for (int row = 0; row<3; row++){
                  pose(row,col) = rotMat(row, col);
          }
         pose(col,3) = Tvec(col);
    }
    pose(3,0)  = 0.0;
    pose(3,1)  = 0.0;
    pose(3,2)  = 0.0;
    pose(3,3)  = 1.0;

    pose = pose.inverse();

    return FrameworkReturnCode::_SUCCESS;

}

FrameworkReturnCode SolARPoseEstimationPnpOpencv::estimate( const std::vector<SRef<Point2Df>> & imagePoints,
                                                            const std::vector<SRef<Point3Df>> & worldPoints,
                                                            std::vector<SRef<Point2Df>>&imagePoints_inlier,
                                                            std::vector<SRef<Point3Df>>&worldPoints_inlier,
                                                            Transform3Df & pose,
                                                            const Transform3Df initialPose) {

    std::vector<cv::Point2f> imageCVPoints;
    std::vector<cv::Point3f> worldCVPoints;
    std::vector<int> inliers;

    Transform3Df initialPoseInverse = initialPose.inverse();

    if (worldPoints.size()!=imagePoints.size() || worldPoints.size()< 4 ){
        LOG_WARNING("world/image points must be valid ( equal and > to 4)");
        return FrameworkReturnCode::_ERROR_  ; // vector of 2D and 3D points must have same size
    }

    for (int i=0;i<imagePoints.size();++i) {
        Point2Df point2D = *(imagePoints.at(i));
        Point3Df point3D = *(worldPoints.at(i));
        imageCVPoints.push_back(cv::Point2f(point2D.getX(), point2D.getY()));
        worldCVPoints.push_back(cv::Point3f(point3D.getX(), point3D.getY(),point3D.getZ()));
    }
     cv::Mat Rvec;
     cv::Mat_<float> Tvec;
     cv::Mat raux, taux;
     cv::Mat inliers_cv;

     // If initialPose is not Identity, set the useExtrinsicGuess to true. Warning, does not work on coplanar points
     if (!initialPoseInverse.isApprox(Transform3Df::Identity()))
     {
         RotationMatrixf initialRot = initialPoseInverse.rotation();
         cv::Mat cvInitialPose = SolAROpenCVHelper::mapToOpenCV(initialRot);
         cv::Rodrigues(cvInitialPose, raux);
         Vector3f initialTranslation = initialPoseInverse.translation();
         taux = SolAROpenCVHelper::mapToOpenCV(initialTranslation);

         cv::solvePnPRansac(worldCVPoints, imageCVPoints, m_camMatrix, m_camDistorsion, raux,taux, true,
                               m_iterationsCount, m_reprojError, m_confidence, inliers_cv);
     }
     else
         cv::solvePnPRansac(worldCVPoints, imageCVPoints, m_camMatrix, m_camDistorsion, raux,taux, false,
                               m_iterationsCount, m_reprojError, m_confidence, inliers_cv);

     std::vector<cv::Point3f>in3d;
     std::vector<cv::Point2f>in2d;
     std::vector<cv::Point2f> projected3D;
     cv::projectPoints(worldCVPoints, raux, taux, m_camMatrix, m_camDistorsion, projected3D);

     for (int i = 0; i<projected3D.size(); i++) {
         double err_reprj = norm(projected3D[i]-imageCVPoints[i]);
         if (err_reprj <m_reprojError) {
             worldPoints_inlier.push_back(worldPoints[i]);
             imagePoints_inlier.push_back(imagePoints[i]);

             in2d.push_back(cv::Point2f(imagePoints[i]->getX(),imagePoints[i]->getY()));
             in3d.push_back(cv::Point3f(worldPoints[i]->getX(),worldPoints[i]->getY(),worldPoints[i]->getZ()));
         }
     }
	 
     if (in3d.size()!=in2d.size() || in3d.size() < std::max(3, m_NbInliersToValidPose)){
         LOG_WARNING("world/image inliers points must be valid ( equal and > to {}): {} inliers for {} input points", std::max(3, m_NbInliersToValidPose), in3d.size(), worldPoints.size());
         return FrameworkReturnCode::_ERROR_  ; // vector of 2D and 3D points must have same size
     }

     cv::solvePnP(in3d, in2d, m_camMatrix, m_camDistorsion, raux,taux, true);

    raux.convertTo(Rvec, CV_32F);
    taux.convertTo(Tvec, CV_32F);

    cv::Mat_<float> rotMat(3, 3);
    cv::Rodrigues(Rvec, rotMat);

    Eigen::Matrix3f Rpose;
    Eigen::Vector3f Tpose;

    for (int col = 0; col<3; col++){
          for (int row = 0; row<3; row++){
                  pose(row,col) = rotMat(row, col);
          }
         pose(col,3) = Tvec(col);
    }
    pose(3,0)  = 0.0;
    pose(3,1)  = 0.0;
    pose(3,2)  = 0.0;
    pose(3,3)  = 1.0;

    pose = pose.inverse();

    return FrameworkReturnCode::_SUCCESS;
}


void SolARPoseEstimationPnpOpencv::setCameraParameters(const CamCalibration & intrinsicParams, const CamDistortion & distorsionParams) {
    //TODO.. check to inverse
    this->m_camDistorsion.at<float>(0, 0)  = distorsionParams(0);
    this->m_camDistorsion.at<float>(1, 0)  = distorsionParams(1);
    this->m_camDistorsion.at<float>(2, 0)  = distorsionParams(2);
    this->m_camDistorsion.at<float>(3, 0)  = distorsionParams(3);
    this->m_camDistorsion.at<float>(4, 0)  = distorsionParams(4);

    this->m_camMatrix.at<float>(0, 0) = intrinsicParams(0,0);
    this->m_camMatrix.at<float>(0, 1) = intrinsicParams(0,1);
    this->m_camMatrix.at<float>(0, 2) = intrinsicParams(0,2);
    this->m_camMatrix.at<float>(1, 0) = intrinsicParams(1,0);
    this->m_camMatrix.at<float>(1, 1) = intrinsicParams(1,1);
    this->m_camMatrix.at<float>(1, 2) = intrinsicParams(1,2);
    this->m_camMatrix.at<float>(2, 0) = intrinsicParams(2,0);
    this->m_camMatrix.at<float>(2, 1) = intrinsicParams(2,1);
    this->m_camMatrix.at<float>(2, 2) = intrinsicParams(2,2);
}

}
}
}
