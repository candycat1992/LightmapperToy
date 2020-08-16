// function vector PathTrace(PathTracerParams params; Ray ray)
// {
//     vector ret = {0.0f, 0.0f, 0.0f};
//     vector throughput = {1.0f, 1.0f, 1.0f};
//     vector rayPos = ray.origin;
//     vector rayDir = ray.direction;
    
//     for (int bounceIndex = 0; bounceIndex <= c_numBounces; ++bounceIndex)
//     {
//         // shoot a ray out into the world
//         vector hitPos, hitUVW;
//         int hitPrim = intersect(1, rayPos + rayDir * params.rayMinDistance, rayDir * params.rayMaxDistance, hitPos, hitUVW);
        
//         // if the ray missed, we are done
//         if (hitPrim < 0)
//         {
//             ret += SampleEnvironment(rayDir) * throughput;
//             break;
//         }

//         vector normal = primuv(1, 'N', hitPrim, hitUVW);

//         // Look up the material data
//         vector albedo = {0.1f, 0.1f, 0.1f};
//         vector emissive = {0.0f, 0.0f, 0.0f};   
//         float percentSpecular = 0.3f;
//         float roughness = 0.2;
//         vector specularColor = {0.9f, 0.9f, 0.9f};   
        
// 		// update the ray position
//         rayPos = hitPos + normal * params.rayPosNormalNudge;

//         rayDir = normalize(normal + sample_sphere_uniform(set(nrandom(), nrandom(), nrandom())));

//         // add in emissive lighting
//         ret += emissive * throughput;

//         // update the colorMultiplier
//         throughput *= albedo;
        
//         // // calculate whether we are going to do a diffuse or specular reflection ray 
//         // float doSpecular = (random(rngState) < percentSpecular) ? 1.0f : 0.0f;
 
//         // // Calculate a new ray direction.
//         // // Diffuse uses a normal oriented cosine weighted hemisphere sample.
//         // // Perfectly smooth specular uses the reflection ray.
//         // // Rough (glossy) specular lerps from the smooth specular to the rough diffuse by the material roughness squared
//         // // Squaring the roughness is just a convention to make roughness feel more linear perceptually.
//         // vector diffuseRayDir = normalize(normal + RandomUnitVector(rngState));
//         // vector specularRayDir = reflect(rayDir, normal);
//         // specularRayDir = normalize(lerp(specularRayDir, diffuseRayDir, roughness * roughness));
//         // rayDir = lerp(diffuseRayDir, specularRayDir, doSpecular);
        
// 		// // add in emissive lighting
//         // ret += emissive * throughput;
        
//         // // update the colorMultiplier
//         // throughput *= lerp(albedo, specularColor, doSpecular);      
        
//         // // Russian Roulette
//         // // As the throughput gets smaller, the ray is more likely to get terminated early.
//         // // Survivors have their value boosted to make up for fewer samples being in the average.
//         // {
//         // 	float p = max(throughput.r, max(throughput.g, throughput.b));
//         // 	if (random(rngState) > p)
//         //     	break;

//         // 	// Add the energy we 'lose' by randomly terminating paths
//         // 	throughput *= 1.0f / p;            
//         // }
//     }
 
//     // return pixel color
//     return ret;
// }



// function vector RandomUnitVector(int seed)
// {
//     float z = random(seed) * 2.0 - 1.0;
//     float a = random(seed) * PI * 2.0;
//     float r = sqrt(1.0 - z * z);
//     float x = r * cos(a);
//     float y = r * sin(a);
//     return set(x, y, z);
// }