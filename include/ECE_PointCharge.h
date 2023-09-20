/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/
#ifndef LAB1_ECE_POINTCHARGE_H_
#define LAB1_ECE_POINTCHARGE_H_

class ECE_PointCharge
{
public:
    ECE_PointCharge() = default;

    ECE_PointCharge(double x, double y, double z, double q);

    void setLocation(double x, double y, double z);

    void setCharge(double q);

protected:
    double x_q; // x-coordinate
    double y_q; // y-coordinate
    double z_q; // z-coordinate
    double q;   // charge of the point
};
#endif