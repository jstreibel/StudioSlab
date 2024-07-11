//
// Created by joao on 7/9/24.
//

#include "3rdParty/ImGui.h"

#include "HistogramsViewer_KG.h"
#include "Graphics/Graph/PlotThemeManager.h"
#include "Graphics/Graph/Plotter.h"

#include "Math/Function/RtoR/Operations/Histogram.h"
#include "Math/Function/RtoR/Operations/FromR2toRNumeric.h"


namespace Slab::Models::KGRtoR {

    HistogramsViewer_KG::HistogramsViewer_KG(const Pointer<Graphics::GUIWindow> &gui_window)
    : KGViewer(gui_window) {

        auto energy_window    = New<PlotWindow>("Energy histogram");
        auto kinetic_window   = New<PlotWindow>("Kinetic energy histogram");
        auto gradient_window  = New<PlotWindow>("Gradient energy histogram");
        auto potential_window = New<PlotWindow>("Potential energy histogram");

        histogram_windows = {energy_window, kinetic_window, gradient_window, potential_window};

        addWindow(gradient_window);
        addWindow(energy_window);

        addWindow(potential_window, true);
        addWindow(kinetic_window);

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles.begin();

        Graphics::Plotter::AddPointSet(energy_window,
                                       Naked(histogram_data_energy),
                                       *style++, "E")->setAffectGraphRanges(true);
        Graphics::Plotter::AddPointSet(kinetic_window,
                                       Naked(histogram_data_kinetic),
                                       *style++, "K")->setAffectGraphRanges(true);
        Graphics::Plotter::AddPointSet(gradient_window,
                                       Naked(histogram_data_gradient),
                                       *style++, "grad")->setAffectGraphRanges(true);
        Graphics::Plotter::AddPointSet(potential_window,
                                       Naked(histogram_data_potential),
                                       *style++, "V")->setAffectGraphRanges(true);
    }

    void HistogramsViewer_KG::draw() {
        beginGUI();

        fix func = getFunction();
        if(func == nullptr) return;

        fix min_t = func->getDomain().yMin;
        fix max_t = func->getDomain().yMax;
        auto dt = func->getDomain().getLy() / (func->getM()-1);

        auto t  = (float)t_min;
        auto Δt = (float)t_delta;

        if(ImGui::SliderInt("n bins", &nbins, 10, 2000)
         | ImGui::Checkbox("Pretty bars", &pretty)
         | ImGui::SliderFloat("t##histogram", &t, min_t, max_t-t_delta)
         | ImGui::SliderFloat("Δt##histogram", &Δt, dt, max_t-t)) {
            t_min = (Slab::Real)t;
            t_delta = (Slab::Real)Δt;

            updateHistograms();
        }

        ImGui::Text("Sheer data size: %i", (int)sheer_size);

        endGUI();
        WindowPanel::draw();
    }

    void HistogramsViewer_KG::updateHistograms() {
        Slab::Math::RtoR::Histogram histogram;

        if(getFunction() == nullptr || getFunctionDerivative() == nullptr) {
            Slab::Core::Log::Error() << "At " << __PRETTY_FUNCTION__ << ":" << __LINE__ << Slab::Core::Log::Flush;
            return;
        }

        auto harnessed_data = harness();

        histogram.Compute(*harnessed_data.energy, nbins);
        histogram.renderPDFToPointSet(Slab::Naked(histogram_data_energy), pretty);

        histogram.Compute(*harnessed_data.kinetic, nbins);
        histogram.renderPDFToPointSet(Slab::Naked(histogram_data_kinetic), pretty);

        histogram.Compute(*harnessed_data.gradient, nbins);
        histogram.renderPDFToPointSet(Slab::Naked(histogram_data_gradient), pretty);

        histogram.Compute(*harnessed_data.potential, nbins);
        histogram.renderPDFToPointSet(Slab::Naked(histogram_data_potential), pretty);

        for(const auto& h_win : histogram_windows)
            h_win->reviewGraphRanges();

    }

    void HistogramsViewer_KG::setFunction(Slab::Pointer<Slab::Math::R2toR::NumericFunction> function) {
        auto domain = function->getDomain();

        t_min = domain.yMin;
        t_delta = domain.yMax-t_min;

        Viewer::setFunction(function);
    }

    void HistogramsViewer_KG::setFunctionDerivative(FuncPointer pointer) {
        KGViewer::setFunctionDerivative(pointer);

        if(isVisible() && areFunctionsConsistent()) updateHistograms();
    }

    HistogramsViewer_KG::HarnessData
    HistogramsViewer_KG::harness() {
        auto f = getFunction();

        fix N = f->getSpace().getMetaData().getN(0);
        fix M = f->getSpace().getMetaData().getN(1);
        fix dt = f->getDomain().getLy() / (M-1);
        fix t0 = f->getDomain().yMin;

        fix j_begin = (size_t)floor((t_min-t0) / dt);
        fix j_end   = (size_t)floor((t_min+t_delta-t0) / dt);

        sheer_size = (j_end-j_begin+1) * f->getN();

        HistogramsViewer_KG::HarnessData data { sheer_size };

        auto Slicer = Slab::Math::RtoR::FromR2toR;
        auto laplacian_type = Math::RtoR::NumericFunction::Standard1D_PeriodicBorder;

        int k = 0;
        for(int j=j_begin; j<=j_end; ++j) {
            auto phi = Slicer(getFunction(), j, laplacian_type);
            auto ddt_phi = Slicer(getFunctionDerivative(), j, laplacian_type);

            if(phi==nullptr || ddt_phi==nullptr) {
                Slab::Core::Log::Error() << "At " << __PRETTY_FUNCTION__ << ":" << __LINE__ << Slab::Core::Log::Flush;
                return data;
            }

            auto hamiltonian = getHamiltonian();

            hamiltonian.computeEnergies(*phi, *ddt_phi);

            const auto *energy    = &hamiltonian.getEnergyDensity()   ->getSpace().getHostData(true)[0];
            const auto *kinetic   = &hamiltonian.getKineticDensity()  ->getSpace().getHostData(true)[0];
            const auto *grad      = &hamiltonian.getGradientDensity() ->getSpace().getHostData(true)[0];
            const auto *potential = &hamiltonian.getPotentialDensity()->getSpace().getHostData(true)[0];

            auto *energy_data    = &(*data.energy)   [k];
            auto *kinetic_data   = &(*data.kinetic)  [k];
            auto *grad_data      = &(*data.gradient) [k];
            auto *potential_data = &(*data.potential)[k];

            memcpy(energy_data,    energy,    N*sizeof(Slab::Real));
            memcpy(kinetic_data,   kinetic,   N*sizeof(Slab::Real));
            memcpy(grad_data,      grad,      N*sizeof(Slab::Real));
            memcpy(potential_data, potential, N*sizeof(Slab::Real));

            k+=N;
        }

        return data;
    }

} // Studios::Fields::Viewers