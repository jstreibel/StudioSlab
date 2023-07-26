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
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkDelaunay2D.h>
#include <vtkOrientationMarkerWidget.h>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

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
        auto renderer = vtkPtrNew(vtkRenderer);
        auto renderWindow = vtkPtrNew(vtkRenderWindow);
        auto renderWindowInteractor = vtkPtrNew(vtkRenderWindowInteractor);

        renderer->SetBackground(1,1,1);
        renderWindow->AddRenderer(renderer);

        // Create a vtkPoints object and reserve space for N*N points
        auto points = vtkPtrNew(vtkPoints);

        // Create a vtkFloatArray to store the height values
        auto heights = vtkPtrNew(vtkFloatArray);
        heights->SetNumberOfValues(outN * outN);

        {
            auto &phi = packet.getEqStateData<R2toR::EquationState>()->getPhi();
            phi.getSpace().syncHost();

            auto L = params.getL();
            auto xMin = params.getxLeft();


            // Fill the points and heights with data from your matrix
            for (int i = 0; i < outN; ++i) {
                for (int j = 0; j < outN; ++j) {
                    auto x = xMin + i * L / outN;
                    auto y = xMin + j * L / outN;

                    double height = phi({x, y});

                    points->InsertNextPoint(x, y, height);
                    heights->SetValue(i * outN + j, height);
                }
            }
        }

        // Apply the transform to the polydata
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        auto data = transformFilter->GetOutput();
        // Create a vtkPolyData object and set the points and scalars
        auto polydata = vtkPtrNew(vtkPolyData);
        polydata->SetPoints(points);
        polydata->GetPointData()->SetScalars(heights);

        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Scale(1.0, 1.0, 4.0); // Replace zScale with the desired scale factor for the 'z' axis

        transformFilter->SetInputData(polydata);
        transformFilter->SetTransform(transform);
        transformFilter->Update();


        // Use Delaunay2D to create a surface from the points
        auto delaunay = vtkPtrNew(vtkDelaunay2D);
        delaunay->SetInputData(data);

        // Create a mapper and actor
        auto mapper = vtkPtrNew(vtkPolyDataMapper);
        mapper->SetInputConnection(delaunay->GetOutputPort());

        auto fieldActor = vtkPtrNew(vtkActor);
        fieldActor->SetMapper(mapper);

        // Add the actor to the renderer
        renderer->AddActor(fieldActor);

        {
            // Create a cube axes actor
            vtkSmartPointer<vtkCubeAxesActor> cubeAxes = vtkSmartPointer<vtkCubeAxesActor>::New();
            cubeAxes->SetBounds(data->GetBounds());
            cubeAxes->SetCamera(renderer->GetActiveCamera());
            cubeAxes->DrawXGridlinesOn();
            cubeAxes->DrawYGridlinesOn();;
            cubeAxes->DrawZGridlinesOn();
            cubeAxes->SetZTitle("phi");

            //cubeAxes->SetZAxisRange(-0.5, 0.7);
            cubeAxes->SetTickLocationToOutside();
            cubeAxes->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);

            auto r = .0,
                    g = .0,
                    b = .0;

            cubeAxes->GetTitleTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetTitleTextProperty(0)->SetFontSize(1);
            cubeAxes->GetTitleTextProperty(1)->SetColor(r, g, b);
            cubeAxes->GetTitleTextProperty(2)->SetColor(r, g, b);

            cubeAxes->GetLabelTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(0)->SetFontSize(1);
            cubeAxes->GetLabelTextProperty(1)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(2)->SetColor(r, g, b);

            cubeAxes->GetXAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetYAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetZAxesLinesProperty()->SetColor(r, g, b);

            renderer->AddActor(cubeAxes);
        }


        // Create a scalar bar
        vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(mapper->GetLookupTable());
        scalarBar->SetWidth(0.1);
        scalarBar->SetHeight(0.8);
        scalarBar->SetPosition(0.9, 0.1);
        //scalarBar->SetTitle("phi");
        scalarBar->SetNumberOfLabels(5);

        scalarBar->GetTitleTextProperty()->SetFontSize(1);
        scalarBar->GetTitleTextProperty()->SetColor(0,0,0);
        scalarBar->GetTitleTextProperty()->SetItalic(false);
        scalarBar->GetTitleTextProperty()->SetBold(false);
        scalarBar->GetTitleTextProperty()->SetFontFamilyToCourier();

        scalarBar->GetLabelTextProperty()->SetFontSize(1);
        scalarBar->GetLabelTextProperty()->SetColor(0,0,0);
        scalarBar->GetLabelTextProperty()->SetItalic(false);
        scalarBar->GetLabelTextProperty()->SetBold(false);
        scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();

        // Add the scalar bar to the renderer
        renderer->AddActor2D(scalarBar);


        renderWindow->SetSize(1600, 900);

        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->Initialize();
        renderWindowInteractor->Start();

        return true;
    }
} // R2toR

