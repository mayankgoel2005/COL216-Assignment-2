# Design Decisions

## Latch Structs and Processor Class
- We made the program modular by modelling the IF-ID, ID-EX, EX-MEM and MEM-WB latches as structs. We have kept all the control signals as members of the respective structs.
- Other members of the latches include: Program Counter for each struct (to denote the instruction present in the respective stage), flag for last instruction (end of pipeline), NOP flag, and ON member to detect stalls.
- We created forwarded and non-forwarded processor classes and instantiated them with the four latch structs and other components of the processor like a memory-simulating array, register array and an instruction vector.

## Executing and Printing the Pipeline
- We have modelled the clock by executing the pipeline cycle by cycle. The output obtained from this algorithm is a vertical, skewed one and without dashes (-) in stalls. To convert this to the required form, we store the stage code in an answer matrix, which is printed at the end of the pipeline. For example, setting ans(i , j)=2 will print ID in the j th cycle of the i th instruction.
- At any cycle, we execute the pipeline stages in an inverted order i.e. the stage functions are called in the order WB->MEM->EX->ID->IF. This is based on the intution that we are basically trying to model parallel hardware through sequential algorithms. To illustrate this, consider arbitrary members L1.x, L2.x and L3.x: if the ID stage is called before EX stage, we propagate the members by doing L2.x=L1.x and hence the data in L2.x is lost (can't be propagated to L3). Calling EX before ID preserves this data as L3.x is set to L2.x first and then L2.x is set to L1.x

## Memory and Register Layout
- We simulated the memory component by creating an arbitrary 1024-length vector. Each element in the vector models a byte in the memory.
- Correspondingly, we have implemented the load byte, load word and load double instructions by taking 1, 4 and 8 bytes starting at the computed memory address. The format followed is little endian. Same for store byte, store word and store double.
- Simulated the registers by creating a 32-size vector. We have not hardwired the x0 register to zero value in our design, so technically it is possible to make it non-zero.

