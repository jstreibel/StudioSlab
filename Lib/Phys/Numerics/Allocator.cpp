#include "Allocator.h"
#include "Common/Log/Log.h"


namespace Numerics {
    Allocator::Allocator(Str name) : DerivableSingleton<Allocator>(name) {
        Log::Attention() << "Allocator instantiated " << Log::ForegroundGreen << Log::BoldFace << name << Log::Flush;
    };
    //ModelBuilder::ModelBuilder() {
    // TODO verificar se aqui precisa ser POT ou apenas divisivel pelo numero de threads na CPU (ou algo do tipo).
    // if(dev.getDevice()==device::GPU)
    //     if(!isPOT(numericParams.getN())) throw "N must be pot for GPU sim.";

    // if(p.getN()%dev.get_nThreads() != 0) throw "N%nThreads must be 0."; }

    //po::options_description Allocator::getCommandLineOptions()
    //{
    //    po::options_description desc("Options for model.");
    //
    //    desc.add_options()
    //            ("pert,p", R"(Use perturbed potential (with $V(\phi)~|\phi| - \phi^2$). Doesn't work with
    //                                         GPU yet.)")
    //            ("nfold,f", po::value<int>()->default_value(1), "Folding parameter for perturbed model. "
    //                                                            "Defines number of minima in folded potential. Beyond last "
    //                                                            "minima is vacuum. Makes sim slower but higher values "
    //                                                            "are required for very big oscillons.");
    //
    //
    //    return desc;
    //}

    auto Allocator::getNumericParams() const -> const NumericParams & {
        return numericParams;
    }

    auto Allocator::getDevice() const -> const Device & {
        return dev;
    }

}