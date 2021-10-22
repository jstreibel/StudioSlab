//
// Created by joao on 03/12/2019.
//

#ifndef V_SHAPE_ANALYTICSHOCKWAVE2DRADIALSYMMETRY_H
#define V_SHAPE_ANALYTICSHOCKWAVE2DRADIALSYMMETRY_H

#include "Lib/Fields/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class AnalyticShockwave2DRadialSymmetry : public Function {
        Real t=0.;

        const PosInt quant = 40;
        Real a_k[40] =    {  1.,       2.43845,   4.04976,   5.77999,  7.60296,  9.50278, 11.4687, 13.4927, 15.5689, 17.6925,
                            19.8596,  22.0669,   24.3117,   26.5917,  28.9047,  31.2491,  33.6232, 36.0256, 38.455,  40.9103,
                            43.3904,  45.8944,   48.4214,   50.9705,  53.5411,  56.1325,  58.7439, 61.3749, 64.0248, 66.6931,
                            69.3793,  72.083,    74.8036,   77.5409,  80.2944,  83.0637,  85.8485, 88.6484, 91.4632, 94.2925};

        Real beta_k[40] = {  0.,       2.66667 , -7.48737,  14.2453 ,-22.8108,  33.0932, -45.0237, 58.5473,-73.6183, 90.1981,
                          -108.253,  127.754,  -148.674,   170.989, -194.679,  219.723, -246.104, 273.805,-302.81,  333.104,
                          -364.675,  397.509,  -431.594,   466.919, -503.474,  541.247, -580.23,  620.412,-661.785, 704.341,
                          -748.071,  792.968,  -839.023,   886.23,  -934.582,  984.072,-1034.69, 1086.44,-1139.31, 1193.28};

        Real W_k(PosInt k, Real z) const;

        Real theta_k(PosInt k, Real z) const;

    public:
        //AnalyticShockwave2DRadialSymmetry(Real a0 = 1.0);

        Real gett()const{return t;}
        void sett(Real t){this->t = t;}
        Real operator()(Real r) const override;
    };


    class AnalyticShockwave2DRadialSymmetryTimeDerivative : public Function {
        Real t=0.;

        const PosInt quant = 40;
        Real a_k[40] =    {  1.,       2.43845,    4.04976,   5.77999,  7.60296,  9.50278, 11.4687, 13.4927, 15.5689, 17.6925,
                             19.8596,  22.0669,   24.3117,   26.5917,  28.9047,  31.2491,  33.6232, 36.0256, 38.455,  40.9103,
                             43.3904,  45.8944,   48.4214,   50.9705,  53.5411,  56.1325,  58.7439, 61.3749, 64.0248, 66.6931,
                             69.3793,  72.083,    74.8036,   77.5409,  80.2944,  83.0637,  85.8485, 88.6484, 91.4632, 94.2925};

        Real beta_k[40] = {     0.,       2.66667 , -7.48737,  14.2453 ,-22.8108,  33.0932, -45.0237, 58.5473,-73.6183, 90.1981,
                             -108.253,  127.754,  -148.674,   170.989, -194.679,  219.723, -246.104, 273.805,-302.81,  333.104,
                             -364.675,  397.509,  -431.594,   466.919, -503.474,  541.247, -580.23,  620.412,-661.785, 704.341,
                             -748.071,  792.968,  -839.023,   886.23,  -934.582,  984.072,-1034.69, 1086.44,-1139.31, 1193.28};

        Real dW_kdz(PosInt k, Real z) const;

        Real theta_k(PosInt k, Real z) const;

    public:
        AnalyticShockwave2DRadialSymmetryTimeDerivative() {throw "I'm not working.";}

        void sett(Real t){this->t = t;}
        Real operator()(Real r) const override;
    };

    class AnalyticShockwave2DRadialSymmetryTimeDerivativeB : public Function {
        Real dt;
        AnalyticShockwave2DRadialSymmetry shockwave1, shockwave2;

    public:
        AnalyticShockwave2DRadialSymmetryTimeDerivativeB(Real dt=1.e-5) : dt(dt) { shockwave2.sett(0+dt); }
        void sett(Real t){
            shockwave1.sett(t);
            shockwave2.sett(t+dt);
        }
        Real operator()(Real r) const override {
            const Real sw2 = shockwave2(r);
            const Real sw1 = shockwave1(r);

            return (sw2-sw1)/dt;
        };
    };
}


#endif //V_SHAPE_ANALYTICSHOCKWAVE2DRADIALSYMMETRY_H
