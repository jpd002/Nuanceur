# NewFloat*
  
NewFloat* functions taking literals are provided when all parameters are literals.

ex.: `NewFloat4(float, float, float, float)`

Otherwise, a vector must be defined using `Float*Value` parameters (through swizzling or using NewFloat* operations).

ex.: `NewFloat4(value1->xy(), NewFloat2(1, 1))`
