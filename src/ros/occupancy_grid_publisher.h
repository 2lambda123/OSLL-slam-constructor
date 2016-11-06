#ifndef __OCCUPANCY_GRID_PUBLISHER_H_INCLUDED
#define __OCCUPANCY_GRID_PUBLISHER_H_INCLUDED

#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>

#include "../core/state_data.h"
#include "../core/maps/grid_map.h"

template <typename GridMapType>
class OccupancyGridPublisher : public WorldMapObserver<GridMapType> {
public: // method
  OccupancyGridPublisher(ros::Publisher pub, double publ_interval_secs = 5.0):
    _map_pub{pub}, _publishing_interval{publ_interval_secs} {}

  virtual void on_map_update(const GridMapType &map) override {
    if ((ros::Time::now() - _last_pub_time) < _publishing_interval) {
      return;
    }

    nav_msgs::OccupancyGrid map_msg;
    map_msg.info.map_load_time = ros::Time::now();
    map_msg.info.width = map.width();
    map_msg.info.height = map.height();
    map_msg.info.resolution = map.scale();
    // move map to the middle
    nav_msgs::MapMetaData &info = map_msg.info;
    info.origin.position.x = -info.resolution * info.height / 2;
    info.origin.position.y = -info.resolution * info.width  / 2;
    info.origin.position.z = 0;

    map_msg.data.reserve(info.height * info.width);
    DiscretePoint2D pnt;
    for (pnt.y = 0; pnt.y < map.height(); ++pnt.y) {
      for (pnt.x = 0; pnt.x < map.width(); ++pnt.x) {
        double value = (double)map[pnt];
        int cell_value = value == -1 ? -1 : value * 100;
        map_msg.data.push_back(cell_value);
      }
    }

    _map_pub.publish(map_msg);
    _last_pub_time = ros::Time::now();
  }

private: // fields
  ros::Publisher _map_pub;
  ros::Time _last_pub_time;
  ros::Duration _publishing_interval;
};

#endif
