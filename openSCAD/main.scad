module prism(l, w, h){
       polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
               );

       }
       
difference() {       
translate([10, 10, 0]) color("DarkViolet") cube([35.6, 24.2, 1]);
translate([12.4,21.43,-0.1]) rotate([0, -90, 90]) prism(2, 2.6, 1.2);
translate([9.9, 21.42,-0.1]) cube([2.6, 20, 2]);
};