#ifndef FIELDBUILDER_H
#define FIELDBUILDER_H

#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Toolset/Device.h"

namespace Numerics {

	class Allocator {
		static Allocator *mySingleInstance;

	protected:

		const String name;

		Allocator(String name);

		static void Instantiate(Allocator *allocator);

	public:

		static auto getInstance() -> Allocator &;

		template<class ARB_FUNC_TYPE>
		static ARB_FUNC_TYPE *NewFunctionArbitrary() {
			return (ARB_FUNC_TYPE *) getInstance().newFunctionArbitrary();
		}

		virtual auto newFunctionArbitrary() -> void * = 0;

		virtual auto newFieldState() -> void * = 0;

		virtual auto getSystemSolver() -> void * = 0;

		auto getNumericParams() const -> const NumericParams &;

		auto getDevice() const -> const Device &;


	protected:
		NumericParams numericParams;
		Device dev;

	};

}

#endif // FIELDBUILDER_H