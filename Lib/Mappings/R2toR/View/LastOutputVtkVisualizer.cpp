//
// Created by joao on 7/25/23.
//

#include "LastOutputVtkVisualizer.h"
#include "Common/Log/Log.h"
#include "Mappings/R2toR/Model/EquationState.h"


#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkFloatArray.h>
#include <vtkActor.h>

#include <vtkProperty.h>
#include <vtkTextProperty.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkLookupTable.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkStructuredGrid.h>
#include <vtkTransform.h>
#include <vtkStructuredGridGeometryFilter.h>

#include <vtkCubeAxesActor.h>
#include <vtkColorTransferFunction.h>

#define vtkPtrNew(ofClass) vtkSmartPointer<ofClass>::New()

namespace R2toR {

    LastOutputVTKVisualizer::LastOutputVTKVisualizer(const NumericParams &params, int outN)
    : Numerics::OutputSystem::Socket(params, "LastOutVTKViz", -1, "VTK visualization of the last simulation output.")
    , outN(outN) {

    }

    bool showA();
    bool showB(const NumericParams &params, const OutputPacket packet, int outN);

    bool LastOutputVTKVisualizer::notifyIntegrationHasFinished(const OutputPacket &lastOut) {
        Log::Info("LastOutputVTKVisualizer") << " will now output;" << Log::Flush;
        return showB(params, lastOut, outN);
    }

    Real logAbs(Real val, Real eps){
        const auto sign = (val>.0?1.0:-1.0);
        return log(abs(val)/eps + 1)*sign;
    }

    Real logAbs_inv(Real val, Real eps){
        const auto sign = (val>.0?1.0:-1.0);
        return eps * (exp(abs(val)) - 1.0) * sign;
    }

    bool showB(const NumericParams &params, const OutputPacket packet, int outN) {
        auto zPassiveScale = 2.0;
        double zMin=10., zMax=-10.;
        const auto eps_log = 7.5e-2;

        // Create a vtkPoints object and reserve space for N*N points
        auto points = vtkPtrNew(vtkPoints);

        // Create a vtkFloatArray to store the height values
        auto heights = vtkPtrNew(vtkFloatArray);
        heights->SetNumberOfComponents(1);

        {
            auto &phi = packet.getEqStateData<R2toR::EquationState>()->getPhi();
            phi.getSpace().syncHost();

            auto L = params.getL();
            auto xMin = params.getxLeft();
            auto dx = L/(double)outN;

            // Fill the points and heights with data from your matrix
            for (int i = 0; i < outN*outN; ++i) {
                auto row = i/outN;
                auto col = i%outN;

                auto x = xMin + row*dx;
                auto y = xMin + col*dx;

                double height = phi({x, y});

                if(height<zMin) zMin = height;
                if(height>zMax) zMax = height;

                points->InsertNextPoint(x, y, zPassiveScale*height);
                if(col==0) continue;

                auto logHeight = logAbs(height, eps_log);

                heights->InsertNextValue(logHeight);
            }
        }

        auto geometryFilter = vtkPtrNew(vtkStructuredGridGeometryFilter);
        {
            auto structuredGrid = vtkPtrNew(vtkStructuredGrid);
            structuredGrid->SetDimensions(outN, outN, 1);
            structuredGrid->SetPoints(points);
            structuredGrid->GetCellData()->SetScalars(heights);

            geometryFilter->SetInputData(structuredGrid);
            geometryFilter->Update();
        }

        auto mapper = vtkPtrNew(vtkPolyDataMapper);
        mapper->SetInputConnection(geometryFilter->GetOutputPort());

        auto fieldActor = vtkPtrNew(vtkActor);
        {
            fieldActor->SetMapper(mapper);
        }

        auto cubeAxes = vtkPtrNew(vtkCubeAxesActor);
        {
            // Create a cube axes actor
            cubeAxes->SetBounds(geometryFilter->GetOutput()->GetBounds());
            cubeAxes->DrawXGridlinesOff();
            cubeAxes->DrawYGridlinesOff();;
            cubeAxes->DrawZGridlinesOff();
            cubeAxes->SetZTitle("phi");

            Log::Info("z (min,max) = (") << zMin << "," << zMax << ")" << Log::Flush;
            cubeAxes->SetZAxisRange(zMin, zMax);
            //cubeAxes->SetZAxisRange(-0.5, 0.7);
            cubeAxes->SetTickLocationToOutside();
            cubeAxes->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);

            auto r = .0,
                 g = .0,
                 b = .0;

            cubeAxes->GetTitleTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetTitleTextProperty(0)->SetFontSize(10);
            cubeAxes->GetTitleTextProperty(1)->SetColor(r, g, b);
            cubeAxes->GetTitleTextProperty(2)->SetColor(r, g, b);

            cubeAxes->GetLabelTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(0)->SetFontSize(10);
            cubeAxes->GetLabelTextProperty(1)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(2)->SetColor(r, g, b);

            cubeAxes->GetXAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetYAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetZAxesLinesProperty()->SetColor(r, g, b);

        }


        // Create a scalar bar
        auto scalarBar = vtkPtrNew(vtkScalarBarActor);
        {
            using namespace std;

            auto BrBG = vector{vector{0.0,                 0.23529411764705882, 0.18823529411764706},
                               vector{0.9572472126105345,  0.9599384851980008,  0.9595540176855056},
                               vector{0.32941176470588235, 0.18823529411764706, 0.0196078431372549}};

            auto lut = vtkPtrNew(vtkColorTransferFunction);
            auto zLims = max(abs(zMax), abs(zMin));
            lut->AddRGBPoint(-zLims, BrBG[0][0], BrBG[0][1], BrBG[0][2]);
            lut->AddRGBPoint(   0.0, BrBG[1][0], BrBG[1][1], BrBG[1][2]);
            lut->AddRGBPoint(+zLims, BrBG[2][0], BrBG[2][1], BrBG[2][2]);

            mapper->SetLookupTable(lut);

            scalarBar->SetLookupTable(mapper->GetLookupTable());
            scalarBar->SetWidth(0.1);
            scalarBar->SetHeight(0.8);
            scalarBar->SetPosition(0.9, 0.1);
            //scalarBar->SetTitle("phi");
            scalarBar->SetNumberOfLabels(5);

            scalarBar->GetTitleTextProperty()->SetFontSize(1);
            scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
            scalarBar->GetTitleTextProperty()->SetItalic(false);
            scalarBar->GetTitleTextProperty()->SetBold(false);
            scalarBar->GetTitleTextProperty()->SetFontFamilyToCourier();

            scalarBar->GetLabelTextProperty()->SetFontSize(1);
            scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
            scalarBar->GetLabelTextProperty()->SetItalic(false);
            scalarBar->GetLabelTextProperty()->SetBold(false);
            scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();

            // Add the scalar bar to the renderer
        }

        auto renderer = vtkPtrNew(vtkRenderer);
        renderer->SetBackground(1,1,1);
        cubeAxes->SetCamera(renderer->GetActiveCamera());
        renderer->AddActor(fieldActor);
        renderer->AddActor(cubeAxes);
        renderer->AddActor2D(scalarBar);

        auto renderWindow = vtkPtrNew(vtkRenderWindow);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(1600, 900);

        auto renderWindowInteractor = vtkPtrNew(vtkRenderWindowInteractor);
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->Initialize();
        renderWindowInteractor->Start();

        return true;
    }
} // R2toR

