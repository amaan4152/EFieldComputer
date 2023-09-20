/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/
#include "include/ECE_PointCharge.h"

ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q) : x_q(x), y_q(y), z_q(z), q(q) {}

void ECE_PointCharge::setLocation(double x, double y, double z)
{
    this->x_q = x;
    this->y_q = y;
    this->z_q = z;
    return;
}

void ECE_PointCharge::setCharge(double q)
{
    this->q = q;
    return;
}