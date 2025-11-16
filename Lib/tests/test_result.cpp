//
// Tests for Slab::Result (fluent-style)
//

#include <catch2/catch_all.hpp>
#include "Utils/Result.h"

using Slab::TResult;
using Slab::Str;

TEST_CASE("Result Ok holds value and succeeds", "[Result]") {
    auto r = TResult<int>::Ok(7);
    REQUIRE(r.IsSuccess());
    REQUIRE_FALSE(r.IsFailure());
    REQUIRE(r.Errors().empty());
    REQUIRE(r.Messages().empty());
    REQUIRE(r.Value() == 7);
}

TEST_CASE("Result Fail holds errors and throws on Value", "[Result]") {
    auto r = TResult<int>::Fail(Str{"Boom"}).WithError("More");
    REQUIRE(r.IsFailure());
    REQUIRE_FALSE(r.IsSuccess());
    REQUIRE(r.Errors().size() == 2);
    REQUIRE_THROWS_AS((void)r.Value(), std::logic_error);
}

TEST_CASE("WithMessage appends messages", "[Result]") {
    auto r = TResult<int>::Ok(1).WithMessage("A").WithMessage("B");
    REQUIRE(r.IsSuccess());
    REQUIRE(r.Messages().size() == 2);
}

TEST_CASE("Map transforms value on success and propagates failure", "[Result]") {
    auto ok = TResult<int>::Ok(3).Map<Str>([](int v){ return std::to_string(v * 2); });
    REQUIRE(ok.IsSuccess());
    REQUIRE(ok.Value() == "6");

    auto fail = TResult<int>::Fail("e1").Map<Str>([](int v){ return std::to_string(v); });
    REQUIRE(fail.IsFailure());
    REQUIRE(fail.Errors().size() == 1);
}

TEST_CASE("Then chains to another Result and propagates errors", "[Result]") {
    auto step = [](int v) -> TResult<int> {
        if (v % 2 == 0) return TResult<int>::Ok(v/2);
        return TResult<int>::Fail("odd");
    };

    auto r1 = TResult<int>::Ok(8).Then<int>(step).Then<int>(step);
    REQUIRE(r1.IsSuccess());
    REQUIRE(r1.Value() == 2);

    auto r2 = TResult<int>::Ok(3).Then<int>(step);
    REQUIRE(r2.IsFailure());
    REQUIRE_FALSE(r2.Errors().empty());

    auto r3 = TResult<int>::Fail("nope").Then<int>(step);
    REQUIRE(r3.IsFailure());
    REQUIRE(r3.Errors().size() == 1);
}

TEST_CASE("OnSuccess/OnFailure callbacks fire appropriately", "[Result]") {
    int okCount = 0;
    int failCount = 0;

    TResult<int>::Ok(10)
        .OnSuccess([&](const int& v){ okCount += v; })
        .OnFailure([&](const Slab::StrVector&){ failCount++; });

    TResult<int>::Fail("bad")
        .OnSuccess([&](const int&){ okCount += 100; })
        .OnFailure([&](const Slab::StrVector& errs){ failCount += int(errs.size()); });

    REQUIRE(okCount == 10);
    REQUIRE(failCount == 1);
}

TEST_CASE("Result<void> basic flow and Then", "[Result]") {
    int side = 0;
    auto r = TResult<void>::Ok()
        .WithMessage("done")
        .OnSuccess([&](){ side += 2; })
        .OnFailure([&](const Slab::StrVector&){ side += 100; });

    REQUIRE(r.IsSuccess());
    REQUIRE(side == 2);

    auto next = r.Then<int>([&](){ return TResult<int>::Ok(5); });
    REQUIRE(next.IsSuccess());
    REQUIRE(next.Value() == 5);

    auto bad = TResult<void>::Fail("x")
        .Then<int>([&](){ return TResult<int>::Ok(1); });
    REQUIRE(bad.IsFailure());
}

