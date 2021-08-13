/******************************************************************************
 *                      Code generated with sympy 1.1.1                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                       This file is part of 'project'                       *
 ******************************************************************************/
#include "block-intersector.h"
#include <cmath>
//#include "fastmath.h"

double tA(double b_y, double c_y, double n_y, double y) {

   double tA_result;
   tA_result = -1.0L/2.0L*(n_y - sqrt(4*b_y*c_y - 4*c_y*y - 2*c_y + pow(n_y, 2)))/c_y;
   return tA_result;

}

double tpA(double b_x, double b_y, double c_x, double c_y, double n_x, double n_y, double x, double y) {

   double tpA_result;
   tpA_result = (1.0L/2.0L)*(-2*b_x*pow(c_y, 2) + 2*b_y*c_x*c_y - 2*c_x*c_y*y - c_x*c_y + c_x*pow(n_y, 2) + 2*pow(c_y, 2)*x + pow(c_y, 2) - c_y*n_x*n_y + (-c_x*n_y + c_y*n_x)*sqrt(4*b_y*c_y - 4*c_y*y - 2*c_y + pow(n_y, 2)))/pow(c_y, 2);
   return tpA_result;

}

double tB(double b_x, double c_x, double n_x, double x) {

   double tB_result;
   tB_result = -1.0L/2.0L*(n_x + sqrt(4*b_x*c_x - 4*c_x*x + 2*c_x + pow(n_x, 2)))/c_x;
   return tB_result;

}

double tpB(double b_x, double b_y, double c_x, double c_y, double n_x, double n_y, double x, double y) {

   double tpB_result;
   tpB_result = (1.0L/2.0L)*(2*b_x*c_x*c_y - 2*b_y*pow(c_x, 2) + 2*pow(c_x, 2)*y + pow(c_x, 2) - 2*c_x*c_y*x + c_x*c_y - c_x*n_x*n_y + c_y*pow(n_x, 2) + (-c_x*n_y + c_y*n_x)*sqrt(4*b_x*c_x - 4*c_x*x + 2*c_x + pow(n_x, 2)))/pow(c_x, 2);
   return tpB_result;

}

double tC(double b_y, double c_y, double n_y, double y) {

   double tC_result;
   tC_result = -1.0L/2.0L*(n_y + sqrt(4*b_y*c_y - 4*c_y*y + 2*c_y + pow(n_y, 2)))/c_y;
   return tC_result;

}

double tpC(double b_x, double b_y, double c_x, double c_y, double n_x, double n_y, double x, double y) {

   double tpC_result;
   tpC_result = (1.0L/2.0L)*(2*b_x*pow(c_y, 2) - 2*b_y*c_x*c_y + 2*c_x*c_y*y - c_x*c_y - c_x*pow(n_y, 2) - 2*pow(c_y, 2)*x + pow(c_y, 2) + c_y*n_x*n_y + (-c_x*n_y + c_y*n_x)*sqrt(4*b_y*c_y - 4*c_y*y + 2*c_y + pow(n_y, 2)))/pow(c_y, 2);
   return tpC_result;

}

double tD(double b_x, double c_x, double n_x, double x) {

   double tD_result;
   tD_result = -1.0L/2.0L*(n_x - sqrt(4*b_x*c_x - 4*c_x*x - 2*c_x + pow(n_x, 2)))/c_x;
   return tD_result;

}

double tpD(double b_x, double b_y, double c_x, double c_y, double n_x, double n_y, double x, double y) {

   double tpD_result;
   tpD_result = (1.0L/2.0L)*(-2*b_x*c_x*c_y + 2*b_y*pow(c_x, 2) - 2*pow(c_x, 2)*y + pow(c_x, 2) + 2*c_x*c_y*x + c_x*c_y + c_x*n_x*n_y - c_y*pow(n_x, 2) + (-c_x*n_y + c_y*n_x)*sqrt(4*b_x*c_x - 4*c_x*x - 2*c_x + pow(n_x, 2)))/pow(c_x, 2);
   return tpD_result;

}
