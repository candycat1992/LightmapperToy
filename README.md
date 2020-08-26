# LightmapperToy

This project is a toy lightmapper completely based on Houdini VEX nodes. Basically it grew out of a re-implementation of <a href="https://mynameismjp.wordpress.com/2016/10/09/sg-series-part-6-step-into-the-baking-lab/" target="_blank">Matt's C++ Baking Lab</a>.

# How To Use It

## Scene Setup

The project includes three scenes to test different baking methods. 

## Path Tracer

The ground truth path tracing results of the three scenes are listed as follows:

| Cornell | Toy  | Shaderball | Path Tracer |
| --- | ---  | --- | --- |
| <img src="images/ground_truth_cornell.png" width="250"/> | <img src="images/ground_truth_toy.png" width="250"/> | <img src="images/ground_truth_shaderball.png" width="250"/> | Diffuse & Specular |
| <img src="images/ground_truth_diffuse_only_cornell.png" width="250"/> | <img src="images/ground_truth_diffuse_only_toy.png" width="250"/> | <img src="images/ground_truth_diffuse_only_shaderball.png" width="250"/> | Diffuse Only |
| <img src="images/ground_truth_specular_only_cornell.png" width="250"/> | <img src="images/ground_truth_specular_only_toy.png" width="250"/> | <img src="images/ground_truth_specular_only_shaderball.png" width="250"/> | Specular Only |

## Lightmap Baker

Diffuse:
* A single RGB value containing the result of applying a standard diffuse BRDF to the incoming lighting, with an albedo of 1.0

| Cornell | Toy  | Shaderball | Diffuse |
| --- | ---  | --- | --- |
| <img src="images/baker_diffuse_cornell.png" width="250"/> | <img src="images/baker_diffuse_toy.png" width="250"/> | <img src="images/baker_diffuse_shaderball.png" width="250"/> | Diffuse & Specular |
| <img src="images/baker_diffuse_cornell.png" width="250"/> | <img src="images/baker_diffuse_toy.png" width="250"/> | <img src="images/baker_diffuse_shaderball.png" width="250"/> | Diffuse Only |
| No specular | No specular | No specular | Specular Only |

Directional:
* Bakes irradiance based on Enlighten's directional approach

| Cornell | Toy  | Shaderball | Directional |
| --- | ---  | --- | --- |
| <img src="images/baker_directional_cornell.png" width="250"/> | <img src="images/baker_directional_toy.png" width="250"/> | <img src="images/baker_directional_shaderball.png" width="250"/> | Diffuse & Specular |
| <img src="images/baker_directional_diffuse_only_cornell.png" width="250"/> | <img src="images/baker_directional_diffuse_only_toy.png" width="250"/> | <img src="images/baker_directional_diffuse_only_shaderball.png" width="250"/> | Diffuse Only |
| <img src="images/baker_directional_specular_only_cornell.png" width="250"/> | <img src="images/baker_directional_specular_only_toy.png" width="250"/> | <img src="images/baker_directional_specular_only_shaderball.png" width="250"/> | Specular Only |

Half-Life 2:
* Directional irradiance projected onto the Half-Life 2 basis, making for a total of 3 sets of RGB coefficients (9 floats total)

| Cornell | Toy  | Shaderball | Half-Life 2 |
| --- | ---  | --- | --- |
| <img src="images/baker_hl2_cornell.png" width="250"/> | <img src="images/baker_hl2_toy.png" width="250"/> | <img src="images/baker_hl2_shaderball.png" width="250"/> | Diffuse & Specular |
| <img src="images/baker_hl2_cornell.png" width="250"/> | <img src="images/baker_hl2_toy.png" width="250"/> | <img src="images/baker_hl2_shaderball.png" width="250"/> | Diffuse Only |
| No specular | No specular | No specular | Specular Only |
