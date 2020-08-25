This project is a toy lightmapper completely based on Houdini VEX nodes. Basically it grew out of a re-implementation of <a href="https://mynameismjp.wordpress.com/2016/10/09/sg-series-part-6-step-into-the-baking-lab/" target="_blank">Matt's C++ Baking Lab</a>.

# How To Use It

## Scene Setup

The project includes three scenes to test different baking methods. 

## Path Tracer

The ground truth path tracing results of the three scenes are listed as follows:

<img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/ground_truth_cornell.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/ground_truth_toy.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/ground_truth_shaderball.png" width="200">

## Lightmap Baker

Diffuse:

<img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_diffuse_cornell.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_diffuse_toy.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_diffuse_shaderball.png" width="200">

Directional:

<img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_directional_cornell.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_directional_toy.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_directional_shaderball.png" width="200">

HL2:

<img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_hl2_cornell.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_hl2_toy.png" width="200"> <img src="https://github.com/candycat1992/LightmapperToy/blob/master/images/baker_hl2_shaderball.png" width="200">