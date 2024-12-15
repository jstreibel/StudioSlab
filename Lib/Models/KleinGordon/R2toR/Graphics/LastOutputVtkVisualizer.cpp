//
// Created by joao on 7/25/23.
//

#include "LastOutputVtkVisualizer.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Graphics/Styles/Colors.h"

#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkStructuredPoints.h>
#include <vtk/vtkFloatArray.h>
#include <vtk/vtkActor.h>

#include <vtk/vtkProperty.h>
#include <vtk/vtkTextProperty.h>

#include <vtk/vtkRenderer.h>
#include <vtk/vtkRenderWindow.h>
#include <vtk/vtkRenderWindowInteractor.h>

#include <vtk/vtkLookupTable.h>
#include <vtk/vtkPoints.h>
#include <vtk/vtkPointData.h>
#include <vtk/vtkCellData.h>
#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkScalarBarActor.h>
#include <vtk/vtkOrientationMarkerWidget.h>
#include <vtk/vtkStructuredGrid.h>
#include <vtk/vtkTransform.h>
#include <vtk/vtkStructuredGridGeometryFilter.h>

#include <vtk/vtkCubeAxesActor.h>
#include <vtk/vtkColorTransferFunction.h>

#define vtkPtrNew(ofClass) vtkSmartPointer<ofClass>::New()

namespace Slab::Models::KGR2toR {

    using namespace Core;

    LastOutputVTKVisualizer::LastOutputVTKVisualizer(Count total_steps, int outN)
    : Math::Socket("LastOutVTKViz", -1, "VTK visualization of the last simulation output.")
    , outN(outN) {

    }

    bool showA();
    bool showB(const OutputPacket& packet, int outN);


    bool LastOutputVTKVisualizer::notifyIntegrationHasFinished(const OutputPacket &lastOut) {
        Log::Info("LastOutputVTKVisualizer") << " will now output;" << Log::Flush;
        return showB(lastOut, outN);
    }

    Real logAbs(Real val, Real eps){
        const auto sign = (val>.0?1.0:-1.0);
        return log(abs(val)/eps + 1)*sign;
    }

    Real logAbs_inv(Real val, Real eps){
        const auto sign = (val>.0?1.0:-1.0);
        return eps * (exp(abs(val)) - 1.0) * sign;
    }

    bool showB(const OutputPacket& packet, int outN) {
        auto zPassiveScale = 2.0;
        double zMin=10., zMax=-10.;
        const auto eps_log = 1.2-2;

        // Create a vtkPoints object and reserve space for N*N points
        auto points = vtkPtrNew(vtkPoints);

        // Create a vtkFloatArray to store the height values
        auto heights = vtkPtrNew(vtkFloatArray);
        heights->SetNumberOfComponents(1);

        {
            auto &phi = packet.GetNakedStateData<R2toR::EquationState>()->getPhi();
            phi.getSpace().syncHost();

            fix h = phi.getSpace().getMetaData().geth(0);
            fix N = phi.getSpace().getMetaData().getN(0);
            auto L = h*N;
            auto xMin = .5*L;
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

                if(false) {
                    auto logHeight = logAbs(height, eps_log);

                    heights->InsertNextValue(logHeight);
                } else {
                    heights->InsertNextValue(abs(height));
                }
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
        if(true) {
            using namespace std;

            #define hexColor(r,g,b) vector {(r)/255., (g)/255., (b)/255.}
                                    /*                   R    G    B   */
            auto blues   = vector { /* -1.000 */hexColor(240, 255, 255),
                                    /* -0.875 */hexColor(223, 247, 255),
                                    /* -0.750 */hexColor(207, 240, 255),
                                    /* -0.625 */hexColor(192, 233, 255),
                                    /* -0.500 */hexColor(175, 225, 255),
                                    /* -0.375 */hexColor(165, 220, 253),
                                    /* -0.250 */hexColor(155, 215, 252),
                                    /* -0.125 */hexColor(144, 210, 251),
                                    /*  0.000 */hexColor(133, 205, 249),
                                    /*  0.125 */hexColor(100, 179, 221),
                                    /*  0.250 */hexColor( 66, 154, 193),
                                    /*  0.375 */hexColor( 32, 128, 165),
                                    /*  0.500 */hexColor(  0, 103, 139),
                                    /*  0.625 */hexColor( 10,  93, 139),
                                    /*  0.750 */hexColor( 20,  83, 139),
                                    /*  0.875 */hexColor( 29,  74, 139),
                                    /*  1.000 */hexColor( 40,  64, 139)};

            auto BrBG    = vector { vector{0.0, 0.23529411764705882, 0.18823529411764706},
                                    vector{0.9572472126105345, 0.9599384851980008, 0.9595540176855056},
                                    vector{0.32941176470588235, 0.18823529411764706, 0.0196078431372549}};

            auto rainbow = vector { vector /* Red:    */ {1.0, 0.0, 0.0},
                                    vector /* Orange: */ {1.0, 0.498, 0.0},
                                    vector /* Yellow: */ {1.0, 1.0, 0.0},
                                    vector /* Green:  */ {0.0, 1.0, 0.0},
                                    vector /* Cyan  : */ { .0, 1.0, 1.0},
                                    vector /* Blue    */ { .0,  .0, 1.0}};

            auto table = blues;
            auto len = table.size();

            const bool symmetric = false;

            auto lut = vtkPtrNew(vtkColorTransferFunction);
            auto _zmax = zMax;
            auto _zmin = zMin;
            if(symmetric){
                _zmax = max(abs(_zmax), abs(_zmin));
                _zmin = -_zmax;
            }
            auto deltaz = (_zmax-_zmin) / (table.size() - 1);

            for(int i=0; i<len; ++i){
                auto x = _zmin + i*deltaz;
                auto c = table[i];
                lut->AddRGBPoint(x, c[0], c[1], c[2]);
            }

            mapper->SetLookupTable(lut);
        }

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
            cubeAxes->SetLabelScaling(true, 2, 2, 2);

            auto r = .0,
                 g = .0,
                 b = .0;

            cubeAxes->GetXAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(0)->SetFontSize(10);
            cubeAxes->GetTitleTextProperty(0)->SetColor(r, g, b);
            cubeAxes->GetTitleTextProperty(0)->SetFontSize(10);

            cubeAxes->GetYAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(1)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(1)->SetFontSize(10);
            cubeAxes->GetTitleTextProperty(1)->SetColor(r, g, b);

            cubeAxes->GetZAxesLinesProperty()->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(2)->SetColor(r, g, b);
            cubeAxes->GetLabelTextProperty(2)->SetFontSize(10);
            cubeAxes->GetTitleTextProperty(2)->SetColor(r, g, b);

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

