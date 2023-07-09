#ifndef FIELDBUILDER_H
#define FIELDBUILDER_H

#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Toolset/Device.h"

namespace Numerics {

	class Allocator {
		static Allocator *mySingleInstance;

	protected:

		const Str name;

		Allocator(Str name);

		static void Instantiate(Allocator *allocator);

	public:

		static auto getInstance() -> Allocator &;

		template<typename StateType>
		static auto NewFieldState() -> StateType*;

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


	template<typename StateType>
	auto Allocator::NewFieldState() -> StateType * {
		auto &me = Allocator::getInstance();

		return (StateType*)me.newFieldState();
	}
}

#endif // FIELDBUILDER_H