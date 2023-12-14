#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{
    // Use texCoords from the previous shader (assuming it's unchanged)
    vec2 tc = texCoords;
    
    // Sample the surrounding pixels
    vec4 centerColor = texture(screenTexture, tc);
    vec4 leftColor = texture(screenTexture, tc - vec2(texOffset.x, 0.0));
    vec4 rightColor = texture(screenTexture, tc + vec2(texOffset.x, 0.0));
    vec4 topColor = texture(screenTexture, tc - vec2(0.0, texOffset.y));
    vec4 bottomColor = texture(screenTexture, tc + vec2(0.0, texOffset.y));

    // Calculate the intensity gradients
    float horizontalGradient = length(leftColor - rightColor);
    float verticalGradient = length(topColor - bottomColor);

    // Combine gradients to detect edges
    float edgeIntensity = sqrt(horizontalGradient * horizontalGradient + verticalGradient * verticalGradient);

    // Apply the threshold to highlight edges
    vec4 edgeColor = edgeIntensity > thresholdLow ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);

    // Perform hysteresis to link edges
    edgeColor = edgeColor * (edgeIntensity > thresholdHigh ? 1.0 : 0.0);

    // Output the edge-detected color
    FragColor = edgeColor;
}