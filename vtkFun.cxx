#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCommand.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkSmartPointer.h"
#include <iostream>
#include <cmath>

class SphereContainer : public vtkCommand {
private:
	vtkSmartPointer<vtkSphereSource> sphere;
	vtkSmartPointer<vtkRenderWindowInteractor> viewer;
 	double a,v;
public:
	SphereContainer() : a(0.001), v(0.01)
	{
		sphere = vtkSphereSource::New();
		sphere->SetThetaResolution(100);
		sphere->SetPhiResolution(50);
		sphere->SetCenter(0,1,0);
	}

	vtkSphereSource *GetSphere() {
		return sphere;
	}

	void SetViewer(vtkSmartPointer<vtkRenderWindowInteractor> viewer) {
		this->viewer = viewer;
	}

	virtual void Execute(vtkObject *caller, 
		             unsigned long eventId,
			     void *callData)
	{
		double center[3] = { 0.0, 0.0, 0.0 };
		sphere->GetCenter(center);
		if(center[1] >= -1.5) {
			center[1] -= v;
		} else if(center[0] < 1.5) {
			center[0] += v;
		}
		v += a;
		sphere->SetCenter(center);
		viewer->Render();
	}
};

int main(int argc, char **argv)
{
	SphereContainer *sc = new SphereContainer();
	
	vtkSmartPointer<vtkCubeSource> cube = vtkCubeSource::New();

	vtkPolyDataMapper *sphereMapper = vtkPolyDataMapper::New();
	sphereMapper->SetInputConnection(sc->GetSphere()->GetOutputPort());

	vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
	cubeMapper->SetInputConnection(cube->GetOutputPort());

	vtkActor *sphere1 = vtkActor::New();
	sphere1->SetMapper(sphereMapper);
	sphere1->GetProperty()->SetColor(1,0,0);
	sphere1->GetProperty()->SetAmbient(0.3);
	sphere1->GetProperty()->SetDiffuse(0.4);
	sphere1->GetProperty()->SetSpecular(0.0);
	sphere1->GetProperty()->SetSpecularPower(5.0);

	vtkActor *cube1 = vtkActor::New();
	cube1->SetMapper(cubeMapper);
	cube1->GetProperty()->SetColor(0,0.7,0);
	cube1->GetProperty()->SetDiffuse(0.4);
	cube1->GetProperty()->SetAmbient(0.3);
	cube1->GetProperty()->SetEdgeVisibility(1);
	cube1->GetProperty()->SetEdgeColor(0,0.8,0);
	cube1->AddPosition(1.25,0,0);

	vtkRenderer *ren1 = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren1);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	vtkInteractorStyleSwitch *switchStyle = (vtkInteractorStyleSwitch *)iren->GetInteractorStyle();
	switchStyle->SetCurrentStyleToTrackballCamera();
	sc->SetViewer(iren);

	ren1->AddActor(sphere1);
	ren1->AddActor(cube1);
	ren1->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(400, 400);
	// Set up the lighting.
	//
	vtkLight *light = vtkLight::New();
	light->SetFocalPoint(1.875,0.6125,0);
	light->SetPosition(0.875,1.6125,1);
	ren1->AddLight(light);

	// We want to eliminate perspective effects on the apparent lighting.
	// Parallel camera projection will be used. To zoom in parallel projection
	// mode, the ParallelScale is set.
	//
	ren1->GetActiveCamera()->SetFocalPoint(0,0,0);
	ren1->GetActiveCamera()->SetPosition(0,0,1);
	ren1->GetActiveCamera()->SetViewUp(0,1,0);
	ren1->GetActiveCamera()->ParallelProjectionOn();
	ren1->ResetCamera();
	ren1->GetActiveCamera()->SetParallelScale(1.5);
	  
	// This starts the event loop and invokes an initial render.
	//
	iren->Initialize();
	iren->AddObserver(vtkCommand::TimerEvent, sc); 
	iren->CreateRepeatingTimer(100.0);
	iren->Start();

	return 0;
}
