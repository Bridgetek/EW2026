# EVE-MCU-Dev Example Code Snippets

[Back](../README.md)

The `snippets` directory contains code that is used in the examples for the Python BT82x Development moduleEVE-MCU-Dev code. The following code is available in that directory:

## Contents

- [Utilities](#Utilities)
  - [Trigonometry using Furmans](#Trigonometry-using-Furmans)
## Utilities

### Trigonometry using Furmans

A utility is provided to perform trigonometery using angles in furmans rather than degrees or radians. 

The return value is an `int16_t` format ranging from -0x8000 to +0x7fff.

_Calling format:_

   `sin_furman(angle);`
   `cos_furman(angle);`

_Parameters:_

-   int16_t **angle**: Angle in furmans

_Example:_

```
uint16_t scale = sin_furman(0xd000);
```

#### Trigonometry Furman Macros

Macros are provided to turn degrees into furmans and vice versa. Care needs to be taken as the accuracy in degrees is integer which is much less than the accuracy of furmans.

_Calling format:_

   `FURMAN2DEG(furman);`
   `DEG2FURMAN(degrees);`

_Parameters:_

-   int16_t **furman**: Angle in furmans
-   int **degrees**: Angle in degrees

_Example:_

```
int16_t angle = DEG2FURMAN(0x4000); // angle = 90
```

#### Trigonometry Circle Macros

Macros are provided to turn a radius and degrees/furmans into components for X and Y vector. Note that the return value is signed.

_Calling format:_

   `CIRC_X(radius, furman);`
   `CIRC_X_DEG(radius, degrees);`
   `CIRC_Y(radius, furman);`
   `CIRC_Y_DEG(radius, degrees);`

_Parameters:_

-   int **radius**: Radius of vector
-   int16_t **furman**: Angle in furmans
-   int **degrees**: Angle in degrees

_Example:_

```
// angle = 45
int16_t vx = CIRC_X(0x2000); 
int16_t vy = CIRC_Y(0x2000); 
```
