import sys
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QMainWindow, QApplication
from gui3 import Ui_MainWindow
from vtk.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
import vtk


dataDir = None
surfaceExtractor = vtk.vtkContourFilter()

 
def slider_SLOT(val):
    surfaceExtractor.SetValue(0, val)
    iren.update()
    

ui = Ui_MainWindow()


class AppWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        # ui = Ui_MainWindow()
        ui.setupUi(self)
        ui.actionload.triggered.connect(open_folder)
        ui.pushButton.clicked.connect(open_folder)

        ui.comboBox.currentIndexChanged.connect(vtk_rendering)

        ui.horizontalSlider.valueChanged.connect(slider_SLOT)
        ui.horizontalSlider_2.valueChanged.connect(vtk_rendering)
        ui.horizontalSlider_3.valueChanged.connect(vtk_rendering)
        ui.horizontalSlider_4.valueChanged.connect(vtk_rendering)

        self.show() 



 
def open_folder():

    global dataDir
    options =  QtWidgets.QFileDialog.Options()
    dataDir = QtWidgets.QFileDialog.getExistingDirectory(caption = "Select Dataset Folder",options=options)

    if dataDir != '': #Directory Specified

        if ui.comboBox.currentIndex == 0: #Surface rendering selected

            ui.horizontalSlider.setValue(500); ui.label_2.setNum(500)
            slider_SLOT(500)


        else: #Ray casting rendering selected

            ui.horizontalSlider_2.setValue(10); ui.label_5.setNum(10)
            ui.horizontalSlider_3.setValue(5) ; ui.label_7.setNum(5)
            ui.horizontalSlider_4.setValue(3); ui.label_9.setNum(3)
            iren.update()


        vtk_rendering()
    else:
        pass



def vtk_rendering():

    renWin = iren.GetRenderWindow()
    aRenderer = vtk.vtkRenderer()
    renWin.AddRenderer(aRenderer)
    

    reader = vtk.vtkDICOMImageReader()
    reader.SetDirectoryName(dataDir)
    reader.Update()


    if ui.comboBox.currentIndex() == 0: #Surface rendering selected

        surfaceExtractor.SetInputConnection(reader.GetOutputPort())
        surfaceExtractor.SetValue(0, 500)
        surfaceNormals = vtk.vtkPolyDataNormals()
        surfaceNormals.SetInputConnection(surfaceExtractor.GetOutputPort())
        surfaceNormals.SetFeatureAngle(60.0)

        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputConnection(surfaceNormals.GetOutputPort())
        surfaceMapper.ScalarVisibilityOff()
        surface = vtk.vtkActor()
        surface.SetMapper(surfaceMapper)

        aRenderer.AddActor(surface)
        
# ********************************************************* 

    else: #Ray casting rendering selected
        volumeMapper = vtk.vtkGPUVolumeRayCastMapper()
        volumeMapper.SetInputConnection(reader.GetOutputPort())
        volumeMapper.SetBlendModeToComposite()

        volumeColor = vtk.vtkColorTransferFunction()
        volumeColor.AddRGBPoint(0,    0.0, 0.0, 0.0)
        volumeColor.AddRGBPoint(500,  ui.horizontalSlider_2.value()/10, ui.horizontalSlider_3.value()/10, ui.horizontalSlider_4.value()/10)
        volumeColor.AddRGBPoint(1000, ui.horizontalSlider_2.value()/10, ui.horizontalSlider_3.value()/10, ui.horizontalSlider_4.value()/10)
        volumeColor.AddRGBPoint(1150, ui.horizontalSlider_2.value()/10, 2 * ui.horizontalSlider_3.value()/10, 2 * ui.horizontalSlider_4.value()/10)

        volumeScalarOpacity = vtk.vtkPiecewiseFunction()
        volumeScalarOpacity.AddPoint(0,    0.00)
        volumeScalarOpacity.AddPoint(500,  0.15)
        volumeScalarOpacity.AddPoint(1000, 0.15)
        volumeScalarOpacity.AddPoint(1150, 0.85)

        volumeGradientOpacity = vtk.vtkPiecewiseFunction()
        volumeGradientOpacity.AddPoint(0,   0.0)
        volumeGradientOpacity.AddPoint(90,  0.5)
        volumeGradientOpacity.AddPoint(100, 1.0)

        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetColor(volumeColor)
        volumeProperty.SetScalarOpacity(volumeScalarOpacity)
        volumeProperty.SetGradientOpacity(volumeGradientOpacity)
        volumeProperty.SetInterpolationTypeToLinear()
        volumeProperty.ShadeOn()
        volumeProperty.SetAmbient(0.4)
        volumeProperty.SetDiffuse(0.6)
        volumeProperty.SetSpecular(0.2)

        volume = vtk.vtkVolume()
        volume.SetMapper(volumeMapper)
        volume.SetProperty(volumeProperty)

        aRenderer.AddViewProp(volume)

# *****************************************



    aCamera = vtk.vtkCamera()
    aCamera.SetViewUp(0, 0, -1)
    aCamera.SetPosition(0, 1, 0)
    aCamera.SetFocalPoint(0, 0, 0)
    aCamera.ComputeViewPlaneNormal()
    
    # aRenderer.AddActor(surface)
    aRenderer.SetActiveCamera(aCamera)
    aRenderer.ResetCamera()
    
    aRenderer.SetBackground(0, 0, 0)
    
    aRenderer.ResetCameraClippingRange()
    
    # Interact with the data.
    iren.Initialize()
    renWin.Render()
    iren.Start()
    iren.show()
 

app = QApplication(sys.argv)
# The class that connect Qt with VTK
iren = QVTKRenderWindowInteractor()
w = AppWindow()
open_folder()
# vtk_rendering()
w.show()
sys.exit(app.exec_())
# Start the event loop.
