//
// Created by joao on 7/25/23.
//

#include "LastOutputVtkVisualizer.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Graphics/Styles/Colors.h"

#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkFloatArray.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkStringArray.h>

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

namespace Slab::Models::KGR2toR {

    using namespace Core;

    LastOutputVTKVisualizer::LastOutputVTKVisualizer(Count total_steps, int outN)
    : Math::Socket("LastOutVTKViz", -1, "VTK visualization of the last simulation output.")
    , outN(outN) {

    }

    void test();
    bool show(const OutputPacket& packet, int outN);
    bool show2(const OutputPacket& packet, int outN);

    LastOutputVTKVisualizer::~LastOutputVTKVisualizer() {
        // We output from destructor because it is called on the main thread.
        // show(lastPacket, outN);
        // test();
    }

    bool LastOutputVTKVisualizer::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);

        // show(theVeryLastOutputInformation, outN);
        show2(theVeryLastOutputInformation, outN);

        return true;
    }

    DevFloat logAbs(DevFloat val, DevFloat eps){
        const auto sign = (val>.0?1.0:-1.0);
        return log(abs(val)/eps + 1)*sign;
    }

    DevFloat logAbs_inv(DevFloat val, DevFloat eps){
        const auto sign = (val>.0?1.0:-1.0);
        return eps * (exp(abs(val)) - 1.0) * sign;
    }

    bool show(const OutputPacket& packet, int outN) {
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
                // if(col==0) continue;

                if(false) {
                    auto logHeight = logAbs(height, eps_log);

                    heights->InsertNextValue(logHeight);
                } else {
                    heights->InsertNextValue((float)abs(height));
                }
            }
        }

        auto geometryFilter = vtkPtrNew(vtkStructuredGridGeometryFilter);
        {
            auto structuredGrid = vtkPtrNew(vtkStructuredGrid);
            structuredGrid->SetDimensions(outN, outN, 1);
            structuredGrid->SetPoints(points);
            // structuredGrid->GetCellData()->SetScalars(heights);
            structuredGrid->GetPointData()->SetScalars(heights);

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

            const auto& table = blues;
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
                const auto& c = table[i];
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

        // auto camera = renderer->GetActiveCamera();
        // camera->SetPosition(0,-3, 2);
        // camera->SetFocalPoint(0,0,0);

        auto renderWindow = vtkPtrNew(vtkRenderWindow);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(1600, 900);
        // renderWindow->InitializeFromCurrentContext();
        // renderWindow->Render();

        auto renderWindowInteractor = vtkPtrNew(vtkRenderWindowInteractor);
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->Initialize();
        renderWindowInteractor->Start();

        // renderWindow->Start();
        // renderWindow->Render();

        return true;
    }

    bool show2(const OutputPacket &packet, int outN) {
        auto &phi = packet.GetNakedStateData<R2toR::EquationState>()->getPhi();
        phi.getSpace().syncHost();

        const auto font_min = (int)(20*1.5);
        const auto font_max = (int)(24*1.5);
        const double zPassiveScale = 2.0;
        double zMin = 10.0, zMax = -10.0;
        const double eps_log = 1.e-1; // -0.8 just as in your code

        // Create a vtkPoints object
        auto points = vtkPtrNew(vtkPoints);

        // Create a vtkFloatArray for heights
        auto heights = vtkPtrNew(vtkFloatArray);
        heights->SetNumberOfComponents(1);

        // Set domain parameters (example values)
        fix h = phi.getSpace().getMetaData().geth(0);
        fix N = phi.getSpace().getMetaData().getN(0);
        auto L = h*N;
        double xMin = -.5*L;
        double dx = L/(double)outN;

        // Fill the points and heights arrays
        for (int i = 0; i < outN*outN; ++i) {
            int row = i/outN;
            int col = i%outN;

            double x = xMin + row*dx;
            double y = xMin + col*dx;

            double height = phi({x, y});

            if(height < zMin) zMin = height;
            if(height > zMax) zMax = height;

            // Insert point and corresponding scalar
            points->InsertNextPoint(x, y, zPassiveScale*height);

            // If you prefer log mapping, uncomment and adjust accordingly:
            // double logHeight = logAbs(height, eps_log);
            // heights->InsertNextValue((float)logHeight);

            // For absolute value mapping as in your original code:
            heights->InsertNextValue((float)std::abs(height));
        }

        // Build the structured grid
        auto structuredGrid = vtkPtrNew(vtkStructuredGrid);
        structuredGrid->SetDimensions(outN, outN, 1);
        structuredGrid->SetPoints(points);
        // Use point data instead of cell data
        structuredGrid->GetPointData()->SetScalars(heights);

        // Filter geometry
        auto geometryFilter = vtkPtrNew(vtkStructuredGridGeometryFilter);
        geometryFilter->SetInputData(structuredGrid);
        geometryFilter->Update();

        // Create a mapper and set the output of geometry filter
        auto mapper = vtkPtrNew(vtkPolyDataMapper);
        mapper->SetInputConnection(geometryFilter->GetOutputPort());

        // Create a color transfer function (same logic as before)
        {
            using namespace std;
#define hexColor(r,g,b) vector {(r)/255., (g)/255., (b)/255.}

            // Same color table as in your code
            auto blues = vector {
                    /* -1.000 */hexColor(240, 255, 255),
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
                    /*  1.000 */hexColor( 40,  64, 139)
            };

            auto BrBG    = vector { vector{0.0, 0.23529411764705882, 0.18823529411764706},
                                    vector{0.9572472126105345, 0.9599384851980008, 0.9595540176855056},
                                    vector{0.32941176470588235, 0.18823529411764706, 0.0196078431372549}};

            const auto& table = blues;
            auto len = (int)table.size();

            const bool symmetric = false;
            auto _zmax = zMax;
            auto _zmin = zMin;
            if(symmetric){
                _zmax = max(abs(_zmax), abs(_zmin));
                _zmin = -_zmax;
            }

            double deltaz = (_zmax - _zmin)/(table.size()-1);

            auto lut = vtkPtrNew(vtkColorTransferFunction);
            for (int i = 0; i < len; ++i) {
                auto x = _zmin + i*deltaz;
                const auto& c = table[i];
                lut->AddRGBPoint(x, c[0], c[1], c[2]);
            }

            mapper->SetLookupTable(lut);
            mapper->UseLookupTableScalarRangeOn();
        }

        // Create an actor for the field
        auto fieldActor = vtkPtrNew(vtkActor);
        fieldActor->SetMapper(mapper);

        // Create cube axes
        auto xtraSize = 1.0;
        auto cubeAxes = vtkPtrNew(vtkCubeAxesActor);
        auto bounds = geometryFilter->GetOutput()->GetBounds();
        bounds[0] *= xtraSize;
        bounds[1] *= xtraSize;
        bounds[2] *= xtraSize;
        cubeAxes->SetBounds(bounds);
        cubeAxes->DrawXGridlinesOff();
        cubeAxes->DrawYGridlinesOff();
        cubeAxes->DrawZGridlinesOff();
        cubeAxes->SetZTitle("");
        cubeAxes->SetZAxisRange(zMin, zMax);
        cubeAxes->SetTickLocationToOutside();
        // cubeAxes->SetTickLocationToBoth();
        // cubeAxes->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);
        cubeAxes->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_ALL);
        cubeAxes->SetLabelScaling(true, 2.0, 2.0, 2.0);

        auto xy_labels = vtkStringArray::New();
        xy_labels->InsertNextValue(ToStr(-.5*L*xtraSize, 0));
        xy_labels->InsertNextValue("0");
        xy_labels->InsertNextValue(ToStr( .5*L*xtraSize, 0));
        cubeAxes->SetAxisLabels(0, xy_labels);
        cubeAxes->SetAxisLabels(1, xy_labels);

        auto z_labels = vtkStringArray::New();
        z_labels->InsertNextValue(ToStr(zMin*xtraSize, 2, true));
        z_labels->InsertNextValue("0");
        z_labels->InsertNextValue(ToStr(zMax*xtraSize, 2, true));
        cubeAxes->SetAxisLabels(2, z_labels);

        cubeAxes->SetUse2DMode(true);

        // Set cube axes font size
        cubeAxes->GetTitleTextProperty(0)->SetFontSize(font_max);
        cubeAxes->GetLabelTextProperty(0)->SetFontSize(font_min);
        cubeAxes->GetTitleTextProperty(1)->SetFontSize(font_max);
        cubeAxes->GetLabelTextProperty(1)->SetFontSize(font_min);
        cubeAxes->GetTitleTextProperty(2)->SetFontSize(font_max);
        cubeAxes->GetLabelTextProperty(2)->SetFontSize(font_min);

        // Just setting axis colors/text:
        double r = 0.0, g = 0.0, b = 0.0;
        cubeAxes->GetXAxesLinesProperty()->SetColor(r, g, b);
        cubeAxes->GetYAxesLinesProperty()->SetColor(r, g, b);
        cubeAxes->GetZAxesLinesProperty()->SetColor(r, g, b);

        cubeAxes->GetLabelTextProperty(0)->SetColor(r, g, b);
        cubeAxes->GetTitleTextProperty(0)->SetColor(r, g, b);
        cubeAxes->GetLabelTextProperty(1)->SetColor(r, g, b);
        cubeAxes->GetTitleTextProperty(1)->SetColor(r, g, b);
        cubeAxes->GetLabelTextProperty(2)->SetColor(r, g, b);
        cubeAxes->GetTitleTextProperty(2)->SetColor(r, g, b);

        cubeAxes->SetXTitle("");
        cubeAxes->SetYTitle("");

        // cubeAxes->

        // cubeAxes->SetZAxisVisibility(false);
        // cubeAxes->SetZAxisMinorTickVisibility(false);
        // cubeAxes->SetZAxisLabelVisibility(false);
        // cubeAxes->SetZAxisTickVisibility(false);

        // cubeAxes->SetXAxisTitleOffset(20.0);
        // cubeAxes->SetTitleOffset(100);
        // cubeAxes->SetXTitleOffset(2.0);
        // cubeAxes->SetYTitleOffset(2.0);
        // cubeAxes->SetZTitleOffset(2.0);


        // Create a scalar bar
        auto scalarBar = vtkPtrNew(vtkScalarBarActor);
        scalarBar->SetLookupTable(mapper->GetLookupTable());
        scalarBar->SetWidth(0.1);
        scalarBar->SetHeight(0.8);
        scalarBar->SetPosition(0.80, 0.1);
        scalarBar->SetNumberOfLabels(5);

        scalarBar->GetTitleTextProperty()->SetFontSize(font_min);
        scalarBar->GetTitleTextProperty()->SetColor(0,0,0);
        scalarBar->GetTitleTextProperty()->SetItalic(false);
        scalarBar->GetTitleTextProperty()->SetBold(false);
        scalarBar->SetUnconstrainedFontSize(true);

        scalarBar->GetLabelTextProperty()->SetFontSize(font_max);
        scalarBar->GetLabelTextProperty()->SetColor(0,0,0);
        scalarBar->GetLabelTextProperty()->SetItalic(false);
        scalarBar->GetLabelTextProperty()->SetBold(false);

        // Create renderer and window
        auto renderer = vtkPtrNew(vtkRenderer);
        renderer->SetBackground(1,1,1);
        cubeAxes->SetCamera(renderer->GetActiveCamera());
        renderer->AddActor(fieldActor);
        renderer->AddActor(cubeAxes);
        renderer->AddActor2D(scalarBar);

        // auto camera = renderer->GetActiveCamera();
        // camera->SetPosition(0, -3, 2);
        // camera->SetFocalPoint(0, 0, 0);

        auto renderWindow = vtkPtrNew(vtkRenderWindow);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(1350, 800);

        auto style = vtkPtrNew(vtkInteractorStyleTrackballCamera);
        auto renderWindowInteractor = vtkPtrNew(vtkRenderWindowInteractor);
        renderWindowInteractor->SetInteractorStyle(style);
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->Initialize();
        renderWindowInteractor->Start();

        return true;
    }

} // R2toR

