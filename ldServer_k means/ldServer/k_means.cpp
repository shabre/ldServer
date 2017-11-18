#include "k_means.hpp"


Point::Point(int id_point, std::vector<float>& values, std::string name)
{
    this->id_point = id_point;
    total_values = values.size();
    
    for(int i = 0; i < total_values; i++)
        this->values.push_back(values[i]);
    
    this->name = name;
    id_cluster = -1;
}

int Point::getID()
{
    return id_point;
}

void Point::setCluster(int id_cluster)
{
    this->id_cluster = id_cluster;
}

int Point::getCluster()
{
    return id_cluster;
}

float Point::getValue(int index)
{
    return values[index];
}

int Point::getTotalValues()
{
    return total_values;
}

void Point::addValue(float value)
{
    values.push_back(value);
}

std::string Point::getName()
{
    return name;
}

Cluster::Cluster(int id_cluster, Point point)
{
    this->id_cluster = id_cluster;
    
    int total_values = point.getTotalValues();
    
    for(int i = 0; i < total_values; i++)
        central_values.push_back(point.getValue(i));
    
    points.push_back(point);
}

void Cluster::addPoint(Point point)
{
    points.push_back(point);
}

bool Cluster::removePoint(int id_point)
{
    int total_points = points.size();
    
    for(int i = 0; i < total_points; i++)
    {
        if(points[i].getID() == id_point)
        {
            points.erase(points.begin() + i);
            return true;
        }
    }
    return false;
}

void Cluster::removeAllPoints(){
    points.clear();
}

float Cluster::getCentralValue(int index)
{
    return central_values[index];
}

void Cluster::setCentralValue(int index, float value)
{
    central_values[index] = value;
}

Point Cluster::getPoint(int index)
{
    return points[index];
}

int Cluster::getTotalPoints()
{
    return points.size();
}

int Cluster::getID()
{
    return id_cluster;
}

int KMeans::getIDNearestCenter(Point point)
{
    float sum = 0.0, min_dist;
    int id_cluster_center = 0;
    
    for(int i = 0; i < total_values; i++)
    {
        sum += pow(clusters[0].getCentralValue(i) -
                   point.getValue(i), 2.0);
    }
    
    min_dist = sqrt(sum);
    
    for(int i = 1; i < K; i++)
    {
        float dist;
        sum = 0.0;
        
        for(int j = 0; j < total_values; j++)
        {
            sum += pow(clusters[i].getCentralValue(j) -
                       point.getValue(j), 2.0);
        }
        
        dist = sqrt(sum);
        
        if(dist < min_dist)
        {
            min_dist = dist;
            id_cluster_center = i;
        }
    }
    
    return id_cluster_center;
}

KMeans::KMeans(int K, int total_points, int total_values, int max_iterations)
{
    this->K = K;
    this->total_points = total_points;
    this->total_values = total_values;
    this->max_iterations = max_iterations;
    
    std::vector<Point> points;
    for(int i=0; i<K; i++){
        std::vector<float> values;
        for(int j=0; j< total_values; j++){
            values.push_back(i*j);
        }
        Point p(i, values, "simulator");
        points.push_back(p);
    }
    init_clusters(points);
}

void KMeans::reset_params(int total_points, int total_values, int max_iterations){
    this->total_points = total_points;
    this->total_values = total_values;
    this->max_iterations = max_iterations;
}

void KMeans::reset_clusterPoints(){
    std::vector<Cluster>::iterator it;
    for(it=clusters.begin(); it!=clusters.end(); it++){
        (*it).removeAllPoints();
    }
}

void KMeans::init_clusters(std::vector<Point> & points){
    
    std::vector<int> prohibited_indexes;
    
    // choose K distinct values for the centers of the clusters
    for(int i = 0; i < K; i++)
    {
        while(true)
        {
            int index_point = rand() % total_points;
            
            if(find(prohibited_indexes.begin(), prohibited_indexes.end(),
                    index_point) == prohibited_indexes.end())
            {
                prohibited_indexes.push_back(index_point);
                points[index_point].setCluster(i);
                Cluster cluster(i, points[index_point]);
                clusters.push_back(cluster);
                break;
            }
        }
    }
}

void KMeans::run(std::vector<Point> & points)
{
    if(K > total_points)
        return;

    if(clusters.size()==0){
        init_clusters(points);
    }
    
    int iter = 1;
    
    while(true)
    {
        bool done = true;
        
        // associates each point to the nearest center
        for(int i = 0; i < total_points; i++)
        {
            int id_old_cluster = points[i].getCluster();
            int id_nearest_center = getIDNearestCenter(points[i]);
            
            if(id_old_cluster != id_nearest_center)
            {
                if(id_old_cluster != -1)
                    clusters[id_old_cluster].removePoint(points[i].getID());
                
                points[i].setCluster(id_nearest_center);
                clusters[id_nearest_center].addPoint(points[i]);
                done = false;
            }
        }
        
        // recalculating the center of each cluster
        for(int i = 0; i < K; i++)
        {
            for(int j = 0; j < total_values; j++)
            {
                int total_points_cluster = clusters[i].getTotalPoints();
                float sum = 0.0;
                
                if(total_points_cluster > 0)
                {
                    for(int p = 0; p < total_points_cluster; p++)
                        sum += clusters[i].getPoint(p).getValue(j);
                    clusters[i].setCentralValue(j, sum / total_points_cluster);
                }
            }
        }
        
        if(done == true || iter >= max_iterations)
        {
            std::cout << "Break in iteration " << iter << "\n\n";
            break;
        }
        
        iter++;
    }
    
    // shows elements of clusters
    for(int i = 0; i < K; i++)
    {
        int total_points_cluster =  clusters[i].getTotalPoints();
        
        std::cout << "Cluster " << clusters[i].getID() + 1 << std::endl;
        /*
        for(int j = 0; j < total_points_cluster; j++)
        {
            std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
            for(int p = 0; p < total_values; p++)
                std::cout << clusters[i].getPoint(j).getValue(p) << " ";
            
            std::string point_name = clusters[i].getPoint(j).getName();
            
            if(point_name != "")
                std::cout << "- " << point_name;
            
            std::cout << std::endl;
        }
        */
        std::cout << "Cluster values: ";
        
        for(int j = 0; j < total_values; j++)
            std::cout << clusters[i].getCentralValue(j) << " ";
        
        std::cout << "\n\n";
    }
}
