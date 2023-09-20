/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/
#ifndef LAB1_ECE_ELECTRICFIELD_H_
#define LAB1_ECE_ELECTRICFIELD_H_

#include <atomic>
#include "ECE_PointCharge.h"

class ECE_ElectricField : public ECE_PointCharge
{
public:
    ECE_ElectricField() = default;

    ECE_ElectricField(double x_q, double y_q, double z_q, double q);

    void computeFieldAt(double x_p, double y_p, double z_p);

    void getElectricField(double &Ex, double &Ey, double &Ez);

    void getQLocation(double &x, double &y, double &z);

protected:
    double Ex; // Electric field in the x-direction
    double Ey; // Electric field in the y-direction
    double Ez; // Electric field in the z-direction

private:
    double norm3d(double x, double y, double z);
};

#endif