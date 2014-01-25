defect-tolerance-nanoarchitectures
==================================

Mapping algorithms for fault tolerance in nano-scale crossbars architectures 

## Introduction
Crossbar architecture is a general approach of molecular electronics and it is consider to be
a very promising technology for constructing memory and other logic functions. Each crosspoint 
can be activated as an electronic device, such as resistor, diode, or transistor and can be 
independently configured to be ON or OFF. However, the main disadvantage of this architecture 
is that a substantial amount of defects is arisen during manufacturing process, in contrast to 
the traditional lithography-based technology. Although the number of defects is considered to 
be very high, manufacturers cannot withdraw all the defective devices from the market because 
manufacturing cost will be increased. Various methods and techniques have been proposed in order 
to overcome with this drawback and provide defect-tolerance systems. In particular, by using 
redundant components, some of them work at the hardware level, while some others are based on 
software fault tolerance methods and techniques. This project is focused only on the second area.
The second ones benefit from the redundant components of a crossbar, by mapping the given
logic function on the correct crosspoints and reject the defective ones. Therefore, a partially-
defective crossbar will still be used with reduced functionality.

Here I have the implementation of 4 different mapping algorithms. The key idea of the algorithms 
is that within the original n × n partially-defective crossbar a reduced k × k defect-free crossbar 
(subset) can be found, on which the logic function is mapped. Therefore, the maximum possible
k should be found from the given n, which is based both on the defect rate of the crossbar and the 
efficiency of the particular mapping algorithm.

## Mapping algorithms
The idea behind each algorithm can be found in:
* M.B. Tahoori. A mapping algorithm for defect-tolerance of reconfigurable nano-
architectures. pages 668–672. Computer-Aided Design, 2005. ICCAD-2005. IEEE/ACM
International Conference on, Nov. 2005. (exact.c, tahoori.c)
* A.A. Al-Yamani, S. Ramsundar, and D.K. Pradhan. A defect tolerance scheme for
nanotechnology circuits. Circuits and Systems I: Regular Papers, IEEE Transactions
on, 54(11):2402–2409, Nov. 2007. (ram.c)
* Costas Argyrides, G. Dimosthenous, Pradhan D.K., C. A. Lisboa, and L. Carro. Re-
liability aware yield improvement technique for nanotechnology based circuits. ACM, 2009.
(costas.c)  

## Requirements
* gcc compiler
* Windows 

## How to run
The algorithms take as input 3 parameters
* N (NxN partially-defective crossbar)
* P (defect rate %)
* T (number of samples)
