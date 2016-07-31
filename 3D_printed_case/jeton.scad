//cylinder(d=23.21,h=2);
$fn=100;
linear_extrude(height = 2, center = false, convexity = 100)
   import (file = "jeton.dxf",layer="0"); 