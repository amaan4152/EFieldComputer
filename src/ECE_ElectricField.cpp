/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/
#include <cmath>
#include <complex>
#include <iostream>
#include "include/ECE_ElectricField.h"

ECE_ElectricField::ECE_ElectricField(double x_q, double y_q, double z_q, double q) : ECE_PointCharge(x_q, y_q, z_q, q) {}

void ECE_ElectricField::computeFieldAt(double x_p, double y_p, double z_p)
{
    double k = 9e9; // Coulomb's constant (Nm^2/C^2)
    double rx = (x_p - this->x_q);
    double ry = (y_p - this->y_q);
    double rz = (z_p - this->z_q);
    double R = this->norm3d(rx, ry, rz);
    this->Ex = (k * this->q / pow(R, 3)) * rx;
    this->Ey = (k * this->q / pow(R, 3)) * ry;
    this->Ez = (k * this->q / pow(R, 3)) * rz;
}

void ECE_ElectricField::getElectricField(double &Ex, double &Ey, double &Ez)
{
    Ex = this->Ex;
    Ey = this->Ey;
    Ez = this->Ez;
}

void ECE_ElectricField::getQLocation(double &x, double &y, double &z)
{
    x = this->x_q;
    y = this->y_q;
    z = this->z_q;
}

double ECE_ElectricField::norm3d(double x, double y, double z)
{
    double d = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    return d;
}