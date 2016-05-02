$fn=100;
difference(){

union(){
 linear_extrude(height = 2, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="0");
   
    linear_extrude(height = 13, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="int");
   
       linear_extrude(height = 10, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="ext");
   
    linear_extrude(height = 8, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="case_int");
    
     linear_extrude(height = 5, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="pcb_holder");
    
         linear_extrude(height = 5, center = false, convexity = 30)
   import (file = "vote_box.dxf",layer="capot_fix");
   
}
translate([72/2-10/2,0,3.5]) cube([10,10,5]);
translate([0,16,5]) cube([10,10,4]);
}
   
