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
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkDelaunay2D.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkStructuredGrid.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkStructuredGridGeometryFilter.h>

#include <vtkCubeAxesActor.h>

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

    bool showB(const NumericParams &params, const OutputPacket packet, int outN) {
        auto zPassiveScale = 1.5;
        double zMin=10., zMax=-10.;

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
                heights->InsertNextValue(height);
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
            cubeAxes->DrawXGridlinesOn();
            cubeAxes->DrawYGridlinesOn();;
            cubeAxes->DrawZGridlinesOn();
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

