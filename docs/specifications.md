## From latitude and longitude to 32 bits unsigned integers
-180° to 180° longitude mapped to 0 0xFFFFFFFF (biggest unsigned 32 bits int) for internal program use.
 
latitude is mapped to unsigned 32 bit int using mercator projection.
0 being the most far south and 0xFFFFFFFF north.
    
    double angleRadians  =  angle * M_PI / 180.0;
     bool signe = false;
     if(angleRadians < 0) {
        angleRadians  = -1.0 * angleRadians;
        signe = true;
     }
    
    double dist = log(tan((angleRadians/2.0 + M_PI/4.0)));
    
    uint32_t normalized = 0;
    
    if(signe) normalized = 0b1000000000000000000000000000000  * (1 + dist/(2.5 * M_PI));
    else normalized =       0b1000000000000000000000000000000 * (1 - dist/(2.5 * M_PI));


## Dividing the world
![image](world.jpg)

world is divided in rectangles, we divide horizontally, then vertically:
1 big part, then 2 smaller parts then 4 even smaller parts and so on 64 times... parts include lower x and y boudaries, not upper which are in the next part.
## ordering the parts
As we want to use theses parts of map as an index, we have to order them. Let chose this way of ordering :

lets associate a number z to each (x,y) position, x an y being 32 bits unsigned integers,( most significant bit on the left )

if x is is in binary

x1 x2 x3 x4 x5 X6 x7 x8 x9 x10 x11  .... x32 each xn beeing 0 or 1

and y

y1 y2 y3 ..... y32

let our number be

z = x1 y1 x2 y2 x3 y3 .... x32 y32 making an unsigned 64 bits integer, x and y bits ar interleaved to make z.

the range  z1 z2 ... zn 0 0 0 0 ... 0 to z1 z2 ... zn 1 1 1 1 will represent a rectangle that we call geobox

let's call z1 z2 ... zn 0 ... 0 the position of the geobox

the number of bits varying from 0 to 1 the masklength of the geobox

we choose to order them this way

the "smaller" geobox is the one which the smaller z number , or the biggest masklength if z numbers  are equal.

we have divided the map in rectangles and ordered them :

as an example, the 7 first geoboxes :



z = 00000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 64

![image](world1.jpg)

z = 00000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 63

![image](world2.jpg)

z = 00000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 62

![image](world3.jpg)

z = 01000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 62

![image](world4.jpg)

z = 10000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 63

![image](world5.jpg)

z = 10000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 62

![image](world6.jpg)

z = 11000000 00000000 000000000 00000000 00000000 00000000 000000000 00000000
masklength is 62

![image](world6.jpg)
