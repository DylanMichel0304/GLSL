#version 330 core
layout (location = 0) in vec3 in_Position;

uniform float time;
uniform mat4 ModelView;
uniform mat4 Projection;

out float percentTravel_For_Frag;

// Constants for fire animation
float timeToLoop = 3.0;  // Time for a particle to go from bottom to top
float travelDist = 2.5;  // Increased travel distance for particles

// Calculate a wiggling effect for particles
float wiggle(float vertex_id, float percentTravel)
{
    float wiggle_result_raw1 = vertex_id + percentTravel + time;
    float wiggle_result_raw2 = vertex_id + percentTravel + time * 0.8;
    float wiggle_result_raw3 = vertex_id + percentTravel + time * 1.2;

    float final_wiggle_result1 = sin(wiggle_result_raw1);
    float final_wiggle_result2 = sin(wiggle_result_raw2);
    float final_wiggle_result3 = sin(wiggle_result_raw3);

    // Weighted average for smoother motion
    float weighted_average_results = (final_wiggle_result1 + final_wiggle_result2 + final_wiggle_result3) / 3.0;

    return weighted_average_results;
}

void main() 
{
    // Calculate particle movement with time
    float localParticleTime = (time + gl_VertexID) / 10000.0;
    float pointMove = mod(time + travelDist * localParticleTime, travelDist);
    
    // Calculate percent of total height traveled
    float percentTravel = pointMove / travelDist;
    
    // Generate wiggle effect on X and Z axes
    float wiggle_x = wiggle(gl_VertexID, percentTravel);
    float wiggle_z = wiggle(mod(gl_VertexID, 40.0), percentTravel);
    
    // Reduced particle size for smaller appearance
    gl_PointSize = 12.0 * (1.0 - percentTravel / 3.5);
    
    // Make particles at the base of the fire slightly larger
    if (percentTravel < 0.2) {
        gl_PointSize *= 1.2;
    }

    // Calculate final position with wiggle effect
    vec3 position = in_Position;
    position.x += wiggle_x * (percentTravel / 2.5);
    
    // Use a more gradual, natural vertical movement curve
    position.y += pointMove * pow(percentTravel, 0.6);
    
    position.z += wiggle_z * (percentTravel / 2.5);
    
    // Set final position and pass travel percentage to fragment shader
    gl_Position = Projection * ModelView * vec4(position, 1.0);
    
    // Pass travel percentage to fragment shader for color blending
    percentTravel_For_Frag = percentTravel;
} 