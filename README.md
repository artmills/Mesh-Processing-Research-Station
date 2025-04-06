# Mesh Processing Research Station

This ever-evolving project is where I perform the computational geometry experiments for my PhD research in discrete differential geometry.
Features are added as I need them to assist in my research on curvature measures.
Although OpenMesh or Blender are more practical in terms of real applications, this project gives me a full hands-on approach to methods in computational geometry;
I started this project when I began working on the computational side of my research, which allowed me to learn the structure and basic methods of 3D CAD software.

This project features mesh generation, processing, and rendering of triangle meshes using OpenGL. 

Current Mesh Processing Research Station features:
* Half-edge mesh data structure implemented as a corner list
* Import mesh data from a .obj or .ply file
* Render mesh using the standard modern OpenGL pipeline with optional smooth lighting
* Wireframe visualization using my implementation of Nvidia's wireframe geometry shader: https://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf
* Select vertex of mesh using raycasting
* Loop subdivision 
* Analysis of mesh using a variety of curvature measures: mean, Gaussian, my own
* Display lines of curvature computed using my own curvature measures
* Gauss map visualization, including the polar dual of the Gauss map of a vertex star
* Procedural terrain generation using a Perlin noise implementation

Previous features that need to be updated and re-added (some are still accessible in previous commits):
* Smoothing using mean curvature flow (and other curvature flows)
* Silhouette and line art rendering
* Morse function computation
* Visualization analysis using depth peeling

Future features (unrelated to my research):
* Native screenshot function
* Save current mesh data, such as after subdivision
* Display multiple meshes at once
* More robust camera movement

![Screenshot_select-area_20250406153729](https://github.com/user-attachments/assets/973b7cdb-817b-4d5c-8cbb-9ed65d3ee88e)
