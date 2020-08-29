# LightmapperToy

This project is a hobby lightmapper completely based on Houdini VEX nodes. Basically it grew out of a re-implementation of <a href="https://github.com/TheRealMJP/BakingLab" target="_blank">Matt's The Baking Lab</a> with some modification. It's my first houdini project so there is no magic (no GPU acceleration, no progressive, just brute force) :(

# How To Use It

There are just four nodes inside the obj tab:

<img src="images/scene_nodes.png" width="600"/>

The SCENE_ROOM node is responsible for setting up the scene geometries (click the "Room Object" menu to change different scenes, toggle the "Enable Light" to enable or hide the area light on the top):

<img src="images/scene_room_node.png" width="600"/>

The RENDERER node is responsible for all the calculating. Change the "Shading Method" menu to switch between path tracer and lightmapper. More detail settings are under "Ground Truth" and "Lightmap Baker" tabs:

<img src="images/renderer_node.png" width="600"/>

# Results

We compare the lighting results of all the methods. As usual, ground truth goes first.

## Path Tracer

| Cornell | Toy  | Shaderball | Path Tracer |
| --- | ---  | --- | --- |
| <img src="images/ground_truth_cornell.png" width="200"/> | <img src="images/ground_truth_toy.png" width="200"/> | <img src="images/ground_truth_shaderball.png" width="200"/> | Diffuse & Specular |
| <img src="images/ground_truth_cornell_diffuse.png" width="200"/> | <img src="images/ground_truth_toy_diffuse.png" width="200"/> | <img src="images/ground_truth_shaderball_diffuse.png" width="200"/> | Diffuse Only |
| <img src="images/ground_truth_cornell_specular.png" width="200"/> | <img src="images/ground_truth_toy_specular.png" width="200"/> | <img src="images/ground_truth_shaderball_specular.png" width="200"/> | Specular Only |

## Diffuse Baker

"Diffuse" uses a single RGB value containing the result of applying a standard diffuse BRDF to the incoming lighting, with an albedo of 1.0 (3 floats per point).

| Cornell | Toy  | Shaderball | Diffuse |
| --- | ---  | --- | --- |
| <img src="images/baker_diffuse_cornell.png" width="200"/> | <img src="images/baker_diffuse_toy.png" width="200"/> | <img src="images/baker_diffuse_shaderball.png" width="200"/> | Diffuse & Specular |
| -- | -- | -- | Diffuse Only |
| No Specular | No Specular | No Specular | Specular Only |

### Directional Baker

"Directional" bakes irradiance based on Enlighten's directional approach, with 3 floats for color, 3 for lighting main direction information, and 1 float to ensure that the directional term evaluates to 1 when the surface normal aligns with normal used when baking (7 floats per point)

| Cornell | Toy  | Shaderball | Directional |
| --- | ---  | --- | --- |
| <img src="images/baker_directional_cornell.png" width="200"/> | <img src="images/baker_directional_toy.png" width="200"/> | <img src="images/baker_directional_shaderball.png" width="200"/> | Diffuse & Specular |
| <img src="images/baker_directional_cornell_diffuse.png" width="200"/> | <img src="images/baker_directional_toy_diffuse.png" width="200"/> | <img src="images/baker_directional_shaderball_diffuse.png" width="200"/> | Diffuse Only |
| <img src="images/baker_directional_cornell_specular.png" width="200"/> | <img src="images/baker_directional_toy_specular.png" width="200"/> | <img src="images/baker_directional_shaderball_specular.png" width="200"/> | Specular Only |

### Half-Life 2 Baker

"Half-Life 2" bakes irradiance projected onto the Half-Life 2 basis, making for a total of 3 sets of RGB coefficients (9 floats per point).

| Cornell | Toy  | Shaderball | Half-Life 2 |
| --- | ---  | --- | --- |
| <img src="images/baker_hl2_cornell.png" width="200"/> | <img src="images/baker_hl2_toy.png" width="200"/> | <img src="images/baker_hl2_shaderball.png" width="200"/> | Diffuse & Specular |
| -- | -- | -- | Diffuse Only |
| No Specular | No Specular | No Specular | Specular Only |

### SH L1/L2 Baker

"SH" bakes irradiance projected onto L1 SH (12 floats per point) or L2 SH basis (27 floats per point). SH spports 3 diffuse modes and 5 specular modes to shading. There are too many possible combinations of diffuse and specular modes (3 x 5), so I just use the default modes (convolution for both diffuse and specular) for the first row.

| Cornell | Toy  | Shaderball | SH L2 |
| --- | ---  | --- | --- |
| <img src="images/baker_shl1_cornell.png" width="200"/> | <img src="images/baker_shl1_toy.png" width="200"/> | <img src="images/baker_shl1_shaderball.png" width="200"/> | Diffuse & Specular |
| <img src="images/baker_shl1_cornell_diffuse_convolution.png" width="200"/> | <img src="images/baker_shl1_toy_diffuse_convolution.png" width="200"/> | <img src="images/baker_shl1_shaderball_diffuse_convolution.png" width="200"/> | Diffuse (Convolution) |
| <img src="images/baker_shl1_cornell_diffuse_geomerics.png" width="200"/> | <img src="images/baker_shl1_toy_diffuse_geomerics.png" width="200"/> | <img src="images/baker_shl1_shaderball_diffuse_geomerics.png" width="200"/> | Diffuse (Geomerics) |
| <img src="images/baker_shl1_cornell_diffuse_ahd.png" width="200"/> | <img src="images/baker_shl1_toy_diffuse_ahd.png" width="200"/> | <img src="images/baker_shl1_shaderball_diffuse_ahd.png" width="200"/> | Diffuse (AHD) |
| <img src="images/baker_shl1_cornell_specular_convolution.png" width="200"/> | <img src="images/baker_shl1_toy_specular_convolution.png" width="200"/> | <img src="images/baker_shl1_shaderball_specular_convolution.png" width="200"/> | Specular (Convolution) |
| <img src="images/baker_shl1_cornell_specular_dominant.png" width="200"/> | <img src="images/baker_shl1_toy_specular_dominant.png" width="200"/> | <img src="images/baker_shl1_shaderball_specular_dominant.png" width="200"/> | Specular (Dominant) |
| <img src="images/baker_shl1_cornell_specular_punctual.png" width="200"/> | <img src="images/baker_shl1_toy_specular_punctual.png" width="200"/> | <img src="images/baker_shl1_shaderball_specular_punctual.png" width="200"/> | Specular (Punctual) |
| <img src="images/baker_shl1_cornell_specular_prefiltered.png" width="200"/> | <img src="images/baker_shl1_toy_specular_prefiltered.png" width="200"/> | <img src="images/baker_shl1_shaderball_specular_prefiltered.png" width="200"/> | Specular (Prefiltered) |
| <img src="images/baker_shl1_cornell_specular_ahd.png" width="200"/> | <img src="images/baker_shl1_toy_specular_ahd.png" width="200"/> | <img src="images/baker_shl1_shaderball_specular_ahd.png" width="200"/> | Specular (AHD) |

## H-basis L1/L2 Baker

TODO

## SG Baker

TODO

# References

[1] <a href="https://mynameismjp.wordpress.com/2016/10/09/sg-series-part-6-step-into-the-baking-lab/" target="_blank">SG Series Part 6: Step Into The Baking Lab</a>
[2] <a href="https://www.ppsloan.org/publications/StupidSH36.pdf" target="_blank">Stupid Spherical Harmonics (SH) Tricks</a>
[3] <a href="https://seblagarde.wordpress.com/2011/10/09/dive-in-sh-buffer-idea/" target="_blank">Dive in SH Buffer Idea</a>
[4] <a href="https://www.ea.com/frostbite/news/precomputed-global-illumination-in-frostbite" target="_blank">Precomputed Global Illumination in Frostbite</a>
[5] <a href="http://miciwan.com/SIGGRAPH2013/Lighting%20Technology%20of%20The%20Last%20Of%20Us.pdf" target="_blank">Lighting Technology of The Last of Us</a>