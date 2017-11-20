#include <string>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/cloud_viewer.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <iostream>
#include <string>
#include <pcl/common/transforms.h>
#include <math.h>
#include <pcl/io/pcd_io.h>
using namespace pcl;
using namespace std;
using namespace cv;
std::vector<cv::Point3d> Generate3DPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,int num);

std::vector<Eigen::Matrix4f> RT;
std::vector<Eigen::Matrix4f> INV;

std::stringstream ss;

int mun=10;

int
main (int argc, char *argv[])
{



    Eigen::Matrix4f rt0,rt1,rt2,rt3,rt4;
    rt0<< 0,0,-1,0,  0,1,0,0,  1,0,0,0, 0,0,0,1;
    rt1<< 0,0,-1,0,  -0.95105651629,0.30901699437,0,0,  0.30901699437,0.95105651629,0,0, 0,0,0,1;
    rt2 << 0,0,-1,0,  -0.58778525229,-0.80901699437,0,0,  -0.80901699437,0.58778525229,0,0, 0,0,0,1;
    rt3 << 0,0,-1,0,  0.58778525229,-0.80901699437,0,0,  -0.80901699437,-0.58778525229,0,0, 0,0,0,1;
    rt4 << 0,0,-1,0,  0.95105651629,0.30901699437,0,0,  0.30901699437,-0.95105651629,0,0, 0,0,0,1;
    Eigen::Matrix4f inv0,inv1,inv2,inv3,inv4;
    inv0=rt0.inverse();
    inv1=rt1.inverse();
    inv2=rt2.inverse();
    inv3=rt3.inverse();
    inv4=rt4.inverse();

    RT.push_back(rt0);
    RT.push_back(rt1);
    RT.push_back(rt2);
    RT.push_back(rt3);
    RT.push_back(rt4);

    INV.push_back(inv0);
    INV.push_back(inv1);
    INV.push_back(inv2);
    INV.push_back(inv3);
    INV.push_back(inv4);


    std::vector<cv::Point2d> imagePoints;

    cv::Mat intrisicMat(3, 3, cv::DataType<double>::type); // Intrisic matrix
    intrisicMat.at<double>(0, 0) = 476.715669286;
    intrisicMat.at<double>(1, 0) = 0;
    intrisicMat.at<double>(2, 0) = 0;

    intrisicMat.at<double>(0, 1) = 0;
    intrisicMat.at<double>(1, 1) = 476.715669286;
    intrisicMat.at<double>(2, 1) = 0;

    intrisicMat.at<double>(0, 2) = 400;
    intrisicMat.at<double>(1, 2) = 400;
    intrisicMat.at<double>(2, 2) = 1;



    cv::Mat rVec(3, 1, cv::DataType<double>::type); // Rotation vector
    rVec.at<double>(0) = 0;
    rVec.at<double>(1) = 0;
    rVec.at<double>(2) = 0;

    cv::Mat tVec(3, 1, cv::DataType<double>::type); // Translation vector
    tVec.at<double>(0) = 0.4;
    tVec.at<double>(1) = 0;
    tVec.at<double>(2) = -0.1;

    cv::Mat distCoeffs(5, 1, cv::DataType<double>::type);   // Distortion vector
    distCoeffs.at<double>(0) = 0;
    distCoeffs.at<double>(1) = 0;
    distCoeffs.at<double>(2) = 0;
    distCoeffs.at<double>(3) = 0;
    distCoeffs.at<double>(4) = 0;

    std::cout << "Intrisic matrix: " << intrisicMat << std::endl << std::endl;
    std::cout << "Rotation vector: " << rVec << std::endl << std::endl;
    std::cout << "Translation vector: " << tVec << std::endl << std::endl;
    std::cout << "Distortion coef: " << distCoeffs << std::endl << std::endl;

    std::vector<cv::Point2f> projectedPoints;


    Mat I;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr colored_cloud_sum (new pcl::PointCloud<pcl::PointXYZRGB>);





    for(int k=0;k<5;k++)
    {

        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);

        ss.str("");
        ss<<"/home/zc/catkin_ws/src/laser/data/cloud"<<mun<<".pcd";

        if ( pcl::io::loadPCDFile <pcl::PointXYZ> (ss.str(), *cloud) == -1)
        {
            std::cout << "Cloud reading failed." << std::endl;
            return (-1);
        }

        std::vector<cv::Point3d> objectPoints = Generate3DPoints(cloud,k);
        cv::projectPoints(objectPoints, rVec, tVec, intrisicMat, distCoeffs, imagePoints);

        ss.str("");
        ss<<"/home/zc/catkin_ws/src/laser/data/image"<<mun<<"p"<<k+1<<".jpg";

        I = imread(ss.str(), CV_LOAD_IMAGE_COLOR);
        Mat_<Vec3b> _I = I;
        cv::flip(I,_I,1);
        //  for (unsigned int i = 0; i < imagePoints.size(); ++i)
        //  {
        //    std::cout << "3d point: " << objectPoints[i] << " Projected to " << imagePoints[i] << std::endl;
        //  }
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr colored_cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr colored_cloud_t (new pcl::PointCloud<pcl::PointXYZRGB>);

        for(int i=0;i<imagePoints.size();i++)
        {
            if(imagePoints[i].x>=0&&imagePoints[i].x<800&&imagePoints[i].y>=0&&imagePoints[i].y<800)
            {

                pcl::PointXYZRGB point;
                point.x = cloud->points[i].x;
                point.y = cloud->points[i].y;
                point.z = cloud->points[i].z;
                point.r = _I(round(imagePoints[i].x),round(imagePoints[i].y))[2];
                point.g = _I(round(imagePoints[i].x),round(imagePoints[i].y))[1];
                point.b = _I(round(imagePoints[i].x),round(imagePoints[i].y))[0];

                colored_cloud->points.push_back (point);
            }
        }



        pcl::transformPointCloud (*colored_cloud, *colored_cloud_t, INV[k]);
        colored_cloud->clear();
        for (int i=0;i<colored_cloud_t->points.size();i++)
        {

            colored_cloud_sum->points.push_back(colored_cloud_t->points[i]);

        }

    }





    pcl::visualization::PCLVisualizer viewer("Cloud viewer");
    viewer.addPointCloud(colored_cloud_sum, "sample cloud");
    viewer.setBackgroundColor(0,0,0);

    while(!viewer.wasStopped())
        //while (!viewer->wasStopped ())
        viewer.spinOnce(100);



    return (0);
}



std::vector<cv::Point3d> Generate3DPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,int num)
{
    std::vector<cv::Point3d> points;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_f (new pcl::PointCloud<pcl::PointXYZ>);

    Eigen::Matrix4f TR;
    TR << 0,0,-1,0,  0,1,0,0,  1,0,0,0, 0,0,0,1;
    pcl::transformPointCloud (*cloud, *cloud_f, RT[num]);

    pcl::PassThrough<pcl::PointXYZ> pass;
    pass.setInputCloud (cloud_f);
    pass.setFilterFieldName ("z");
    pass.setFilterLimits (0.0, 10);
    //pass.setFilterLimitsNegative (true);
    pass.filter (*cloud);
    cout<<"size:"<<cloud->size()<<endl;



    for(int i=0;i<=cloud->points.size();i++)
    {
        points.push_back(cv::Point3d(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z));
    }

    return points;
}








