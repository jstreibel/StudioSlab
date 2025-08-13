//
// Created by joao on 4/22/25.
//

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <Math/Formalism/Categories.h>

using RealT = double;    // if DevFloat is an alias (e.g., double)
using Real2D = Slab::Math::Real2D;

TEST_CASE("Real2D explicit constructor initializes x and y correctly", "[Real2D]") {
    Real2D p(3.5, -2.5);
    REQUIRE(p.x == Catch::Approx(3.5));
    REQUIRE(p.y == Catch::Approx(-2.5));
}

TEST_CASE("Addition operator (+)", "[Real2D]") {
    Real2D a(1.0, 2.0);
    Real2D b(3.0, 4.0);
    Real2D c = a + b;
    REQUIRE(c.x == Catch::Approx(4.0));
    REQUIRE(c.y == Catch::Approx(6.0));
}

TEST_CASE("Subtraction operator (const) returns new object", "[Real2D]") {
    const Real2D a(5.0, 7.0);
    Real2D b(2.0, 3.0);
    Real2D c = a - b;    // uses the const operator-
    REQUIRE(c.x == Catch::Approx(3.0));
    REQUIRE(c.y == Catch::Approx(4.0));
}

TEST_CASE("Subtraction operator (non-const) mutates the lhs", "[Real2D]") {
    Real2D a(5.0, 7.0);
    Real2D b(1.0, 2.0);
    Real2D &r = (a - b); // uses the non-const operator-
    REQUIRE(&r == &a);   // must return reference to lhs
    REQUIRE(a.x == Catch::Approx(4.0));
    REQUIRE(a.y == Catch::Approx(5.0));
}

TEST_CASE("Scalar multiplication operator (*)", "[Real2D]") {
    Real2D p(1.5, -2.5);
    RealT s = 2.0;
    Real2D r = p * s;
    REQUIRE(r.x == Catch::Approx(3.0));
    REQUIRE(r.y == Catch::Approx(-5.0));
}

TEST_CASE("Norm calculation", "[Real2D]") {
    Real2D p(3.0, 4.0);
    // √(3² + 4²) == 5
    REQUIRE(p.norm() == Catch::Approx(5.0).epsilon(1e-12));
}

TEST_CASE("Chained operations: (p+q)*s - t", "[Real2D]") {
    Real2D p(1.0, 1.0);
    Real2D q(2.0, 3.0);
    Real2D t(1.0, 1.0);
    // (p+q) = (3,4); *2 = (6,8); -t = (5,7)
    Real2D result = (p + q) * RealT(2.0) - t;
    REQUIRE(result.x == Catch::Approx(5.0));
    REQUIRE(result.y == Catch::Approx(7.0));
}

