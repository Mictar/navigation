#! /usr/bin/env python3

import rospy
from sensor_msgs.msg import ScanLaser

class OsbDetect(object):
    def __init__(self) -> None:
        subs = rospy.subscribe("scan", ScanLaser, self.scanCallback)
        publish = rospy.publisher("object_pose", 
