#include "provided.h"
#include <vector>
#include <iostream>
#include "ExpandableHashMap.h"
#include <queue>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    struct DelivCrow {
        DelivCrow(GeoCoord coord1, GeoCoord coord2, double dist) : m_start(coord1), m_end(coord2), m_dist(dist) {}
        GeoCoord m_start;
        GeoCoord m_end;
        double m_dist;
    };
    struct CompareDist {
        bool operator()(DelivCrow const& p1, DelivCrow const& p2)
        {
            // return "true" if "p1" is ordered  
            // before "p2", for example: 
            return p1.m_dist < p2.m_dist;
        }
    };
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    //Makes sure Distance are 0
    oldCrowDistance = newCrowDistance = 0;

    //Nearest Neighbor Approach
    GeoCoord coord1 = depot;


    //Calculates oldCrowDistance
    for (int i = 0; i < (int) deliveries.size(); i++) {
        GeoCoord coord2 = deliveries[i].location;
        double currentDist = distanceEarthMiles(coord1, coord2);
        oldCrowDistance += currentDist;
        coord1 = coord2;
    }

    //Reorganizes vector using pointers
    vector<DeliveryRequest*> reorganized;
    coord1 = depot; //Previous location
    DeliveryRequest* nextLocation;
    int n = 0;
    while (n < (int) deliveries.size()) {
        nextLocation = &deliveries[0]; //reset next location to default first value
        double minDist = distanceEarthMiles(coord1, nextLocation->location);
        for (int j = 0; j < (int) deliveries.size(); j++) { //Finds closest city
            double currentDist = distanceEarthMiles(coord1, deliveries[j].location);
            if (minDist > currentDist) {
                bool duplicate = false;
                for (int i = 0; i < (int) reorganized.size(); i++) {
                    if (reorganized[i] == &deliveries[j]) { //Same Address == already sorted, skip
                        duplicate = true;
                    }
                }
                if (!duplicate) {
                    nextLocation = &deliveries[j]; //Closest city to previous delivery point
                }
            }
        }
        //Found closest city, put in order
        reorganized.push_back(nextLocation);
        coord1 = nextLocation->location;
        newCrowDistance += minDist;
        n++;
    }
    //Back to Depot
    oldCrowDistance += distanceEarthMiles(depot, deliveries[deliveries.size() - 1].location); 
    newCrowDistance += distanceEarthMiles(reorganized[reorganized.size()-1]->location, depot);
    cerr << "oldCrowDistance is " << oldCrowDistance << endl << "newCrowDistance is " << newCrowDistance << endl;
    cerr << "Reorganized vector is size " << reorganized.size() << " and original is " << deliveries.size() << endl;
    //Reorganize original vector
    for (int i = 0; i < (int) deliveries.size(); i++) {
        DeliveryRequest m(deliveries[i].item, deliveries[i].location); //holds replaced value
        int temp = reorganized[i] - &deliveries[0]; //Position of reorganized[i]
        deliveries[i] = *reorganized[i]; //Swaps Delivery requests
        deliveries[temp] = m;
    }
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
