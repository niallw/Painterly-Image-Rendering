# Painterly Image Rendering

This is my implementation of the paper by [Aaron Hertzmann "Painterly rendering with curved brush strokes of multiple sizes"](https://dl.acm.org/citation.cfm?id=280951). 

This was done for a final project in CSC 361 - Computer Graphics.
A full write-up can be found [here]().

To use this program, you must supply the path to a **.ppm image.**

We are able to render images in different painted styles, including impressionist and pointillist. The render's appearance depends on:
    - Minimum brush size
    - Brush size ratio 
    - Number of brushes used
    - New stroke threshold
    - Minimum and maximum stroke length
    - Curvature filture (determines how much curviness strokes have)

To paint images, we apply a blur to the original image with an intensity based on the size of the current brush being used to paint. We compare the blurred image to our current canvas to determine which areas should be painted with new strokes. We model strokes using cubic B-splines.

### Sample Renders

![X](sample-images/profile.png) ![X](sample-images/profile_painted.png)  

![X](sample-images/donald.jpg)
![X](sample-images/donald_painted.png)  

![X](sample-images/chambers6.jpg)
![X](sample-images/chambers6_painted.png)  

![X](sample-images/tree.jpg)
![X](sample-images/tree_painted.png)  
![X](sample-images/tree_painted_curvy.png)  

![X](sample-images/manatee.jpeg)
![X](sample-images/manatee_painted_curvy.png)  

![X](sample-images/leaves.jpg)
![X](sample-images/leaves_painted.png)  

![X](sample-images/lights.jpg)
![X](sample-images/lights_painted_point.png)  

![X](sample-images/snow2.jpg)
![X](sample-images/snow2_painted_high_curve.png)   

![X](sample-images/japan.jpg)
![X](sample-images/japan_painted.png)  

![X](sample-images/flowers.jpg)
![X](sample-images/flowers_painted_fixed.png)  
![X](sample-images/flowers_pointillist.png)  

![X](sample-images/colosseum.jpg)
![X](sample-images/colosseum_painted.png)  

### Useful Links

[Interactive B-spline drawing tool](http://nurbscalculator.in/)
<!-- via https://stackoverflow.com/questions/53564615/how-do-i-get-the-b-spline-curve-to-connect-to-the-final-control-point -->

[Another interactive B-spline drawing tool](http://research.engr.utexas.edu/cagd/B-Spline-Interaction/)