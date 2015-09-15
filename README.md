# NPR-Comic-Book-and-Sketchy-Effect

Non Photorealistic rendering(NPR) can help us use computers to produce images that look like hand drawings. Generally, computer graphics primarily focuses on producing images which are as precise as photographs. However, photographs are not always good for visual effect. Sometimes we need some artistic styles for animated cartoons, painting, video games and so on.

The purpose of this project was to show the results that one can expect from NPR. The procedure involved was the implementation of toon shading and pencil sketching style by doing calculations regarding the edges, lighting and principal curvature. The results were various images of three different forms of NPR.

1. Toon Shading
Our idea of implementing cel shading is to create banded regions of color to represent varying levels of lighting. Then we went with four steps. First, decrease the number of directional light sources to one and adjust its distance and rotation to make the resulting image looks more cartoon­like. Second, check frame buffer pixel by pixel and change the original color to three flat colors according to thresholds. Third, add some random noise to each pixel by adding a small random deviation to RGB value of each pixel. Last, draw outline of the object and merge the outline with cel shading.

2. Sketchy Style
For the sketchy part, we noticed that when people are drawing, the stroke directions generally follow the principal curvature directions. To apply this rule, we calculated curvature directions for each vertex as preprocessing. Then 
