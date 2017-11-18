//
//  k_means.hpp
//  k_means test
//
//  Created by Shabre on 2017. 11. 17..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef k_means_hpp
#define k_means_hpp

// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>



class Point
{
private:
    int id_point, id_cluster;
    std::vector<float> values;
    int total_values;
    std::string name;
    
public:
    Point(int id_point, std::vector<float>& values, std::string name = "");
    int getID();
    void setCluster(int id_cluster);
    int getCluster();
    float getValue(int index);
    int getTotalValues();
    void addValue(float value);
    std::string getName();
};

class Cluster
{
private:
    int id_cluster;
    std::vector<float> central_values;
    std::vector<Point> points;
    
public:
    Cluster(int id_cluster, Point point);
    void addPoint(Point point);
    bool removePoint(int id_point);
    void removeAllPoints();
    float getCentralValue(int index);
    void setCentralValue(int index, float value);
    Point getPoint(int index);
    int getTotalPoints();
    int getID();
};

class KMeans
{
private:
    int K; // number of clusters
    int total_values, total_points, max_iterations;
    std::vector<Cluster> clusters;
    void init_clusters(std::vector<Point> & points);
    // return ID of nearest center (uses euclidean distance)
    
    
public:
    KMeans(int K, int total_points, int total_values, int max_iterations);
    int getIDNearestCenter(Point point);
    void reset_params(int total_points, int total_values, int max_iterations);
    void reset_clusterPoints();
    void run(std::vector<Point> & points);
};

#endif /* k_means_hpp */
