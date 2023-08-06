from numpy import sqrt, heaviside

class Shockwave2D(object):
    quant = 40
    a_k = 1.,      2.43845, 4.04976, 5.77999, 7.60296, 9.50278, 11.4687, 13.4927, 15.5689, 17.6925, \
         19.8596, 22.0669, 24.3117, 26.5917, 28.9047, 31.2491,  33.6232, 36.0256, 38.455,  40.9103, \
         43.3904, 45.8944, 48.4214, 50.9705, 53.5411, 56.1325,  58.7439, 61.3749, 64.0248, 66.6931, \
         69.3793, 72.083,  74.8036, 77.5409, 80.2944, 83.0637,  85.8485, 88.6484, 91.4632, 94.2925

    beta_k = 0.,      2.66667, -7.48737, 14.2453, -22.8108, 33.0932, -45.0237, 58.5473, -73.6183, 90.1981, \
          -108.253, 127.754, -148.674,  170.989, -194.679, 219.723, -246.104, 273.805, -302.81,  333.104, \
          -364.675, 397.509, -431.594,  466.919, -503.474, 541.247, -580.23,  620.412, -661.785, 704.341, \
          -748.071, 792.968, -839.023,  886.23,  -934.582, 984.072, -1034.69, 1086.44, -1139.31, 1193.28

    def __init__(self):
        pass

    def W_k(self, k: int, z: float):
        q = self.quant
        a_k = self.a_k
        beta_k = self.beta_k

        if k >= q: return .0;
        if z > .0: return .0;

        c = 0.666666666666667

        if k == 0:
            return c * (z + a_k[0]);

        a = a_k[k - 1]

        if z == .0: return .0

        if k % 2: # impar
            return -c * (z + a) + beta_k[k] * (1. / sqrt(-z) - 1. / sqrt(a))
        else:
            return c * (z + a) + beta_k[k] * (1. / sqrt(-z) - 1. / sqrt(a))

    def theta_k(self, k:int, z:float):
        UnitStep = lambda x: heaviside(x, 1)
        a_k = self.a_k

        if k == 0: return UnitStep(-z) * UnitStep(-(-z-a_k[0]));

        return UnitStep(-(z + a_k[k - 1])) * UnitStep(z + a_k[k])

    def __call__(self, r: float, t: float):
        theta_k = self.theta_k
        W_k = self.W_k

        z = .25 * (r**2 - t**2)

        totalSum = .0

        for n in range(self.quant):
            totalSum += theta_k(n, z) * W_k(n, z)

        return totalSum

