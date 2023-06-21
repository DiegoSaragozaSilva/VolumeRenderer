from PIL import Image, ImageFile
import pydicom
import glob

import vtk
import numpy as np

ImageFile.LOAD_TRUNCATED_IMAGES = True

def load_volume_files(folder_path : str) -> np.array:
    # Volume image reading
    image_files = [image_file for image_file in glob.glob(folder_path)]
    image_files.sort()

    # Querying volume basic informations
    temp_image = Image.open(image_files[0])
    depth, width = temp_image.size
    height = len(image_files)
    print(f"Volume data: {width}W {height}H {depth}D")

    # Store volume in memory
    k = 0
    volume = np.zeros((width, depth, height), dtype = np.uint16)
    for image_file in image_files:
        temp_image = Image.open(image_file)
        volume[:, :, k] = np.array(temp_image)
        k += 1
    
    return volume

def load_volume_files_dicom(folder_path : str) -> np.array:
    # Volume image reading
    image_files = [image_file for image_file in glob.glob(folder_path)]
    image_files.sort()

    # Querying volume basic informations
    temp_image = pydicom.dcmread(image_files[0]).pixel_array
    depth, width = temp_image.shape[0], temp_image.shape[1]
    height = len(image_files)
    print(f"Volume data: {width}W {height}H {depth}D")

    # Store volume in memory
    k = 0
    volume = np.zeros((width, depth, height), dtype = np.uint16)
    for image_file in image_files:
        temp_image = pydicom.dcmread(image_file).pixel_array
        volume[:, :, k] = np.array(temp_image)
        k += 1
    
    return volume

def exit_check(object, event):
    if object.GetEventPending() != 0:
        object.SetAbortRender(1)

# Load volume data
volume_data = load_volume_files_dicom("../build/assets/volumes/CTHuman/*.dcm")
width, depth, height = volume_data.shape

# Transform volume data to VTK-image slices
data_importer = vtk.vtkImageImport()
data_importer.CopyImportVoidPointer(volume_data.tobytes(), len(volume_data.tobytes()))
data_importer.SetDataScalarTypeToUnsignedShort()
data_importer.SetNumberOfScalarComponents(1)
data_importer.SetDataExtent(0, height - 1, 0, depth - 1, 0, width - 1)
data_importer.SetWholeExtent(0, height - 1, 0, depth - 1, 0, width - 1)

# Create the vtk color functions
color_function = vtk.vtkColorTransferFunction()
color_function.AddRGBPoint(160, 0.0, 0.0, 0.0)
color_function.AddRGBPoint(255, 1.0, 1.0, 1.0)

alpha_function = vtk.vtkPiecewiseFunction()
alpha_function.AddPoint(160, 0)
alpha_function.AddPoint(187.783, 0.06875)
alpha_function.AddPoint(255, 0)

# Create the vtk volume property
volume_property = vtk.vtkVolumeProperty()
volume_property.SetColor(color_function)
volume_property.SetScalarOpacity(alpha_function)
volume_property.SetInterpolationTypeToLinear()

# Create the vtk volume mapper
volume_mapper = vtk.vtkGPUVolumeRayCastMapper()
volume_mapper.SetMaximumImageSampleDistance(0.01)
volume_mapper.SetInputConnection(data_importer.GetOutputPort())
volume_mapper.SetBlendModeToComposite();

# Create the vtk volume
volume = vtk.vtkVolume()
volume.SetMapper(volume_mapper)
volume.SetProperty(volume_property)

# Create the vtk renderer
renderer = vtk.vtkRenderer()
renderer_window = vtk.vtkRenderWindow()
renderer_window.AddRenderer(renderer)
renderer_interactor = vtk.vtkRenderWindowInteractor()
renderer_interactor.SetRenderWindow(renderer_window)

# Add the volume to the renderer and configure the enviroment
renderer.AddVolume(volume)
renderer.SetBackground(1, 1, 1)
renderer_window.SetSize(800, 600)
renderer_window.SetMultiSamples(4)
renderer_window.AddObserver("AbortCheckEvent", exit_check)
renderer_interactor.Initialize()

# Render
renderer.Render()
renderer_interactor.Start()
