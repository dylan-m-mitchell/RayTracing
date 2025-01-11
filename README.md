# RayTracing Project

## Overview

This project is a simple ray tracing application that renders a 3D scene using the Walnut framework. The application 
    includes a graphical user interface (GUI) for interacting with the rendering settings.

## Features

- Ray tracing rendering
- Adjustable rendering settings via GUI
- Real-time updates

## Getting Started

### Prerequisites

- Visual Studio 2022
- C++17 compatible compiler
- Walnut framework

### Building the Project

1. Clone the repository.
   
2. Open the solution file `RayTracing.sln` in Visual Studio 2022.

3. Build the solution by selecting `Build > Build Solution` from the menu.

### Running the Application

1. After building the solution, run the application by selecting `Debug > Start Debugging` or pressing `F5`.

2. The application window will open, displaying the rendered scene and the GUI.

## GUI Overview

The GUI is built using the ImGui library and provides controls for interacting with the rendering settings.

### Main Components

- **Settings Window**: This window contains various controls for adjusting the rendering settings.
  - **Last Render Time**: Displays the time taken for the last render in milliseconds.
  - **Render Button**: Triggers a new render.
  - **Sphere Color**: Allows the user to adjust the color of the rendered sphere.

### ExampleLayer Class

The `ExampleLayer` class is responsible for rendering the GUI. It inherits from `Walnut::Layer` and overrides 
    the `OnUIRender` method to create the GUI elements.

### Renderer Class

The `Renderer` class handles the actual rendering of the scene. It provides methods for resizing the render 
    target and rendering the scene with different settings.




    
