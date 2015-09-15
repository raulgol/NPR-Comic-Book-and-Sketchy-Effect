<snippet>
  <content>
# NPR: Comic Book and Sketchy Effect
## Demo
Demo: https://sites.google.com/a/usc.edu/oculusdrift/project/demo 
<br />
Website: https://sites.google.com/a/usc.edu/oculusdrift/
## Introduction
Non Photorealistic rendering(NPR) can help us use computers to produce images that look like hand drawings. Generally, computer graphics primarily focuses on producing images which are as precise as photographs. However, photographs are not always good for visual effect. Sometimes we need some artistic styles for animated cartoons, painting, video games and so on.
## Implementation
1. Toon Shading
Our idea of implementing cel shading is to create banded regions of color to represent varying levels of lighting. Then we went with four steps. First, decrease the number of directional light sources to one and adjust its distance and rotation to make the resulting image looks more cartoon­like. Second, check frame buffer pixel by pixel and change the original color to three flat colors according to thresholds. Third, add some random noise to each pixel by adding a small random deviation to RGB value of each pixel. Last, draw outline of the object and merge the outline with cel shading.

2. Sketchy Style
For the sketchy part, we noticed that when people are drawing, the stroke directions generally follow the principal curvature directions. To apply this rule, we calculated curvature directions for each vertex as preprocessing. We also prepared some texture images with different intensity of strokes. After preprocessing, we draw strokes by selecting appropriate stroke textures and rotate them according to curvature. Finally the strokes look natural and continuous.

></content>
  <tabTrigger></tabTrigger>
</snippet>




